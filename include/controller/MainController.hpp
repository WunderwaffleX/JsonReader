#pragma once
#include "core/DbWorker.hpp"
#include "core/FileWorker.hpp"
#include "model/TableModel.hpp"
#include "view/ImportDialog.hpp"
#include "view/MainWindow.hpp"
#include <QObject>
#include <QThread>

class MainController : public QObject {
    Q_OBJECT

  public:
    MainController(MainWindow *view, TableModel *model,
                   QObject *parent = nullptr);
    ~MainController();

  private:
    MainWindow *view;
    TableModel *model;
    ImportDialog *importDialog = nullptr;

    QThread *dbThread = nullptr;
    DbWorker *dbWorker = nullptr;

    QThread *fileThread = nullptr;
    FileWorker *fileWorker = nullptr;

  private:
    void initView();
    void initDbWorker();
    void startFileWorker(const QStringList &paths);

  private slots:
    void importDirectory();
    void importFile();
    void clearTable();

    void onFileWorkerFinished(const QVector<DataObject> &data);
    void onFileWorkerError(const QString &path, const QString &msg);
    void onFileWorkerParsed(const QString &path, int count);

    void onDataInserted(const QVector<DataObject> &data);

    void editRow(int row);
    void deleteRow(int row);
    void exportRow(int row);
    void showContextMenu(const QPoint &pos);

  signals:
    void loadAllRequested();
    void insertDataRequested(const QVector<DataObject> &rows);
    void editRowRequested(const DataObject &obj);
    void deleteRowRequested(int id);
    void clearRequested();
};
