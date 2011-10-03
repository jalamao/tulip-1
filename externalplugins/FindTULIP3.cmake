set(TULIP_INCLUDE_DIR ${TulipInclude} ${TulipBuildInclude} ${TulipOGLInclude} ${TulipOGLBuildInclude} ${TulipQtInclude} ${TulipQtBuildInclude} ${TulipPluginsManagerInclude})
set(TULIP_LIBRARY ${LibTulipName})
set(TULIP_OGL_LIBRARY ${LibTulipOglName})
set(TULIP_QT4_LIBRARY ${LibTulipQtName})
set(TULIP_PLUGINS_MANAGER_LIBRARY ${LibTulipPluginsManagerName})
set(TULIP_LIBS ${TULIP_LIBRARY} ${TULIP_OGL_LIBRARY} ${TULIP_QT4_LIBRARY})
set(TULIP_FOUND "YES")
set(TULIP_VERSION	"${TulipVersion}")
set(TULIP_PLUGIN_VERSION "${TulipVersion}")
set(TULIP_PLUGINS_PATH "${TulipPluginsInstallDir}")
SET(TULIP_DIR ${CMAKE_INSTALL_PREFIX})
set(TULIP_SHARE_PATH ${TulipShareInstallDir})
