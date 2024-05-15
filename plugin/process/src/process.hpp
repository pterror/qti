#pragma once

#include <QProcess>
#include <QtQml/QQmlEngine>

class Process : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(Process);
  Q_PROPERTY(
      QString program READ program WRITE setProgram NOTIFY programChanged);
  Q_PROPERTY(QStringList arguments READ arguments WRITE setArguments NOTIFY
                 argumentsChanged);

public:
  [[nodiscard]] QString program() const;
  void setProgram(const QString &program);

  [[nodiscard]] QStringList arguments() const;
  void setArguments(const QStringList &arguments);

  Q_INVOKABLE void startDetached();

signals:
  void programChanged();
  void argumentsChanged();

private:
  QProcess mProcess;
};
