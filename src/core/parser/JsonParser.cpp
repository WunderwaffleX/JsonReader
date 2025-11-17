#include "core/parser/JsonParser.hpp"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonParseError>

JsonParser::JsonParser(QObject *parent) : IParser(parent) {}

QVector<DataObject> JsonParser::parseFolder(const QString &folderPath) {
    QVector<DataObject> allData;

    QDir dir(folderPath);
    QFileInfoList files = dir.entryInfoList({"*.json"}, QDir::Files);

    for (const QFileInfo &fileInfo : files)
        allData += parseFile(fileInfo.absoluteFilePath());

    return allData;
}

QVector<DataObject> JsonParser::parseFile(const QString &filePath) {
    QVector<DataObject> result;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit fileError(filePath, "Failed to open file");
        return result;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);

    if (err.error != QJsonParseError::NoError) {
        emit fileError(filePath, err.errorString());
        return result;
    }

    if (!doc.isObject()) {
        emit fileError(filePath, "Invalid JSON format — expected object");
        return result;
    }

    QJsonObject rootObj = doc.object().value("root").toObject();
    if (rootObj.isEmpty()) {
        emit fileError(filePath, "Missing root object");
        return result;
    }

    result.append(parseObject(rootObj));

    emit fileParsed(QFileInfo(filePath).fileName(), result.size());
    return result;
}

DataObject JsonParser::parseObject(const QJsonObject &obj) {
    DataObject d;

    d.textEditor = obj.value("texteditor").toString();
    d.fileFormats = obj.value("fileformats").toString();
    d.encoding = obj.value("encoding").toString();
    d.hasIntellisense = obj.value("hasintellisense").toBool(false);
    d.hasPlugins = obj.value("hasplugins").toBool(false);
    d.canCompile = obj.value("cancompile").toBool(false);

    return d;
}

bool JsonParser::exportDataObjects(const QVector<DataObject> &data,
                                   const QString &outputPath) {
    if (data.isEmpty())
        return false;

    const DataObject &d = data[0];

    QJsonObject obj;
    obj["texteditor"] = d.textEditor;
    obj["fileformats"] = d.fileFormats;
    obj["encoding"] = d.encoding;
    obj["hasintellisense"] = d.hasIntellisense;
    obj["hasplugins"] = d.hasPlugins;
    obj["cancompile"] = d.canCompile;

    QJsonObject root;
    root["root"] = obj;

    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}
