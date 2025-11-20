#pragma once
#include "DataObject.hpp"
#include <QAbstractTableModel>
#include <QVector>
#include <qabstractitemmodel.h>

class TableModel : public QAbstractTableModel {
    Q_OBJECT
  public:
    explicit TableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    void setDataObjects(const QVector<DataObject> &objs);
    void appendDataObjects(const QVector<DataObject> &objs);

    DataObject getObject(int row) const;

    int rowById(int id) const;

    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    void updateRow(int row, const DataObject &obj);

    void clear();

  private:
    QVector<DataObject> dataObjects;
    QHash<int, int> idToRow;
};
