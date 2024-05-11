#pragma once

#include <QtQml/QQmlEngine>
#include <qtmetamacros.h>

struct ApplicationDesktopAction {
  Q_GADGET;
  Q_PROPERTY(QString id MEMBER id);
  Q_PROPERTY(QString name MEMBER name);
  Q_PROPERTY(QString exec MEMBER exec);

public:
  bool operator==(const ApplicationDesktopAction &other) const;

  QString id;
  QString name;
  QString exec;
};
Q_DECLARE_METATYPE(ApplicationDesktopAction);

struct ApplicationInfo {
  Q_GADGET;
  Q_PROPERTY(QString type MEMBER type);
  Q_PROPERTY(QString icon MEMBER icon);
  Q_PROPERTY(QString name MEMBER name);
  Q_PROPERTY(QMap<QString, QString> localizedNames MEMBER localizedNames);
  Q_PROPERTY(QString genericName MEMBER genericName);
  Q_PROPERTY(QMap<QString, QString> localizedGenericNames MEMBER
                 localizedGenericNames);
  Q_PROPERTY(QString comment MEMBER comment);
  Q_PROPERTY(QMap<QString, QString> localizedComments MEMBER localizedComments);
  Q_PROPERTY(QList<QString> categories MEMBER categories);
  Q_PROPERTY(QList<QString> mimeTypes MEMBER mimeTypes);
  Q_PROPERTY(QString exec MEMBER exec);
  Q_PROPERTY(bool startupNotify MEMBER startupNotify);
  Q_PROPERTY(bool terminal MEMBER terminal);
  Q_PROPERTY(QList<ApplicationDesktopAction> actions MEMBER actions);

public:
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
Q_DECLARE_METATYPE(ApplicationInfo);

class ApplicationDatabase : public QObject {
  Q_OBJECT;
  QML_NAMED_ELEMENT(ApplicationDatabase);
  QML_SINGLETON;

  Q_PROPERTY(bool mergeByName READ mergeByName WRITE setMergeByName NOTIFY
                 mergeByNameChanged);
  Q_PROPERTY(QList<ApplicationInfo> applications READ applications NOTIFY
                 applicationsChanged);

public:
  [[nodiscard]] bool mergeByName() const;
  void setMergeByName(bool mergeByName);

  [[nodiscard]] QList<ApplicationInfo> applications();

  Q_INVOKABLE [[nodiscard]] QString
  applicationNameForMimetype(const QString &mimeType);

signals:
  void mergeByNameChanged();
  void applicationsChanged();

private:
  void initialize();
  void scanDir(const QString &dirPath);
  void scanFile(const QString &filePath);

  bool mInitialized = false;
  bool mMergeByName = true;
  QMap<QString, ApplicationInfo> mApplications;
  QMap<QString, QString> mApplicationNameToFirstPath;
  QMap<QString, QString> mDefaultMimetypeHandlers;
};
