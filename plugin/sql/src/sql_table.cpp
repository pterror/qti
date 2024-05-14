#include "sql_table.hpp"

#include "sql_database.hpp"

SqlTable::SqlTable(QObject *parent)
    : QObject(parent), mDatabase(dynamic_cast<SqlDatabase *>(this->parent())) {
  if (this->mDatabase == nullptr) {
    qWarning() << "Qti.Sql.SqlTable: parent is not a Qti.Sql.SqlDatabase";
  }
  QObject::connect(this->mDatabase, &SqlDatabase::reloaded, this,
                   &SqlTable::rowsChanged);
  QObject::connect(this, &SqlTable::nameChanged, this, &SqlTable::rowsChanged);
}

QString SqlTable::name() const { return this->mName; }
void SqlTable::setName(const QString &name) {
  this->mName = name;
  emit this->nameChanged();
}

QList<QVariantMap> SqlTable::rows() const {
  return this->mDatabase->getRows(this->mName);
}
