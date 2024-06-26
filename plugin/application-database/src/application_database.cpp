#include "application_database.hpp"

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <qiodevicebase.h>

enum class SectionType { DesktopEntry, DesktopAction };

bool ApplicationDesktopAction::operator==(
    const ApplicationDesktopAction &other) const {
  return this->id == other.id && this->name == other.name &&
         this->exec == other.exec;
}

bool ApplicationDatabase::mergeByName() const { return this->mMergeByName; }

void ApplicationDatabase::setMergeByName(bool mergeByName) {
  this->mMergeByName = mergeByName;
  emit this->mergeByNameChanged();
  this->mInitialized = false;
  this->initialize();
}

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
  this->mInitialized = true;
  this->mApplications.clear();
  this->mApplicationNameToFirstPath.clear();
  this->mDefaultMimetypeHandlers.clear();
  auto xdgDataDirs = QString(qgetenv("XDG_DATA_DIRS"));
  if (xdgDataDirs.isEmpty()) {
    xdgDataDirs = "/usr/share";
  }
  for (const auto &dirPath : xdgDataDirs.split(':')) {
    this->scanDir(dirPath + "/applications");
  }
  // do not `emit this->applicationsChanged()` here because this may be
  // triggered by accessing `applications`
}

void ApplicationDatabase::scanDir(const QString &dirPath) {
  const auto dir = QDir(dirPath);
  if (!dir.exists()) {
    return;
  }
  for (const auto &childDirPath :
       dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
    this->scanDir(dirPath + "/" + childDirPath);
  }
  for (const auto &filePath : dir.entryList(QDir::Files)) {
    this->scanFile(dirPath + "/" + filePath);
  }
}

void ApplicationDatabase::scanFile(const QString &filePath) {
  auto file = QFile(filePath);
  if (!file.exists()) {
    return;
  }
  auto section = SectionType::DesktopEntry;
  auto info = ApplicationInfo();
  auto *action = static_cast<ApplicationDesktopAction *>(nullptr);
  auto match = QRegularExpressionMatch();
  file.open(QIODeviceBase::ReadOnly);
  while (!file.atEnd()) {
    const auto line = QString(file.readLine()).replace('\n', "");
    if (line.startsWith("[")) {
      if (line == "[Desktop Entry]") {
        section = SectionType::DesktopEntry;
      } else if (line.contains(DESKTOP_ACTION_REG_EXP, &match)) {
        section = SectionType::DesktopAction;
        action = &info.actions.emplace_back(ApplicationDesktopAction());
        action->id = match.captured(1);
      }
    } else if (section == SectionType::DesktopEntry) {
      const auto parts = line.split('=');
      const auto &key = parts[0];
      const auto value = parts.length() < 2    ? QString()
                         : parts.length() == 2 ? parts[1]
                                               : parts.sliced(1).join('=');
      auto keyTypeIt = NAME_TO_APPLICATION_INFO_PROPERTY.constFind(key);
      if (keyTypeIt != NAME_TO_APPLICATION_INFO_PROPERTY.constEnd()) {
        switch (keyTypeIt.value()) {
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
          info.categories = value.split(";").filter(NON_EMPTY_REG_EXP);
          break;
        }
        case ApplicationInfoProperty::MimeType: {
          info.mimeTypes = value.split(";").filter(NON_EMPTY_REG_EXP);
          for (const auto &mimeType : info.mimeTypes) {
            if (this->mDefaultMimetypeHandlers.contains(mimeType)) {
              continue;
            }
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
      auto keyTypeIt =
          NAME_TO_APPLICATION_DESKTOP_ACTION_PROPERTY.constFind(key);
      if (keyTypeIt != NAME_TO_APPLICATION_DESKTOP_ACTION_PROPERTY.constEnd()) {
        switch (keyTypeIt.value()) {
        case ApplicationDesktopActionProperty::Name: {
          action->name = value;
          break;
        }
        case ApplicationDesktopActionProperty::Exec: {
          action->exec = value;
          break;
        }
        }
      }
    }
  }
  if (this->mMergeByName) {
    const auto pathIt = this->mApplicationNameToFirstPath.constFind(info.name);
    if (pathIt == this->mApplicationNameToFirstPath.constEnd()) {
      this->mApplicationNameToFirstPath[info.name] = filePath;
    } else {
      const auto &path = pathIt.value();
      auto existingInfo = this->mApplications[path];
      // assume localizations and categories never need to be merged
      // assume mimetypes and actions never overlap
      existingInfo.mimeTypes.append(info.mimeTypes);
      existingInfo.actions.append(info.actions);
      this->mApplications[path] = existingInfo;
      return;
    }
  }
  this->mApplications[filePath] = info;
}
