#pragma once

#include <QAbstractListModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtQml/QQmlEngine>
#include <qcontainerfwd.h>

class SqlQueryModel : public QAbstractListModel {
  Q_OBJECT;
  QML_NAMED_ELEMENT(SqlQueryModel);
  QML_UNCREATABLE("Created by other QML types only");

public:
  explicit SqlQueryModel(QObject *parent, const QString &databaseId,
                         const QString &query,
                         QVariantList parameters = QVariantList());
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;
  [[nodiscard]] int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;
  [[nodiscard]] QVariant data(const QModelIndex &parent,
                              int role = Qt::DisplayRole) const override;

private:
  [[nodiscard]] bool execIfNeeded() const;
  bool initialize();

  std::unique_ptr<QSqlQuery> mQuery;
  std::unique_ptr<QSqlRecord> mRecord;
  std::unique_ptr<int> mSize;
  QString mQueryString;
  QVariantList mParameters;
};
Q_DECLARE_METATYPE(SqlQueryModel *);
