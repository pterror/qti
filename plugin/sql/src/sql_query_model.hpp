#pragma once

#include <QAbstractListModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtQml/QQmlEngine>

class SqlQueryModel : public QAbstractListModel {
  Q_OBJECT;
  QML_NAMED_ELEMENT(SqlQueryModel);
  QML_UNCREATABLE("Created by other QML types only");

public:
  explicit SqlQueryModel(QObject *parent, QSqlQuery &&query);
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const override;
  [[nodiscard]] int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;
  [[nodiscard]] QVariant data(const QModelIndex &parent,
                              int role = Qt::DisplayRole) const override;

private:
  bool initialize();

  std::unique_ptr<QSqlQuery> mQuery;
  QSqlRecord mRecord;
  std::unique_ptr<int> mSize;
};
Q_DECLARE_METATYPE(SqlQueryModel *);
