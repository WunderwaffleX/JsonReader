#include "view/MainWindow.hpp"
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    tableView = new QTableView(this);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    importDirButton = new QPushButton("Import directory", this);
    importFileButton = new QPushButton("Import file", this);
    clearButton = new QPushButton("Clear", this);

    layout->addWidget(tableView, 1);
    layout->addWidget(importDirButton);
    layout->addWidget(importFileButton);
    layout->addWidget(clearButton);

    central->setLayout(layout);
    setCentralWidget(central);
    resize(800, 600);
}
