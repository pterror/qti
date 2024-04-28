#pragma once

#include <QtQml/QQmlEngine>

enum class SectionType { DesktopEntry, DesktopAction };

struct ApplicationDesktopAction {
  QString id;
  QString name;
  QString exec;
};

struct ApplicationInfo {
  QString type;
  QString icon;
  QString name;
  QMap<QString, QString> localizedNames;
  QString genericName;
  QMap<QString, QString> localizedGenericNames;
  QString comment;
  QMap<QString, QString> localizedComments;
  QList<QString> categories;
  QList<QString> mimeTypes;
  QString exec;
  bool startupNotify;
  bool terminal;
  QList<ApplicationDesktopAction> actions;
};

class ApplicationDatabase : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(ApplicationDatabase);
  QML_SINGLETON;

  Q_PROPERTY(QString applications READ applications NOTIFY applicationsChanged);

public:
  ApplicationDatabase();

  [[nodiscard]] QList<ApplicationInfo> applications();

  Q_INVOKABLE [[nodiscard]] QString
  applicationNameForMimetype(const QString &mimeType);

signals:
  void applicationsChanged();

private:
  void initialize();

  bool mInitialized;
  QMap<QString, ApplicationInfo> mApplications;
  QMap<QString, QString> mDefaultMimetypeHandlers;
};
