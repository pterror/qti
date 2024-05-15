#include "process.hpp"

QString Process::program() const { return this->mProcess.program(); }
void Process::setProgram(const QString &program) {
  this->mProcess.setProgram(program);
  emit this->programChanged();
}

QStringList Process::arguments() const { return this->mProcess.arguments(); }
void Process::setArguments(const QStringList &arguments) {
  this->mProcess.setArguments(arguments);
  emit this->argumentsChanged();
}

void Process::startDetached() { this->mProcess.startDetached(); }

// FIXME: implement everything else
