#pragma once

#include <QHash>
#include <QString>
#include <QVector>
#include <Qt>

enum class FileType { Json, Xml, Unknown };

struct FileFormat {
    QString name;
    QString displayName;
    QString dialogFilter;
    QString mask;

    explicit FileFormat(const QString &lowerName)
        : name(lowerName), displayName(lowerName.toUpper()),
          dialogFilter(QString("%1 files (*.%2)").arg(displayName, lowerName)),
          mask("*." + lowerName) {}
};

namespace FileConst {
inline const FileFormat Json("json");
inline const FileFormat Xml("xml");

inline const QVector<FileFormat> AllFormats = {Json, Xml};

inline const QString DialogFilters = []() {
    QStringList list;
    for (const auto &f : AllFormats)
        list << f.dialogFilter;
    return list.join(";;");
}();
} // namespace FileConst

inline const QHash<FileType, FileFormat> FILE_TYPE_MAP = {
    {FileType::Json, FileConst::Json},
    {FileType::Xml, FileConst::Xml},
};

inline FileType detectParser(const QString &path) {
    QString lower = path.toLower();

    for (auto it = FILE_TYPE_MAP.begin(); it != FILE_TYPE_MAP.end(); ++it) {
        if (lower.endsWith(it.value().name)) {
            return it.key();
        }
    }
    return FileType::Unknown;
}
