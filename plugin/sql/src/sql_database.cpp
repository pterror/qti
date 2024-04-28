#include "sql_database.hpp"
#include <ranges>

SqlDatabase::SqlDatabase()
    : mDatabaseId(QUuid::createUuid()), mType("QSQLITE") {}

QString SqlDatabase::type() const { return this->mType; }
void SqlDatabase::setType(QString value) {
  this->mType = value;
  this->reload();
}

QString SqlDatabase::name() const { return this->mName; }
void SqlDatabase::setName(QString value) {
  this->mName = value;
  this->reload();
}

QString SqlDatabase::username() const { return this->mUsername; }
void SqlDatabase::setUsername(QString value) {
  this->mUsername = value;
  this->reload();
}

QString SqlDatabase::password() const { return this->mPassword; }
void SqlDatabase::setPassword(QString value) {
  this->mPassword = value;
  this->reload();
}

QString SqlDatabase::hostname() const { return this->mHostname; }
void SqlDatabase::setHostname(QString value) {
  this->mHostname = value;
  this->reload();
}

int SqlDatabase::port() const { return this->mPort; }
void SqlDatabase::setPort(int value) {
  this->mPort = value;
  this->reload();
}

QString SqlDatabase::connectionOptions() const {
  return this->mConnectionOptions;
}
void SqlDatabase::setConnectionOptions(QString value) {
  this->mConnectionOptions = value;
  this->reload();
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
}

// Could be implemented using `insertMany`, but kept separate as a
// micro-optimization.
void SqlDatabase::insert(QString tableName, QVariant row) {
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
  for (const auto i : std::views::iota(0, keysCount)) {
    query.bindValue(i, values[i]);
  }
  query.exec();
}

void SqlDatabase::insertMany(QString tableName, QList<QVariant> rows) {
  if (rows.size() == 0) {
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
    for (const auto i : std::views::iota(0, keysCount)) {
      query.bindValue(i, map[keys[i]]);
    }
    query.exec();
  }
  database.commit();
}
