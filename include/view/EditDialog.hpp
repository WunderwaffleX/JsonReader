#pragma once

#include "model/DataObject.hpp"
#include <QCheckBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class EditDialog : public QDialog {
    Q_OBJECT

  public:
    explicit EditDialog(const DataObject &obj, QWidget *parent = nullptr);
    DataObject getEditedData() const;

  private:
    QLineEdit *nameEdit;
    QLineEdit *formatsEdit;
    QLineEdit *encodingEdit;
    QCheckBox *intelliEdit;
    QCheckBox *pluginsEdit;
    QCheckBox *compileEdit;

    DataObject currentData;
};
