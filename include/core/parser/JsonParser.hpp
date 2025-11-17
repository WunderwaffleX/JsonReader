#pragma once
#include "core/parser/iParser.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class JsonParser : public IParser {
    Q_OBJECT
  public:
    explicit JsonParser(QObject *parent = nullptr);

    FileType type() const override {
        return FileType::Json;
    }

    QVector<DataObject> parseFile(const QString &filePath) override;
    QVector<DataObject> parseFolder(const QString &folderPath) override;
    bool exportDataObjects(const QVector<DataObject> &data,
                           const QString &outputPath) override;

  private:
    DataObject parseObject(const QJsonObject &obj);
};
