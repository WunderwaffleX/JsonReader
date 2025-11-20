#include "controller/MainController.hpp"
#include "core/FileTypes.hpp"
#include "view/EditDialog.hpp"
#include <QCheckBox>
#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>

MainController::MainController(MainWindow *view, TableModel *model,
                               QObject *parent)
    : QObject(parent), view(view), model(model) {

    initView();
    initDbWorker();

    emit loadAllRequested();
}

void MainController::initView() {
    view->getTableView()->setModel(model);

    connect(view->getClearButton(), &QPushButton::clicked, this,
            &MainController::clearTable);

    connect(view->getImportDirButton(), &QPushButton::clicked, this,
            &MainController::importDirectory);

    connect(view->getImportFileButton(), &QPushButton::clicked, this,
            &MainController::importFile);

    connect(view->getTableView(), &QWidget::customContextMenuRequested, this,
            &MainController::showContextMenu);
}

void MainController::initDbWorker() {
    QString dbPath = QCoreApplication::applicationDirPath() + "/data.db";

    dbThread = new QThread(this);
    dbWorker = new DbWorker(dbPath);

    dbWorker->moveToThread(dbThread);

    connect(dbThread, &QThread::finished, dbWorker, &QObject::deleteLater);

    connect(this, &MainController::loadAllRequested, dbWorker,
            &DbWorker::loadAll);

    connect(this, &MainController::insertDataRequested, dbWorker,
            &DbWorker::insertData);

    connect(dbWorker, &DbWorker::dataInserted, this,
            &MainController::onDataInserted);

    connect(dbWorker, &DbWorker::loaded, this,
            [this](const QVector<DataObject> &rows) {
                this->model->setDataObjects(rows);
            });

    connect(dbWorker, &DbWorker::error, this, [this](const QString &msg) {
        QMessageBox::warning(this->view, "DB Error", msg);
    });

    connect(this, &MainController::editRowRequested, dbWorker,
            &DbWorker::editRow);

    connect(this, &MainController::deleteRowRequested, dbWorker,
            &DbWorker::removeRowById);

    connect(dbWorker, &DbWorker::rowEdited, this,
            [this](const DataObject &obj) {
                int row = model->rowById(obj.id);
                if (row != -1) {
                    model->updateRow(row, obj);
                }
            });

    connect(dbWorker, &DbWorker::removed, this, [this](int id) {
        int row = model->rowById(id);
        if (row != -1) {
            model->removeRow(row);
        }
    });

    connect(this, &MainController::clearRequested, dbWorker,
            &DbWorker::clearAll);

    connect(dbThread, &QThread::started, dbWorker, &DbWorker::initDatabase);

    connect(dbWorker, &DbWorker::cleared, model, &TableModel::clear);

    dbThread->start();
}

void MainController::startFileWorker(const QStringList &paths) {
    if (paths.isEmpty())
        return;

    importDialog = new ImportDialog(view);
    importDialog->show();

    fileThread = new QThread(this);
    fileWorker = new FileWorker(paths);

    fileWorker->moveToThread(fileThread);

    connect(fileThread, &QThread::started, fileWorker, &FileWorker::process);

    connect(fileWorker, &FileWorker::finished, this,
            &MainController::onFileWorkerFinished);

    connect(fileWorker, &FileWorker::fileError, importDialog,
            &ImportDialog::addError);

    connect(fileWorker, &FileWorker::fileParsed, importDialog,
            &ImportDialog::addParsed);

    connect(fileWorker, &FileWorker::progressUpdated, importDialog,
            &ImportDialog::updateProgress);

    connect(importDialog, &QDialog::rejected, this, [this]() {
        if (fileThread) {
            fileThread->quit();
            fileThread->wait();
        }
    });

    connect(fileThread, &QThread::finished, fileWorker, &QObject::deleteLater);

    fileThread->start();
}

void MainController::importFile() {
    QString path = QFileDialog::getOpenFileName(
        view, "Select JSON/XML file", QString(), FileConst::DialogFilters());
    if (path.isEmpty())
        return;

    startFileWorker({path});
}

void MainController::importDirectory() {
    QString dirPath =
        QFileDialog::getExistingDirectory(view, "Select directory");

    if (dirPath.isEmpty())
        return;

    QDir dir(dirPath);

    QStringList files = dir.entryList(FileConst::AllMasks(), QDir::Files);

    QStringList paths;
    for (const QString &f : files)
        paths << dir.absoluteFilePath(f);

    if (paths.isEmpty()) {
        QMessageBox::information(view, "No files",
                                 "No supported files found in the directory.");
        return;
    }

    startFileWorker(paths);
}

void MainController::onFileWorkerFinished(const QVector<DataObject> &data) {
    emit insertDataRequested(data);

    fileThread->quit();
    fileThread->wait();
    fileThread->deleteLater();

    fileThread = nullptr;
    fileWorker = nullptr;
}

void MainController::onDataInserted(const QVector<DataObject> &dataWithIds) {
    model->appendDataObjects(dataWithIds);
}

void MainController::onFileWorkerError(const QString &path,
                                       const QString &msg) {
    QMessageBox::warning(view, "File error",
                         QString("File: %1\n%2").arg(path, msg));
}

void MainController::onFileWorkerParsed(const QString &path, int count) {
    qDebug() << "Parsed:" << path << "->" << count;
}

void MainController::clearTable() {
    emit clearRequested();
}

void MainController::editRow(int row) {
    DataObject obj = model->getObject(row);

    EditDialog dialog(obj, view);

    if (dialog.exec() == QDialog::Accepted) {
        DataObject edited = dialog.getEditedData();

        emit editRowRequested(edited);
    }
}

void MainController::deleteRow(int row) {
    DataObject obj = model->getObject(row);
    emit deleteRowRequested(obj.id);
}

void MainController::exportRow(int row) {
    QVector<DataObject> vec = {model->getObject(row)};

    QString path = QFileDialog::getSaveFileName(view, "Export file", "export",
                                                FileConst::DialogFilters());
    if (path.isEmpty())
        return;

    FileWorker *worker = new FileWorker({}, this);
    connect(worker, &FileWorker::exportFinished, this,
            [this, worker](bool success, const QString &path) {
                if (!success) {
                    QMessageBox::warning(view, "Error",
                                         "Failed to export file: " + path);
                }
                worker->deleteLater();
            });

    worker->exportData(vec, path);
}

void MainController::showContextMenu(const QPoint &pos) {
    QTableView *table = view->getTableView();
    QModelIndex index = table->indexAt(pos);

    if (!index.isValid())
        return;

    table->selectRow(index.row());

    QMenu menu(view);

    QAction *editAct = menu.addAction("Edit");
    QAction *deleteAct = menu.addAction("Delete");
    QAction *exportAct = menu.addAction("Export as");

    QAction *selected = menu.exec(table->viewport()->mapToGlobal(pos));

    if (!selected)
        return;

    if (selected == editAct)
        editRow(index.row());
    else if (selected == deleteAct)
        deleteRow(index.row());
    else if (selected == exportAct)
        exportRow(index.row());
}

MainController::~MainController() {
    if (fileThread) {
        fileThread->quit();
        fileThread->wait();
        fileThread->deleteLater();
    }

    dbThread->quit();
    dbThread->wait();
    dbThread->deleteLater();
}
