#pragma once

#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>

#include "core/FileTypes.hpp"
#include "core/parser/iParser.hpp"
#include "model/DataObject.hpp"

class FileWorker : public QObject {
    Q_OBJECT

  public:
    explicit FileWorker(const QStringList &filesOrDirs,
                        QObject *parent = nullptr);

  public slots:
    void process();
    void exportData(const QVector<DataObject> &data, const QString &outputPath);

  signals:
    void fileParsed(const QString &fileName, int count);
    void fileError(const QString &fileName, const QString &error);
    void finished(const QVector<DataObject> &allData);
    void exportFinished(bool success, const QString &path);
    void progressUpdated(int processed, int total);

  private:
    QStringList paths;
    QMap<FileType, QSharedPointer<IParser>> parsers;

  private:
    void initParsers();
    FileType detectParser(const QString &path) const;
};
