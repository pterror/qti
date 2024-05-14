#pragma once

#include <QtQml/QQmlEngine>

class SqlDatabase;

class SqlTable : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(SqlTable);
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
  Q_PROPERTY(QVariant rows READ rows NOTIFY rowsChanged);

public:
  explicit SqlTable(QObject *parent = nullptr);

  [[nodiscard]] QString name() const;
  void setName(const QString &name);

  [[nodiscard]] QVariant rows() const;

signals:
  void nameChanged();
  void rowsChanged();

private:
  SqlDatabase *mDatabase;
  QString mName;
};
