#include <tulip/PluginInformations.h>

#include <tulip/WithDependency.h>
#include <tulip/AbstractPluginInfo.h>
#include <tulip/PluginLister.h>
#include <tulip/TulipRelease.h>

#include <QtCore/QFileInfo>
#include <tulip/TlpQtTools.h>
#include <tulip/DownloadManager.h>
#include <QDesktopServices>
#include <tulip/QuaZIPFacade.h>
#include <qtextstream.h>
#include <tulip/TulipSettings.h>

using namespace tlp;

template <class T, class U>
static std::list<tlp::Dependency> getPluginDependencies(T* factory, U context) {
  return StaticPluginLister<T, U>::getPluginDependencies(factory->getName());
}

PluginInformations::PluginInformations(const tlp::AbstractPluginInfo& info, const std::string& type, const std::string& library)
  :_lastVersion(info.getRelease().c_str()), _type(type.c_str()), _iconPath(":/tulip/gui/icons/logo32x32.png"), _longDescriptionPath("http://www.perdu.com"), _isLocal(true),
   _installedVersion(info.getRelease().c_str()), _updateAvailable(false), _version(info.getRelease().c_str()), _infos(&info), _library(library.c_str()) {
}

PluginInformations::PluginInformations(const tlp::AbstractPluginInfo& info, const QString& type, const QString& basePath, const QString& remotepluginName)
  :_lastVersion(info.getRelease().c_str()), _type(type), _iconPath(basePath + "/icon.png"), _longDescriptionPath(basePath + "/html/index.html"), _isLocal(false), _installedVersion(QString::null),
   _updateAvailable(false), _remoteLocation(basePath + "/" + remotepluginName), _remoteArchive(_remoteLocation + "/" + tlp::getPluginPackageName(remotepluginName)), _version(info.getRelease().c_str()), _infos(&info) {
}

void PluginInformations::AddPluginInformations(const tlp::AbstractPluginInfo* info) {
  QString newVersion = info->getRelease().c_str();

  if(_installedVersion < newVersion) {
    _updateAvailable = true;
    _lastVersion = newVersion;
  }
}

QString PluginInformations::identifier() const {
  return _infos->getName().c_str();
}

QString PluginInformations::name() const {
  return _infos->getName().c_str();
}

QString PluginInformations::author() const {
  return _infos->getAuthor().c_str();
}

QString PluginInformations::group() const {
  return _infos->getGroup().c_str();
}

QString PluginInformations::shortDescription() const {
  return _infos->getInfo().c_str();
}

QString PluginInformations::longDescriptionPath() const {
  return _longDescriptionPath;
}

QString PluginInformations::iconPath() const {
  return _iconPath;
}

QDateTime PluginInformations::installDate() const {
  return QDateTime::currentDateTime();
}

QString PluginInformations::type() const {
  return _type;
}

const QStringList PluginInformations::dependencies() const {
  QStringList result;
  result.reserve(_infos->getDependencies().size());

  for(std::list<tlp::Dependency>::const_iterator it = _infos->getDependencies().begin(); it != _infos->getDependencies().end(); ++it) {
    result.append(it->pluginName.c_str());
  }

  return result;
}

const QString& PluginInformations::version() const {
  return _version;
}

QString PluginInformations::installedVersion() const {
  return _installedVersion;
}

bool PluginInformations::isInstalled() const {
  return !(installedVersion().isEmpty());
}

bool PluginInformations::updateAvailable() const {
  return _updateAvailable;
}

QString PluginInformations::latestVersion() const {
  return _lastVersion;
}

bool PluginInformations::fetch() const {
  bool result = false;
  const QString archiveName = tlp::getPluginPackageName(name());
  QNetworkReply* reply = DownloadManager::getInstance()->downloadPlugin(_remoteArchive, tlp::getPluginStagingDirectory() + archiveName);
  emit(DownloadStarted(reply));

  std::cout << isInstalled() << ": " << installedVersion().toStdString() << ";" << _installedVersion.isEmpty() << std::endl;

  if(!isInstalled()) {
    tlp::SimplePluginProgress* progress = new tlp::SimplePluginProgress();
    result = QuaZIPFacade::unzip(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/plugins", tlp::getPluginStagingDirectory() + "/" + archiveName, progress);
//     std::cout << archiveName.toStdString() << ":" << result << "; " << progress->getError() << std::endl;
//     PluginLibraryLoader::loadPlugins();
  }

  return result;
}

void PluginInformations::remove() const {
  TulipSettings::instance().markPluginForRemoval(_library);
}
