#include "view/ImportDialog.hpp"

ImportDialog::ImportDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Importing Files");
    setModal(true);
    resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);

    label = new QLabel("Starting import...", this);
    progress = new QProgressBar(this);
    progress->setRange(0, 100);
    progress->setValue(0);

    log = new QTextEdit(this);
    log->setReadOnly(true);

    okButton = new QPushButton("OK", this);
    okButton->setEnabled(false);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(label);
    layout->addWidget(progress);
    layout->addWidget(log);
    layout->addWidget(okButton);
}

void ImportDialog::updateProgress(int processed, int total) {
    progress->setMaximum(total);
    progress->setValue(processed);
    label->setText(
        QString("Imported %1 of %2 files").arg(processed).arg(total));

    if (processed >= total) {
        okButton->setEnabled(true);
    }
}

void ImportDialog::addError(const QString &file, const QString &msg) {
    log->append(QString("<span style='color:red;'>[%1] ERROR: %2</span>")
                    .arg(file, msg));
}

void ImportDialog::addParsed(const QString &file, int count) {
    log->append(
        QString("<span style='color:green;'>[%1] Imported %2 files</span>")
            .arg(file)
            .arg(count));
}
