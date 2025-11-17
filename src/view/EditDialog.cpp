#include "view/EditDialog.hpp"

EditDialog::EditDialog(const DataObject &obj, QWidget *parent)
    : QDialog(parent), currentData(obj)
{
    setWindowTitle("Edit Row");

    nameEdit = new QLineEdit(obj.textEditor, this);
    formatsEdit = new QLineEdit(obj.fileFormats, this);
    encodingEdit = new QLineEdit(obj.encoding, this);

    intelliEdit = new QCheckBox("Intellisense", this);
    intelliEdit->setChecked(obj.hasIntellisense);

    pluginsEdit = new QCheckBox("Plugins", this);
    pluginsEdit->setChecked(obj.hasPlugins);

    compileEdit = new QCheckBox("Can Compile", this);
    compileEdit->setChecked(obj.canCompile);

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(formatsEdit);
    mainLayout->addWidget(encodingEdit);
    mainLayout->addWidget(intelliEdit);
    mainLayout->addWidget(pluginsEdit);
    mainLayout->addWidget(compileEdit);
    mainLayout->addLayout(buttonsLayout);
}

DataObject EditDialog::getEditedData() const
{
    DataObject edited = currentData;
    edited.textEditor = nameEdit->text();
    edited.fileFormats = formatsEdit->text();
    edited.encoding = encodingEdit->text();
    edited.hasIntellisense = intelliEdit->isChecked();
    edited.hasPlugins = pluginsEdit->isChecked();
    edited.canCompile = compileEdit->isChecked();
    return edited;
}

