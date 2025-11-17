#include "core/FileWorker.hpp"
#include "core/parser/JsonParser.hpp"
#include "core/parser/XmlParser.hpp"
#include <QDebug>
#include <QFileInfo>

FileWorker::FileWorker(const QStringList &filesOrDirs, QObject *parent)
    : QObject(parent), paths(filesOrDirs) {
    initParsers();
}

void FileWorker::initParsers() {
    parsers[FileType::Json] = QSharedPointer<JsonParser>::create(this);
    parsers[FileType::Xml] = QSharedPointer<XmlParser>::create(this);
}

FileType FileWorker::detectParser(const QString &path) const {
    QFileInfo fi(path);
    QString ext = fi.suffix().toLower();

    if (ext == "json")
        return FileType::Json;
    if (ext == "xml")
        return FileType::Xml;

    return FileType::Unknown;
}

void FileWorker::process() {
    QVector<DataObject> allData;
    const int totalFiles = paths.size();
    int processed = 0;

    for (const QString &path : paths) {
        QFileInfo fi(path);

        FileType type = detectParser(path);
        if (type == FileType::Unknown) {
            emit fileError(path, "Unsupported file type");
            processed++;
            emit progressUpdated(processed, totalFiles);
            continue;
        }

        auto parser = parsers.value(type);
        if (!parser) {
            emit fileError(path, "Parser not initialized");
            processed++;
            emit progressUpdated(processed, totalFiles);
            continue;
        }

        QVector<DataObject> parsed;
        if (fi.isDir()) {
            parsed = parser->parseFolder(path);
        } else {
            parsed = parser->parseFile(path);
        }

        if (parsed.isEmpty()) {
            emit fileError(path, "No valid entries found");
        } else {
            allData += parsed;
            emit fileParsed(path, parsed.size());
        }

        processed++;
        emit progressUpdated(processed, totalFiles);
    }

    emit finished(allData);
}

void FileWorker::exportData(const QVector<DataObject> &data,
                            const QString &outputPath) {
    if (data.isEmpty()) {
        emit exportFinished(false, outputPath);
        return;
    }

    FileType type = detectParser(outputPath);

    if (!parsers.contains(type)) {
        emit exportFinished(false, outputPath);
        return;
    }

    auto parser = parsers.value(type);
    bool ok = parser->exportDataObjects(data, outputPath);
    emit exportFinished(ok, outputPath);
}
