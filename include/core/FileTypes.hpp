#pragma once

#include <QHash>
#include <QString>
#include <QStringList>
#include <QVector>

enum class FileType { Json, Xml, Unknown, Count };

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

inline const QVector<FileFormat> &AllFormats() {
    static const QVector<FileFormat> formats = {FileFormat("json"),
                                                FileFormat("xml")};
    return formats;
}

inline const QString &DialogFilters() {
    static const QString filters = [] {
        QStringList list;
        for (const auto &f : AllFormats())
            list << f.dialogFilter;
        return list.join(";;");
    }();
    return filters;
}

inline const QStringList &AllMasks() {
    static const QStringList masks = [] {
        QStringList list;
        for (const auto &format : AllFormats())
            list << format.mask;
        return list;
    }();
    return masks;
}

inline const QHash<FileType, FileFormat> &TypeMap() {
    static const QHash<FileType, FileFormat> map = {
        {FileType::Json, FileFormat("json")},
        {FileType::Xml, FileFormat("xml")}};
    return map;
}

} // namespace FileConst

inline FileType detectParser(const QString &path) {
    const QString lower = path.toLower();

    for (auto it = FileConst::TypeMap().cbegin();
         it != FileConst::TypeMap().cend(); ++it) {
        if (lower.endsWith('.' + it.value().name)) {
            return it.key();
        }
    }

    return FileType::Unknown;
}
