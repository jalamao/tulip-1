ADD_CORE_FILES(CMakeLists.txt)

IF(CHECK_MEMORY)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CHECK_MEMORY_FLAGS}")
ENDIF(CHECK_MEMORY)

ADD_SUBDIRECTORY(clustering)
ADD_SUBDIRECTORY(export)
ADD_SUBDIRECTORY(general)
ADD_SUBDIRECTORY(import)
ADD_SUBDIRECTORY(layout)
ADD_SUBDIRECTORY(metric)
ADD_SUBDIRECTORY(selection)
ADD_SUBDIRECTORY(sizes)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(colors)
IF(NOT BUILD_CORE_ONLY)
ADD_SUBDIRECTORY(glyph)
ADD_SUBDIRECTORY(interactor)
# Tulip perspective is not bundled in tulipgui Python wheel
IF(NOT ACTIVATE_PYTHON_WHEELS_TARGETS)
ADD_SUBDIRECTORY(perspective)
ENDIF(NOT ACTIVATE_PYTHON_WHEELS_TARGETS)
ADD_SUBDIRECTORY(string)
ADD_SUBDIRECTORY(view)
ENDIF(NOT BUILD_CORE_ONLY)
