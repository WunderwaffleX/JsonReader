#pragma once
#include "core/parser/iParser.hpp"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class XmlParser : public IParser {
    Q_OBJECT
  public:
    explicit XmlParser(QObject *parent = nullptr);

    FileType type() const override {
        return FileType::Xml;
    }

    QVector<DataObject> parseFile(const QString &filePath) override;
    bool exportDataObjects(const QVector<DataObject> &data,
                           const QString &outputPath) override;

  private:
    DataObject parseObject(QXmlStreamReader &xml);
};
