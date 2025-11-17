#pragma once
#include <QComboBox>
#include <QMainWindow>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = nullptr);

    QTableView *getTableView() const {
        return tableView;
    }
    QPushButton *getImportDirButton() const {
        return importDirButton;
    }
    QPushButton *getImportFileButton() const {
        return importFileButton;
    }
    QPushButton *getClearButton() const {
        return clearButton;
    }

  private:
    QTableView *tableView;
    QPushButton *importDirButton;
    QPushButton *importFileButton;
    QPushButton *clearButton;
};
