#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QRegularExpression>

const auto FILE_DIRECTORY_AND_NAME_REG_EXP =
    QRegularExpression("(.*)/([^/]+?)(?:[.]qml)?$");

int main(int argc, char **argv) {
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

  const auto engine = QQmlApplicationEngine(path);
  QQuickWindow::setDefaultAlphaBuffer(true);
  if (cliParser.isSet(daemonOption)) {
    QGuiApplication::setQuitOnLastWindowClosed(false);
  }
  return QGuiApplication::exec();
}
