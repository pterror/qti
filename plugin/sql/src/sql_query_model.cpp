#include "sql_query_model.hpp"

#include <QSqlError>

SqlQueryModel::SqlQueryModel(QObject *parent, const QString &databaseId,
                             const QString &query, QVariantList parameters)
    : QAbstractListModel(parent),
      mQuery(std::make_unique<QSqlQuery>(
          QSqlQuery(query, QSqlDatabase::database(databaseId)))),
      mRecord(std::make_unique<QSqlRecord>(this->mQuery->record())),
      mSize(std::make_unique<int>(-1)), mQueryString(query),
      mParameters(std::move(parameters)) {
}

bool SqlQueryModel::execIfNeeded() const {
  if (this->mQuery->isActive()) {
    return true;
  }
  this->mQuery->prepare(this->mQueryString);
  const auto parameterCount = this->mParameters.size();
  for (auto i = 0; i < parameterCount; i += 1) {
    this->mQuery->bindValue(i, this->mParameters[i]);
  }
  this->mQuery->exec();
  // if still inactive then the query is broken
  if (!this->mQuery->isActive()) {
    qWarning() << "Qti.Sql: SQL query failed:" << this->mQuery->lastError();
    qWarning() << "Qti.Sql: Query was" << this->mQuery->lastQuery();
    return false;
  }
  *this->mRecord = this->mQuery->record();
  return true;
}

QVariant SqlQueryModel::headerData(int section, Qt::Orientation /*orientation*/,
                                   int /*role*/) const {
  return section;
}

int SqlQueryModel::rowCount(const QModelIndex & /*parent*/) const {
  if (*this->mSize != -1) {
    return *this->mSize;
  }
  if (!this->execIfNeeded()) {
    return 0;
  }
  auto size = this->mQuery->size();
  if (size == -1) {
    this->mQuery->seek(0);
    if (this->mQuery->at() == QSql::AfterLastRow) {
      *this->mSize = 0;
      return 0;
    }
    auto current = 1 << 16;
    auto low = 0;
    auto high = 0;
    this->mQuery->seek(current);
    auto position = this->mQuery->at();
    if (position == QSql::AfterLastRow) {
      while (position == QSql::AfterLastRow) {
        high = current;
        current = high / 2;
        this->mQuery->seek(current);
        position = this->mQuery->at();
      }
    } else {
      while (position != QSql::AfterLastRow) {
        high = current;
        current = high * 2;
        this->mQuery->seek(current);
        position = this->mQuery->at();
      }
      low = high / 2;
    }
    while (low != high) {
      current = (low + high + 1) / 2;
      this->mQuery->seek(current);
      auto position = this->mQuery->at();
      if (position == QSql::AfterLastRow) {
        high = current - 1;
      } else {
        low = current;
      }
    }
    size = low;
  }
  *this->mSize = size;
  return size;
}

QVariant SqlQueryModel::data(const QModelIndex &parent, int role) const {
  if (role != Qt::DisplayRole || !this->execIfNeeded()) {
    return QVariant();
  }
  this->mQuery->seek(parent.row());
  const auto fieldCount = this->mRecord->count();
  // cannot be const otherwise
  auto map = QVariantMap();
  for (auto i = 0; i < fieldCount; i += 1) {
    map[this->mRecord->fieldName(i)] = this->mQuery->value(i);
  }
  return map;
}
