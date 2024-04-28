#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickWindow>
#include <qguiapplication.h>

int main(int argc, char **argv) {
  auto app = QGuiApplication(argc, argv);

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

  const auto engine = QQmlApplicationEngine(cliParser.value(pathOption));
  QQuickWindow::setDefaultAlphaBuffer(true);
  if (cliParser.isSet(daemonOption)) {
    app.setQuitOnLastWindowClosed(false);
  }
  return app.exec();
}
