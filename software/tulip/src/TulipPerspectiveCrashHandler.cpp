#include "TulipPerspectiveCrashHandler.h"

#include "ui_TulipPerspectiveCrashHandler.h"

#include <QtNetwork/QNetworkReply>
#include <QtCore/QFile>
#include "FormPost.h"

TulipPerspectiveCrashHandler::TulipPerspectiveCrashHandler(QWidget *parent)
  : QDialog(parent), _ui(new Ui::TulipPerspectiveCrashHandlerData), _isDetailedView(false) {
  _ui->setupUi(this);
  setDetailedView(false);
#ifndef USE_GOOGLE_BREAKPAD
  _ui->errorReportTitle->setVisible(false);
  _ui->sendReportButton->setVisible(false);
  _ui->detailsLink->setVisible(false);
#endif

  connect(_ui->detailsLink,SIGNAL(linkActivated(QString)),this,SLOT(toggleDetailedView()));
  connect(_ui->sendReportButton,SIGNAL(clicked()),this,SLOT(sendReport()));
  connect(_ui->saveButton,SIGNAL(clicked()),this,SLOT(saveData()));
}

void TulipPerspectiveCrashHandler::setDetailedView(bool f) {
  _isDetailedView = f;
  _ui->detailsFrame->setVisible(f);
  int w = width();
  adjustSize();
  resize(w,height());
}

void TulipPerspectiveCrashHandler::toggleDetailedView() {
  setDetailedView(!_isDetailedView);
}

void TulipPerspectiveCrashHandler::sendReport() {
  if (_reportPath.isNull())
    return;
  FormPost *poster = new FormPost;
  poster->addField("platform",_ui->plateformValue->text());
  poster->addField("compiler",_ui->compilerValue->text());
  poster->addField("arch",_ui->archValue->text());
  poster->addField("perspective_name",_ui->perspectiveNameValue->text());
  poster->addField("tulip_version",_ui->versionValue->text());
  poster->addFile("dump_file",_reportPath,"application/octet-stream");
  connect(poster->postData("http://tulip.labri.fr/crash_report/send_report.php"),SIGNAL(finished()),this,SLOT(reportPosted()));
  _ui->sendReportButton->setText(trUtf8("Sending report"));
  _ui->sendReportButton->setEnabled(false);
}

void TulipPerspectiveCrashHandler::reportPosted() {
  _ui->errorReportTitle->setText(trUtf8("<b>Report has been sent. Thank you for supporting Tulip !</b>"));
  sender()->deleteLater();
}

void TulipPerspectiveCrashHandler::saveData() {

}

#ifdef USE_GOOGLE_BREAKPAD
void TulipPerspectiveCrashHandler::setEnvData(const QString &plateform, const QString &arch, const QString &compiler, const QString &version, const QString &dumpFile) {
  _ui->plateformValue->setText(plateform);
  _ui->archValue->setText(arch);
  _ui->compilerValue->setText(compiler);
  _ui->versionValue->setText(version);
  QFile f(dumpFile);
  f.open(QIODevice::ReadOnly);
  while (!f.atEnd())
    _ui->dumpEdit->setPlainText(_ui->dumpEdit->toPlainText() + f.read(512));
  _reportPath=dumpFile;
}
#endif

void TulipPerspectiveCrashHandler::setPerspectiveData(PerspectiveProcessInfos infos) {
  _perspectiveInfos = infos;
  _ui->perspectiveNameValue->setText(infos.name);
  QString args;
  QString a;
  foreach(a,infos.args.keys())
    args += "--" + a + "=" + infos.args[a].toString() + " ";
  _ui->perspectiveArgumentsValue->setText(args);
}
