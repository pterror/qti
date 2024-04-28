#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QtQml/QQmlEngine>

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

public:
  SqlDatabase();

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

  // TODO: consider whether `transaction()` is needed.
  void insert(QString tableName, QVariant row);
  void insertMany(QString tableName, QList<QVariant> rows);
  // TODO: fill in arguments
  // TODO: how to conditions
  void update(QString tableName);
  void delete_(QString tableName);
  void createTable(QString tableName, QList<QVariant> columns);

signals:
  void typeChanged();
  void nameChanged();
  void usernameChanged();
  void passwordChanged();
  void hostnameChanged();
  void portChanged();
  void connectionOptionsChanged();
  void pathChanged();

private:
  void reload();

  bool mInitialized;
  QUuid mDatabaseId;
  QString mType;
  QString mName;
  QString mUsername;
  QString mPassword;
  QString mHostname;
  qint32 mPort;
  QString mConnectionOptions;
};
