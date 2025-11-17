#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include "core/FileTypes.hpp"
#include "model/DataObject.hpp"

class IParser : public QObject {
    Q_OBJECT
public:
    explicit IParser(QObject *parent = nullptr) : QObject(parent) {}

    virtual FileType type() const = 0;

    virtual QVector<DataObject> parseFile(const QString &filePath) = 0;
    virtual QVector<DataObject> parseFolder(const QString &folderPath) = 0;
    virtual bool exportDataObjects(const QVector<DataObject> &data, const QString &outputPath) = 0;

signals:
    void fileParsed(const QString &fileName, int fileCount);
    void fileError(const QString &fileName, const QString &error);
};

