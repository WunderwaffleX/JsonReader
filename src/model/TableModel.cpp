#include "model/TableModel.hpp"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent) {}

enum Columns {
    COL_NAME = 0,
    COL_FORMATS,
    COL_ENCODING,
    COL_INTELLISENSE,
    COL_PLUGINS,
    COL_COMPILE,
    COL_COUNT
};

int TableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return dataObjects.size();
}

int TableModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return COL_COUNT;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case COL_NAME:
            return "Editor";
        case COL_FORMATS:
            return "Formats";
        case COL_ENCODING:
            return "Encoding";
        case COL_INTELLISENSE:
            return "Intellisense";
        case COL_PLUGINS:
            return "Plugins";
        case COL_COMPILE:
            return "Compile";
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const auto &obj = dataObjects[index.row()];

    switch (index.column()) {

    case COL_NAME:
        return obj.textEditor;

    case COL_FORMATS:
        return obj.fileFormats;

    case COL_ENCODING:
        return obj.encoding;

    case COL_INTELLISENSE:
        return obj.hasIntellisense ? "Yes" : "No";

    case COL_PLUGINS:
        return obj.hasPlugins ? "Yes" : "No";

    case COL_COMPILE:
        return obj.canCompile ? "Yes" : "No";

    default:
        return QVariant();
    }
}

void TableModel::setDataObjects(const QVector<DataObject> &objs) {
    beginResetModel();
    dataObjects = objs;
    endResetModel();
}

void TableModel::appendDataObjects(const QVector<DataObject> &objs) {
    if (objs.isEmpty())
        return;

    const int oldSize = dataObjects.size();
    beginInsertRows(QModelIndex(), oldSize, oldSize + objs.size() - 1);
    dataObjects += objs;
    endInsertRows();
}

DataObject TableModel::getObject(int row) const {
    if (row < 0 || row >= dataObjects.size())
        return DataObject{};
    return dataObjects[row];
}

bool TableModel::removeRow(int row, const QModelIndex &parent) {
    if (row < 0 || row >= dataObjects.size())
        return false;
    beginRemoveRows(parent, row, row);
    dataObjects.removeAt(row);
    endRemoveRows();
    return true;
}

void TableModel::updateRow(int row, const DataObject &obj) {
    if (row < 0 || row >= dataObjects.size())
        return;

    dataObjects[row] = obj;
    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

void TableModel::clear() {
    beginResetModel();
    dataObjects.clear();
    endResetModel();
}
