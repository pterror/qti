#include "sql_query_model.hpp"
#include <qnamespace.h>
#include <qvariant.h>

SqlQueryModel::SqlQueryModel(QObject *parent, QSqlQuery &&query)
    : QAbstractListModel(parent),
      mQuery(std::make_unique<QSqlQuery>(std::move(query))),
      mSize(std::make_unique<int>(-1)) {
  this->mRecord = this->mQuery->record();
}

QVariant SqlQueryModel::headerData(int section, Qt::Orientation /*orientation*/,
                                   int /*role*/) const {
  return section;
}

int SqlQueryModel::rowCount(const QModelIndex & /*parent*/) const {
  if (*this->mSize != -1) {
    return *this->mSize;
  }
  if (!this->mQuery->isActive()) {
    this->mQuery->exec();
  }
  auto size = this->mQuery->size();
  if (size == -1) {
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
        low = current;
        current = low * 2;
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
  if (role != Qt::DisplayRole) {
    return QVariant();
  }
  if (!this->mQuery->isActive()) {
    this->mQuery->exec();
  }
  // if still inactive then the query is broken
  if (!this->mQuery->isActive()) {
    return QVariant();
  }
  this->mQuery->seek(parent.row());
  const auto fieldCount = this->mRecord.count();
  // cannot be const otherwise
  auto map = QVariantMap();
  for (auto i = 0; i < fieldCount; i += 1) {
    map[this->mRecord.fieldName(i)] = this->mQuery->value(i);
  }
  return map;
}
