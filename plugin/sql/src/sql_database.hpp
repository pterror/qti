#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QtQml/QQmlEngine>
#include <qcontainerfwd.h>

class SqlDatabase : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(SqlDatabase);
  Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged);
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
  Q_PROPERTY(
      QString username READ username WRITE setUsername NOTIFY usernameChanged);
  Q_PROPERTY(
      QString password READ password WRITE setPassword NOTIFY passwordChanged);
  Q_PROPERTY(
      QString hostname READ hostname WRITE setHostname NOTIFY hostnameChanged);
  Q_PROPERTY(qint32 port READ port WRITE setPort NOTIFY portChanged);
  Q_PROPERTY(QString connectionOptions READ connectionOptions WRITE
                 setConnectionOptions NOTIFY connectionOptionsChanged);
  Q_PROPERTY(QStringList tableNames READ tableNames NOTIFY tableNamesChanged);
  Q_PROPERTY(QVariantMap tables READ tables NOTIFY tablesChanged);

public:
  explicit SqlDatabase();

  [[nodiscard]] QString type() const;
  void setType(QString type);

  [[nodiscard]] QString name() const;
  void setName(QString name);

  [[nodiscard]] QString username() const;
  void setUsername(QString username);

  [[nodiscard]] QString password() const;
  void setPassword(QString password);

  [[nodiscard]] QString hostname() const;
  void setHostname(QString hostname);

  [[nodiscard]] qint32 port() const;
  void setPort(qint32 port);

  [[nodiscard]] QString connectionOptions() const;
  void setConnectionOptions(QString connectionOptions);

  [[nodiscard]] QStringList tableNames() const;

  [[nodiscard]] QVariantMap tables();

  Q_INVOKABLE void transaction(const QJSValue &function) const;
  Q_INVOKABLE [[nodiscard]] QList<QVariantMap>
  getRows(const QString &tableName) const;
  Q_INVOKABLE [[nodiscard]] QList<QVariantMap>
  select(const QString &tableName, const QString &expression) const;
  Q_INVOKABLE void insert(const QString &tableName, const QVariant &row) const;
  Q_INVOKABLE void insertMany(const QString &tableName,
                              QList<QVariant> rows) const;
  Q_INVOKABLE void update(const QString &tableName,
                          const QString &expression) const;
  Q_INVOKABLE void delete_(const QString &tableName, // NOLINT
                           const QString &expression) const;
  Q_INVOKABLE void createTable(const QString &tableName,
                               QList<QVariant> columns) const;

signals:
  void typeChanged();
  void nameChanged();
  void usernameChanged();
  void passwordChanged();
  void hostnameChanged();
  void portChanged();
  void connectionOptionsChanged();
  void tableNamesChanged();
  void tablesChanged();
  void reloaded();

private:
  void reload();

  bool mInitialized = false;
  QUuid mDatabaseId;
  QString mType;
  QString mName;
  QString mUsername;
  QString mPassword;
  QString mHostname;
  qint32 mPort = 0;
  QString mConnectionOptions;
};
