SET(tulip_LIB_SRCS ${tulip_LIB_SRCS}
Plugin.cpp
AcyclicTest.cpp
Bfs.cpp
BiconnectedTest.cpp
BooleanProperty.cpp
BoundingBox.cpp
Color.cpp
ColorProperty.cpp
ColorScale.cpp
ConnectedTest.cpp
ConvexHull.cpp
DataSet.cpp
DoubleProperty.cpp
DrawingTools.cpp
FaceIterator.cpp
GraphAbstract.cpp
Graph.cpp
GraphDecorator.cpp
GraphImpl.cpp
GraphIterator.cpp
GraphMeasure.cpp
GraphProperty.cpp
GraphStorage.cpp
GraphTools.cpp
GraphUpdatesRecorder.cpp
GraphView.cpp
IdManager.cpp
IntegerProperty.cpp
LayoutProperty.cpp
MapIterator.cpp
Observable.cpp
ObservableGraph.cpp
ObservableProperty.cpp
Ordering.cpp
OuterPlanarTest.cpp
PlanarConMap.cpp
PlanarityTest.cpp
PlanarityTestEmbed.cpp
PlanarityTestImpl.cpp
PlanarityTestObstr.cpp
PlanarityTestTools.cpp
Plane.cpp
PluginLibraryLoader.cpp
PluginLoaderTxt.cpp
PreferenceManager.cpp
PropertyAlgorithm.cpp
PropertyInterface.cpp
PropertyManager.cpp
PropertyTypes.cpp
SimplePluginProgress.cpp
SimpleTest.cpp
SizeProperty.cpp
StlFunctions.cpp
StringCollection.cpp
StringProperty.cpp
TLPExport.cpp
TLPImport.cpp
TlpJsonExport.cpp
TlpJsonImport.cpp
TlpTools.cpp
TreeTest.cpp
TriconnectedTest.cpp
vectorgraph.cpp
WithParameter.cpp
YajlFacade.cpp
PluginLister.cpp
tulipconf.cpp
TulipException.cpp)

IF(CHECK_MEMORY)
SET(tulip_LIB_SRCS ${tulip_LIB_SRCS} MemoryChecker.cpp)
ENDIF(CHECK_MEMORY)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_TULIP_LIB_DIR=\"\\\"${CMAKE_INSTALL_PREFIX}/lib/\\\"\"")
INCLUDE_DIRECTORIES(../include/ ${TulipCoreBuildInclude} ${PROJECT_SOURCE_DIR} ${ZLIB_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${YAJL_INCLUDE_DIRS})

ADD_LIBRARY(${LibTulipCoreName} SHARED ${tulip_LIB_SRCS} ../include/tulip/TulipRelease.h)
TARGET_LINK_LIBRARIES(${LibTulipCoreName} gzstream ${QT_QTCORE_LIBRARY} yajl)
IF(WIN32)
  IF(MSVC)
    TARGET_LINK_LIBRARIES(${LibTulipCoreName} Dbghelp)
    IF(${CMAKE_GENERATOR} MATCHES "Visual Studio 9" OR ${CMAKE_GENERATOR} MATCHES "NMake Makefiles")
      SET_TARGET_PROPERTIES(${LibTulipCoreName} PROPERTIES LINK_FLAGS " /NODEFAULTLIB:LIBCMT.lib ")
    ENDIF()
  ENDIF()
ENDIF(WIN32)

INSTALL(TARGETS ${LibTulipCoreName}
  RUNTIME DESTINATION ${TulipBinInstallDir} COMPONENT tulip_core
  LIBRARY DESTINATION ${TulipLibInstallDir} COMPONENT tulip_core
  ARCHIVE DESTINATION ${TulipLibInstallDir} COMPONENT tulip_core)