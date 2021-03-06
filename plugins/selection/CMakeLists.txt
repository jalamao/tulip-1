ADD_CORE_FILES(.)

INCLUDE_DIRECTORIES(${TulipCoreInclude} ${TulipCoreBuildInclude})

##----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(SpanningDagSelection-${TulipVersion} SHARED SpanningDagSelection.cpp SpanningDagSelection.h)
TARGET_LINK_LIBRARIES(SpanningDagSelection-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(ReachableSubGraphSelection-${TulipVersion} SHARED ReachableSubGraphSelection.h ReachableSubGraphSelection.cpp)
TARGET_LINK_LIBRARIES(ReachableSubGraphSelection-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(SpanningTreeSelection-${TulipVersion} SHARED SpanningTreeSelection.cpp SpanningTreeSelection.h)
TARGET_LINK_LIBRARIES(SpanningTreeSelection-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(InducedSubGraphSelection-${TulipVersion} SHARED InducedSubGraphSelection.cpp InducedSubGraphSelection.h)
TARGET_LINK_LIBRARIES(InducedSubGraphSelection-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(LoopSelection-${TulipVersion} SHARED LoopSelection.cpp LoopSelection.h)
TARGET_LINK_LIBRARIES(LoopSelection-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(MultipleSelection-${TulipVersion} SHARED MultipleEdgeSelection.cpp MultipleEdgeSelection.h)
TARGET_LINK_LIBRARIES(MultipleSelection-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
ADD_LIBRARY(Kruskal-${TulipVersion} SHARED Kruskal.cpp Kruskal.h)
TARGET_LINK_LIBRARIES(Kruskal-${TulipVersion} ${LibTulipCoreName})

##----------------------------------------------------------------------------------------------------------------------------
INSTALL_TULIP_PLUGIN(SpanningDagSelection-${TulipVersion} ${TulipPluginsInstallDir})
INSTALL_TULIP_PLUGIN(ReachableSubGraphSelection-${TulipVersion} ${TulipPluginsInstallDir})
INSTALL_TULIP_PLUGIN(SpanningTreeSelection-${TulipVersion} ${TulipPluginsInstallDir})
INSTALL_TULIP_PLUGIN(InducedSubGraphSelection-${TulipVersion} ${TulipPluginsInstallDir})
INSTALL_TULIP_PLUGIN(LoopSelection-${TulipVersion} ${TulipPluginsInstallDir})
INSTALL_TULIP_PLUGIN(MultipleSelection-${TulipVersion} ${TulipPluginsInstallDir})
INSTALL_TULIP_PLUGIN(Kruskal-${TulipVersion} ${TulipPluginsInstallDir})
