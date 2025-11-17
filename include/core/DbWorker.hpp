#pragma once
#include "model/DataObject.hpp"
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QUuid>
#include <QVector>

class DbWorker : public QObject {
    Q_OBJECT
  public:
    explicit DbWorker(const QString &dbPath, QObject *parent = nullptr);
    ~DbWorker();

  public slots:
    void initDatabase();
    void loadAll();
    void insertData(const QVector<DataObject> &data);
    void editRow(const DataObject &obj); 
    void removeRowById(int id);
    void clearAll();

  signals:
    void loaded(const QVector<DataObject> &rows);
    void dataInserted(QVector<DataObject> updatedData);
    void rowEdited(const DataObject &obj);
    void removed(int id);
    void cleared();
    void error(const QString &msg);

  private:
    QString makeConnectionName() const;
    bool ensureDb();

    QString dbPath;
    QString connectionName;
};
