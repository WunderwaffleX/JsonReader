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
    parsers[static_cast<int>(FileType::Json)] =
        QSharedPointer<JsonParser>::create(this);
    parsers[static_cast<int>(FileType::Xml)] =
        QSharedPointer<XmlParser>::create(this);
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

        auto parser = parsers[static_cast<int>(type)];

        if (!parser) {
            emit fileError(path, "Parser not initialized");
            processed++;
            emit progressUpdated(processed, totalFiles);
            continue;
        }

        QVector<DataObject> parsed = parser->parseFile(path);

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

    auto parser = parsers[static_cast<int>(type)];

    if (!parser) {
        emit exportFinished(false, outputPath);
        return;
    }

    bool ok = parser->exportDataObjects(data, outputPath);
    emit exportFinished(ok, outputPath);
}
