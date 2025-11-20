#pragma once
#include <QMetaType>
#include <QString>

struct DataObject {
    int id = -1;
    QString textEditor;
    QString fileFormats;
    QString encoding;
    bool hasIntellisense = false;
    bool hasPlugins = false;
    bool canCompile = false;
};
