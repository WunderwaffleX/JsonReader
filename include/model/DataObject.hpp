#pragma once
#include <QString>
#include <QMetaType>

struct DataObject {
    int id = -1;
    QString textEditor;
    QString fileFormats;
    QString encoding;
    bool hasIntellisense = false;
    bool hasPlugins = false;
    bool canCompile = false;
};
