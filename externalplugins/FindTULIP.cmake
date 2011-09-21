SET(TULIP_DIR ${CMAKE_INSTALL_PREFIX})
SET(TULIP_FOUND true)
SET(TULIP_INCLUDE_DIR ${TulipCoreInclude} ${TulipCoreBuildInclude} ${TulipOGLInclude} ${TulipOGDFInclude} ${TulipGUIInclude})
SET(TULIP_LIBRARIES ${LibTulipCoreName} ${LibTulipOglName} ${LibTulipGUIName} ${LibTulipOGDFName} ${LibTulip3CompatName})
IF(WIN32)
  SET(TULIP_LIBRARIES_DIR ${CMAKE_INSTALL_PREFIX}/bin)
ELSEIF(APPLE)
  SET(TULIP_LIBRARIES_DIR ${CMAKE_INSTALL_PREFIX}/Frameworks)
ELSE()
  SET(TULIP_LIBRARIES_DIR ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF()
SET(TULIP_PLUGINS_DIR ${TULIP_LIBRARIES_DIR}/tulip)
SET(TULIP_CORE_LIBRARY ${LibTulipCoreName})
SET(TULIP_OGL_LIBRARY ${LibTulipOglName})
SET(TULIP_GUI_LIBRARY ${LibTulipGUIName})
SET(TULIP_PYTHON_CORE_LIBRARY ${LibTulipPythonName})
SET(TULIP_PYTHON_OGL_LIBRARY ${LibTulipOglPythonName})
SET(TULIP_PYTHON_GUI_LIBRARY ${LibTulipGUIPythonName})
SET(TULIP_OGDF_LIBRARY ${LibTulipOGDFName})
SET(TULIP3_COMPAT_LIBRARY ${LibTulip3CompatName})
SET(TULIP_VERSION ${TulipVersion})
SET(TULIP_MAJOR_VERSION ${TulipMajorVersion})
SET(TULIP_MINOR_VERSION ${TulipMinorVersion})
SET(TULIP_PATCH_VERSION ${TulipReleaseVersion})
