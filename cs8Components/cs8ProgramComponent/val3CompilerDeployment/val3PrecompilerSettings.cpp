#include <QSettings>
#include <QTimer>
#include <cs8application.h>

#include "ui_val3PrecompilerSettings.h"
#include "val3PrecompilerSettings.h"

val3PrecompilerSettings::val3PrecompilerSettings(QDialog *parent)
    : QDialog(parent), ui(new Ui::val3PrecompilerSettings) {
  ui->setupUi(this);
  QCoreApplication::setApplicationName("Val3Linter");
  QCoreApplication::setOrganizationName("Saxe Group");
  QCoreApplication::setOrganizationDomain("saxe-group.com");

  cs8Application app;
  app.initPrecompilerSettings();

  QSettings settings;
  ui->cpUnusedPublicGlobalVar->setChecked(
      settings.value("reportUnusedPublicGlobalVariables", true).toBool());
  ui->cpHiddenGlobalVariables->setChecked(
      settings.value("reportHiddenGlobalVariables", true).toBool());
  ui->cpParametersPostfix->setChecked(
      settings.value("reportParametersPostfix", true).toBool());
  ui->cpReportsToDos->setChecked(settings.value("reportToDos", true).toBool());
}

val3PrecompilerSettings::~val3PrecompilerSettings() { delete ui; }

void val3PrecompilerSettings::changeEvent(QEvent *e) {
  QWidget::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void val3PrecompilerSettings::on_cpUnusedPublicGlobalVar_clicked(bool checked) {
  QSettings settings;
  settings.setValue("reportUnusedPublicGlobalVariables", checked);
}

void val3PrecompilerSettings::on_buttonBox_accepted() { close(); }

void val3PrecompilerSettings::on_buttonBox_rejected() { close(); }

void val3PrecompilerSettings::on_cpHiddenGlobalVariables_toggled(bool checked) {
  QSettings settings;
  settings.setValue("reportHiddenGlobalVariables", checked);
}

void val3PrecompilerSettings::on_cpParametersPostfix_toggled(bool checked) {
  QSettings settings;
  settings.setValue("reportParametersPostfix", checked);
}

void val3PrecompilerSettings::on_cpReportsToDos_toggled(bool checked) {
  QSettings settings;
  settings.setValue("reportToDos", checked);
}

void val3PrecompilerSettings::slotStartup() {}
