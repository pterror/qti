#include "application_database.hpp"

#include <QDir>
#include <QFile>
#include <QRegularExpression>

QList<ApplicationInfo> ApplicationDatabase::applications() {
  if (!this->mInitialized) {
    this->initialize();
  }
  return this->mApplications.values();
}

QString
ApplicationDatabase::applicationNameForMimetype(const QString &mimeType) {
  if (!this->mInitialized) {
    this->initialize();
  }
  return this->mDefaultMimetypeHandlers[mimeType];
}

enum class ApplicationDesktopActionProperty { Name, Exec };

const auto NAME_TO_APPLICATION_DESKTOP_ACTION_PROPERTY =
    QMap<QString, ApplicationDesktopActionProperty>({
        {"Name", ApplicationDesktopActionProperty::Name},
        {"Exec", ApplicationDesktopActionProperty::Exec},
    });

enum class ApplicationInfoProperty : int {
  Type,
  Icon,
  Name,
  LocalizedNames,
  GenericName,
  LocalizedGenericNames,
  Comment,
  LocalizedComments,
  Categories,
  MimeType,
  Exec,
  StartupNotify,
  Terminal,
};

const auto NAME_TO_APPLICATION_INFO_PROPERTY =
    QMap<QString, ApplicationInfoProperty>({
        {"Type", ApplicationInfoProperty::Type},
        {"Icon", ApplicationInfoProperty::Icon},
        {"Name", ApplicationInfoProperty::Name},
        {"GenericName", ApplicationInfoProperty::GenericName},
        {"Comment", ApplicationInfoProperty::Comment},
        {"Categories", ApplicationInfoProperty::Categories},
        {"MimeType", ApplicationInfoProperty::MimeType},
        {"Exec", ApplicationInfoProperty::Exec},
        {"StartupNotify", ApplicationInfoProperty::StartupNotify},
        {"Terminal", ApplicationInfoProperty::Terminal},
    });

const auto NON_EMPTY_REG_EXP = QRegularExpression(".");
const auto DESKTOP_ACTION_REG_EXP =
    QRegularExpression("^\\[Desktop Action (.+)\\]$");
const auto LOCALIZED_NAME_REG_EXP = QRegularExpression("^Name\\[(.+)\\]$");
const auto LOCALIZED_GENERIC_NAME_REG_EXP =
    QRegularExpression("^GenericName\\[(.+)\\]$");
const auto LOCALIZED_COMMENT_REG_EXP =
    QRegularExpression("^Comment\\[(.+)\\]$");

// TODO: watch for changes
void ApplicationDatabase::initialize() {
  if (this->mInitialized) {
    return;
  }
  auto xdgDataDirs = QString(qgetenv("XDG_DATA_DIRS"));
  if (xdgDataDirs.isEmpty()) {
    xdgDataDirs = "/usr/share/applications";
  }
  for (const auto &dirPath : xdgDataDirs.split(':')) {
    const auto dir = QDir(dirPath);
    if (!dir.exists()) {
      continue;
    }
    for (const auto &filePath : dir.entryList(QDir::Files)) {
      auto file = QFile(filePath);
      auto section = SectionType::DesktopEntry;
      auto info = ApplicationInfo();
      auto action = ApplicationDesktopAction();
      auto match = QRegularExpressionMatch();
      while (!file.atEnd()) {
        const auto line = QString(file.readLine());
        if (line.startsWith("[")) {
          if (line == "[Desktop Entry]") {
            section = SectionType::DesktopEntry;
          } else if (line.contains(DESKTOP_ACTION_REG_EXP, &match)) {
            section = SectionType::DesktopAction;
            // TODO: fix this if `name` and `exec` are not being set
            action = info.actions.emplace_back(ApplicationDesktopAction());
            action.id = match.captured(1);
          }
        } else if (section == SectionType::DesktopEntry) {
          const auto parts = line.split('=');
          const auto &key = parts[0];
          const auto value = parts.length() < 2    ? QString()
                             : parts.length() == 2 ? parts[1]
                                                   : parts.sliced(1).join('=');
          auto keyType = NAME_TO_APPLICATION_INFO_PROPERTY.constFind(key);
          if (keyType != NAME_TO_APPLICATION_INFO_PROPERTY.constEnd()) {
            switch (keyType.value()) {
            case ApplicationInfoProperty::Exec: {
              info.exec = value;
              break;
            }
            case ApplicationInfoProperty::Type: {
              info.type = value;
              break;
            }
            case ApplicationInfoProperty::Icon: {
              info.icon = value;
              break;
            }
            case ApplicationInfoProperty::Name: {
              info.name = value;
              info.localizedNames["en_US"] = value;
              break;
            }
            case ApplicationInfoProperty::GenericName: {
              info.genericName = value;
              info.localizedGenericNames["en_US"] = value;
              break;
            }
            case ApplicationInfoProperty::Comment: {
              info.comment = value;
              info.localizedComments["en_US"] = value;
              break;
            }
            case ApplicationInfoProperty::Categories: {
              // TODO: test that the filter works
              info.categories = value.split(";").filter(NON_EMPTY_REG_EXP);
              break;
            }
            case ApplicationInfoProperty::MimeType: {
              info.mimeTypes = value.split(";").filter(NON_EMPTY_REG_EXP);
              for (const auto &mimeType : info.mimeTypes) {
                this->mDefaultMimetypeHandlers[mimeType] = filePath;
              }
              break;
            }
            case ApplicationInfoProperty::StartupNotify: {
              info.startupNotify = value == "true";
              break;
            }
            case ApplicationInfoProperty::Terminal: {
              info.terminal = value == "true";
              break;
            }
            case ApplicationInfoProperty::LocalizedNames:
            case ApplicationInfoProperty::LocalizedGenericNames:
            case ApplicationInfoProperty::LocalizedComments: {
              // ignore; these do not have one single key name
              break;
            }
            }
          } else if (key.contains(LOCALIZED_NAME_REG_EXP, &match)) {
            info.localizedNames[match.captured(1)] = value;
          } else if (key.contains(LOCALIZED_GENERIC_NAME_REG_EXP, &match)) {
            info.localizedGenericNames[match.captured(1)] = value;
          } else if (key.contains(LOCALIZED_COMMENT_REG_EXP, &match)) {
            info.localizedComments[match.captured(1)] = value;
          }
        } else if (section == SectionType::DesktopAction) {
          const auto parts = line.split('=');
          const auto &key = parts[0];
          const auto value = parts.length() < 2    ? QString()
                             : parts.length() == 2 ? parts[1]
                                                   : parts.sliced(1).join('=');
          auto keyType =
              NAME_TO_APPLICATION_DESKTOP_ACTION_PROPERTY.constFind(key);
          if (keyType !=
              NAME_TO_APPLICATION_DESKTOP_ACTION_PROPERTY.constEnd()) {
            switch (keyType.value()) {
            case ApplicationDesktopActionProperty::Name: {
              action.name = value;
              break;
            }
            case ApplicationDesktopActionProperty::Exec: {
              action.exec = value;
              break;
            }
            }
          }
        }
      }
      this->mApplications[filePath] = info;
    }
    emit this->applicationsChanged();
  }
}