#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include <QGuiApplication>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlNetworkAccessManagerFactory>
#include <QQuickWindow>
#include <QRegularExpression>
#include <cstdlib>
#include <limits>

const auto FILE_DIRECTORY_AND_NAME_REG_EXP =
    QRegularExpression("(.*)/([^/]+?)(?:[.]qml)?$");

class DiskNetworkAccessManagerFactory : public QQmlNetworkAccessManagerFactory {
public:
  QNetworkAccessManager *create(QObject *parent) override;
};

QNetworkAccessManager *
DiskNetworkAccessManagerFactory::create(QObject *parent) {
  auto *networkAccessManager = new QNetworkAccessManager(parent);
  auto *cache = new QNetworkDiskCache(parent);
  cache->setMaximumCacheSize(std::numeric_limits<qint64>::max());
  cache->setCacheDirectory(QString(std::getenv("HOME")) + // NOLINT
                           "/.cache/qti/web/");
  networkAccessManager->setCache(cache);
  return networkAccessManager;
}

int main(int argc, char **argv) {
  // ShareOpenGLContexts is needed for using the threaded renderer
  // on Nvidia EGLStreams
  QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
  const auto app = QGuiApplication(argc, argv);

  auto cliParser = QCommandLineParser();
  const auto pathOption =
      QCommandLineOption("path", "Path to the root file in a QML app.", "path");
  const auto daemonOption = QCommandLineOption(
      "daemon", "When true, do not quit even if there are no windows open.");
  cliParser.addHelpOption();
  cliParser.addVersionOption();
  cliParser.addOption(pathOption);
  cliParser.addOption(daemonOption);
  cliParser.process(app);
  cliParser.unknownOptionNames();

  const auto path = cliParser.value(pathOption);
  auto appDirectory = QString();
  auto appName = QString("qti");
  auto match = QRegularExpressionMatch();
  if (path.contains(FILE_DIRECTORY_AND_NAME_REG_EXP, &match)) {
    appDirectory = match.captured(1);
    appName = match.captured(2);
  }
  QGuiApplication::setDesktopFileName(appName);
  QGuiApplication::setApplicationName(appName);
  QGuiApplication::setApplicationDisplayName(appName);

  // TODO: configurable default theme name
  QIcon::setThemeName("qti-dark");
  auto themeSearchPaths = QIcon::themeSearchPaths();
  themeSearchPaths.emplace_back(appDirectory + "/icons");
  QIcon::setThemeSearchPaths(themeSearchPaths);

  auto engine = QQmlApplicationEngine();
  auto *networkAccessManagerFactory = new DiskNetworkAccessManagerFactory();
  engine.setNetworkAccessManagerFactory(networkAccessManagerFactory);
  engine.networkAccessManager();
  engine.load(path);
  QQuickWindow::setDefaultAlphaBuffer(true);
  if (cliParser.isSet(daemonOption)) {
    QGuiApplication::setQuitOnLastWindowClosed(false);
  }
  return QGuiApplication::exec();
}
