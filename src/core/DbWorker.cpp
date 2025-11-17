#include "core/DbWorker.hpp"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

DbWorker::DbWorker(const QString &dbPath, QObject *parent)
    : QObject(parent), dbPath(dbPath) {
    connectionName = makeConnectionName();
}

DbWorker::~DbWorker() {
    QSqlDatabase::removeDatabase(connectionName);
}

QString DbWorker::makeConnectionName() const {
    return QString("conn_%1").arg(QUuid::createUuid().toString());
}

bool DbWorker::ensureDb() {
    if (QSqlDatabase::contains(connectionName))
        return true;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(dbPath);
    if (!db.open()) {
        emit error(QString("Failed to open DB: %1").arg(db.lastError().text()));
        return false;
    }

    QSqlQuery q(db);
    bool ok = q.exec(R"(
        CREATE TABLE IF NOT EXISTS editors (
          id INTEGER PRIMARY KEY AUTOINCREMENT,
          texteditor TEXT,
          fileformats TEXT,
          encoding TEXT,
          hasintellisense INTEGER,
          hasplugins INTEGER,
          cancompile INTEGER
        );
    )");
    if (!ok) {
        emit error(
            QString("Failed to create table: %1").arg(q.lastError().text()));
        return false;
    }
    return true;
}

void DbWorker::initDatabase() {
    if (!ensureDb())
        return;
    emit loaded({});
}

void DbWorker::loadAll() {
    if (!ensureDb())
        return;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery q(db);
    if (!q.exec("SELECT id, texteditor, fileformats, encoding, "
                "hasintellisense, hasplugins, cancompile FROM editors")) {
        emit error(q.lastError().text());
        return;
    }

    QVector<DataObject> rows;
    while (q.next()) {
        DataObject d;
        d.id = q.value(0).toInt();
        d.textEditor = q.value(1).toString();
        d.fileFormats = q.value(2).toString();
        d.encoding = q.value(3).toString();
        d.hasIntellisense = q.value(4).toBool();
        d.hasPlugins = q.value(5).toBool();
        d.canCompile = q.value(6).toBool();
        rows.append(d);
    }

    emit loaded(rows);
}

void DbWorker::insertData(const QVector<DataObject> &data) {
    if (!ensureDb())
        return;

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QVector<DataObject> result;
    result.reserve(data.size());

    for (auto obj : data) {

        QSqlQuery q(db);
        q.prepare(R"(
            INSERT INTO editors
            (texteditor, fileformats, encoding, hasintellisense, hasplugins, cancompile)
            VALUES (:t, :f, :e, :i, :p, :c)
        )");

        q.bindValue(":t", obj.textEditor);
        q.bindValue(":f", obj.fileFormats);
        q.bindValue(":e", obj.encoding);
        q.bindValue(":i", obj.hasIntellisense);
        q.bindValue(":p", obj.hasPlugins);
        q.bindValue(":c", obj.canCompile);

        if (!q.exec()) {
            qWarning() << "Insert error:" << q.lastError().text();
            obj.id = -1;
        } else {
            obj.id = q.lastInsertId().toInt();
        }

        result.append(obj);
    }

    emit dataInserted(result);
}

void DbWorker::editRow(const DataObject &obj) {
    if (!ensureDb())
        return;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery q(db);

    q.prepare(R"(
        UPDATE editors
        SET texteditor = :t,
            fileformats = :f,
            encoding = :e,
            hasintellisense = :i,
            hasplugins = :p,
            cancompile = :c
        WHERE id = :id
    )");

    q.bindValue(":t", obj.textEditor);
    q.bindValue(":f", obj.fileFormats);
    q.bindValue(":e", obj.encoding);
    q.bindValue(":i", obj.hasIntellisense);
    q.bindValue(":p", obj.hasPlugins);
    q.bindValue(":c", obj.canCompile);
    q.bindValue(":id", obj.id);

    if (!q.exec()) {
        emit error(q.lastError().text());
        return;
    }

    emit rowEdited(obj);
}

void DbWorker::removeRowById(int id) {
    if (!ensureDb())
        return;
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery q(db);
    q.prepare("DELETE FROM editors WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec()) {
        emit error(q.lastError().text());
        return;
    }
    emit removed(id);
}

void DbWorker::clearAll() {
    if (!ensureDb()) return;

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery q(db);

    if (!q.exec("DELETE FROM editors")) {
        emit error(q.lastError().text());
        return;
    }

    emit cleared(); 
}
