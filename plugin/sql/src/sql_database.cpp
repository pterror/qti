#include "sql_database.hpp"

#include "sql_query_model.hpp"
#include "sql_table.hpp"

#include <qcontainerfwd.h>
#include <qvariant.h>
#include <utility>

#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>

SqlDatabase::SqlDatabase()
    : mDatabaseId(QUuid::createUuid()), mType("QSQLITE") {
  QObject::connect(this, &SqlDatabase::reloaded, this,
                   &SqlDatabase::tableNamesChanged);
  QObject::connect(this, &SqlDatabase::reloaded, this,
                   &SqlDatabase::tablesChanged);
  QObject::connect(this, &SqlDatabase::reloaded, this,
                   &SqlDatabase::reactiveChanged);
}

SqlDatabase *SqlDatabase::reactive() { return this; }

QString SqlDatabase::type() const { return this->mType; }
void SqlDatabase::setType(QString type) {
  this->mType = std::move(type);
  this->reload();
}

QString SqlDatabase::name() const { return this->mName; }
void SqlDatabase::setName(QString name) {
  this->mName = std::move(name);
  this->reload();
}

QString SqlDatabase::username() const { return this->mUsername; }
void SqlDatabase::setUsername(QString username) {
  this->mUsername = std::move(username);
  this->reload();
}

QString SqlDatabase::password() const { return this->mPassword; }
void SqlDatabase::setPassword(QString password) {
  this->mPassword = std::move(password);
  this->reload();
}

QString SqlDatabase::hostname() const { return this->mHostname; }
void SqlDatabase::setHostname(QString hostname) {
  this->mHostname = std::move(hostname);
  this->reload();
}

int SqlDatabase::port() const { return this->mPort; }
void SqlDatabase::setPort(int port) {
  this->mPort = port;
  this->reload();
}

QString SqlDatabase::connectionOptions() const {
  return this->mConnectionOptions;
}
void SqlDatabase::setConnectionOptions(QString connectionOptions) {
  this->mConnectionOptions = std::move(connectionOptions);
  this->reload();
}

QStringList SqlDatabase::tableNames() const {
  if (this->mType == "QSQLITE") {
    const auto id = this->mDatabaseId.toString();
    auto database = QSqlDatabase::database(id);
    if (!database.isOpen()) {
      return QStringList();
    }
    static const auto *queryString =
        "SELECT name FROM sqlite_master WHERE type='table'";
    auto query = QSqlQuery(queryString, database);
    query.exec();
    auto rows = QStringList();
    while (query.next()) {
      rows.emplace_back(query.value(0).toString());
    }
    return rows;
  } else {
    qWarning() << "Qti.Sql does not know how to list tables for" << this->mType
               << "databases.";
    return QStringList();
  }
}

QVariantMap SqlDatabase::tables() {
  const auto tableNames = this->tableNames();
  // cannot be `const` otherwise it stays empty
  auto tables = QVariantMap();
  for (const auto &tableName : tableNames) {
    auto *table = new SqlTable(this);
    table->setName(tableName);
    tables[tableName] = QVariant::fromValue(table);
  }
  return tables;
}

void SqlDatabase::reload() {
  auto id = this->mDatabaseId.toString();
  if (this->mInitialized) {
    QSqlDatabase::removeDatabase(id);
  }
  QSqlDatabase::addDatabase(this->mType, id);
  auto database = QSqlDatabase::database(id);
  database.setDatabaseName(this->mName);
  database.setUserName(this->mUsername);
  database.setPassword(this->mPassword);
  database.setHostName(this->mHostname);
  database.setPort(this->mPort);
  database.setConnectOptions(this->mConnectionOptions);
  database.open();
  this->mInitialized = true;
  emit this->reloaded();
}

// TODO: does this work?
void SqlDatabase::transaction(const QJSValue &function) const {
  const auto id = this->mDatabaseId.toString();
  auto database = QSqlDatabase::database(id);
  database.transaction();
  function.call();
  database.commit();
}

QVariant SqlDatabase::getRows(const QString &tableName) {
  const auto id = this->mDatabaseId.toString();
  const auto database = QSqlDatabase::database(id);
  if (!database.isOpen()) {
    return QVariant::fromValue(new SqlQueryModel(this, id, ""));
  }
  const auto queryString = "SELECT * FROM `" + tableName + "`";
  return QVariant::fromValue(new SqlQueryModel(this, id, queryString));
}

QVariant SqlDatabase::query(const QString &query,
                            const QVariantList &parameters) {
  const auto id = this->mDatabaseId.toString();
  const auto database = QSqlDatabase::database(id);
  if (!database.isOpen()) {
    return QVariant::fromValue(new SqlQueryModel(this, id, ""));
  }
  return QVariant::fromValue(new SqlQueryModel(this, id, query, parameters));
}

// could be implemented using `insertMany`, but kept separate as a
// premature optimization
void SqlDatabase::insert(const QString &tableName, const QVariant &row) const {
  const auto id = this->mDatabaseId.toString();
  const auto database = QSqlDatabase::database(id);
  const auto rowAsMap = row.toMap();
  const auto keysCount = rowAsMap.size();
  const auto keysString =
      keysCount == 0 ? "" : "`" + rowAsMap.keys().join("`, `") + "`";
  const auto placeholdersList = QList<QString>();
  const auto placeholdersString = QList<QString>(keysCount).fill("?").join(",");
  const auto queryString = "INSERT INTO `" + tableName + "` (" + keysString +
                           ") VALUES (" + placeholdersString + ")";
  auto query = QSqlQuery(queryString, database);
  const auto values = rowAsMap.values();
  for (auto i = 0; i < keysCount; i += 1) {
    query.bindValue(i, values[i]);
  }
  query.exec();
}

void SqlDatabase::insertMany(const QString &tableName,
                             QList<QVariant> rows) const {
  if (rows.empty()) {
    return;
  }
  const auto id = this->mDatabaseId.toString();
  auto database = QSqlDatabase::database(id);
  const auto rowAsMap = rows[0].toMap();
  const auto keys = rowAsMap.keys();
  const auto keysCount = rowAsMap.size();
  const auto keysString =
      keysCount == 0 ? "" : "`" + rowAsMap.keys().join("`, `") + "`";
  const auto placeholdersList = QList<QString>();
  const auto placeholdersString = QList<QString>(keysCount).fill("?").join(",");
  const auto queryString = "INSERT INTO `" + tableName + "` (" + keysString +
                           ") VALUES (" + placeholdersString + ")";
  auto query = QSqlQuery(queryString, database);
  database.transaction();
  for (const auto &row : rows) {
    const auto map = row.toMap();
    for (auto i = 0; i < keysCount; i += 1) {
      query.bindValue(i, map[keys[i]]);
    }
    query.exec();
  }
  database.commit();
}

void SqlDatabase::update(const QString &tableName,          // NOLINT
                         const QString &expression) const { // NOLINT
  // TODO:
}

void SqlDatabase::delete_(const QString &tableName,          // NOLINT
                          const QString &expression) const { // NOLINT
  // TODO:
}

void SqlDatabase::createTable(const QString &tableName,        // NOLINT
                              QList<QVariant> columns) const { // NOLINT
  // TODO:
}
