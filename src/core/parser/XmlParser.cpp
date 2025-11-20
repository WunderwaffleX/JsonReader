#include "core/parser/XmlParser.hpp"
#include <QDebug>
#include <QDir>
#include <QFile>

XmlParser::XmlParser(QObject *parent) : IParser(parent) {}

QVector<DataObject> XmlParser::parseFile(const QString &filePath) {
    QVector<DataObject> result;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit fileError(filePath, "Failed to open file");
        return result;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == QStringLiteral("root")) {
            result.append(parseObject(xml));
        }
    }

    if (xml.hasError()) {
        emit fileError(filePath, xml.errorString());
    } else {
        emit fileParsed(QFileInfo(filePath).fileName(), result.size());
    }

    return result;
}

DataObject XmlParser::parseObject(QXmlStreamReader &xml) {
    DataObject d;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            QString tag = xml.name().toString();

            auto val = xml.readElementText();

            if (tag == "texteditor")
                d.textEditor = val;
            else if (tag == "fileformats")
                d.fileFormats = val;
            else if (tag == "encoding")
                d.encoding = val;
            else if (tag == "hasintellisense")
                d.hasIntellisense =
                    (val.compare("true", Qt::CaseInsensitive) == 0);
            else if (tag == "hasplugins")
                d.hasPlugins = (val.compare("true", Qt::CaseInsensitive) == 0);
            else if (tag == "cancompile")
                d.canCompile = (val.compare("true", Qt::CaseInsensitive) == 0);
        }

        if (xml.isEndElement() && xml.name() == QStringLiteral("root"))
            break;
    }

    return d;
}

bool XmlParser::exportDataObjects(const QVector<DataObject> &data,
                                  const QString &outputPath) {
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();

    for (const DataObject &d : data) {
        xml.writeStartElement("root");

        xml.writeTextElement("texteditor", d.textEditor);
        xml.writeTextElement("fileformats", d.fileFormats);
        xml.writeTextElement("encoding", d.encoding);
        xml.writeTextElement("hasintellisense",
                             d.hasIntellisense ? "true" : "false");
        xml.writeTextElement("hasplugins", d.hasPlugins ? "true" : "false");
        xml.writeTextElement("cancompile", d.canCompile ? "true" : "false");

        xml.writeEndElement();
    }

    xml.writeEndDocument();
    return true;
}
