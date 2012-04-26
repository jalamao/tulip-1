INCLUDE_DIRECTORIES(${TulipCoreInclude} ${TulipGUIInclude} ${TulipOGLInclude} ${TulipCoreBuildInclude} ${QuazipInclude})

QT4_WRAP_CPP(pluginPusher_MOC_SRCS WebDavManager.h)
ADD_EXECUTABLE(pushPlugins PluginPusher.cpp WebDavManager.cpp ${pluginPusher_MOC_SRCS})

ADD_EXECUTABLE(packagePlugins PluginPackager.cpp)

# needs to link against all Tulip libs to be able to laod all tulip plugins
target_link_libraries(packagePlugins ${QT_LIBRARIES}
                      ${QT_QTXML_LIBRARY}
                      ${LibTulipCoreName}
                      ${LibTulipGUIName}
                      ${LibTulipOglName}
                      ${LibTulipOGDFName}
                      ${OGDFLibrary}
                      pthread
                      quazip)

target_link_libraries(pushPlugins ${QT_LIBRARIES}
                      ${LibTulipGUIName}
                      ${QT_QTXML_LIBRARY})

IF(TULIP_GENERATE_PLUGINSERVER_TOOLS)
  INSTALL(TARGETS packagePlugins DESTINATION ${TulipBinInstallDir} COMPONENT plugin_server)
  INSTALL(TARGETS pushPlugins DESTINATION ${TulipBinInstallDir} COMPONENT plugin_server)
  BUNDLE_DEFINE_COMPONENT(plugin_server "plugin server" " " "Dev tools")
ENDIF()