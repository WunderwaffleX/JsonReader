#pragma once
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>

class ImportDialog : public QDialog {
    Q_OBJECT
  public:
    explicit ImportDialog(QWidget *parent = nullptr);
  public slots:
    void updateProgress(int processed, int total);

    void addError(const QString &file, const QString &msg);

    void addParsed(const QString &file, int count);

  private:
    QLabel *label;
    QProgressBar *progress;
    QTextEdit *log;
    QPushButton *okButton;
};
