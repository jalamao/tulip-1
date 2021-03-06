IF(PYTHONLIBS_FOUND)
IF(SIP_OK)

SET(STL_SIP_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/std_list.sip
    ${CMAKE_CURRENT_SOURCE_DIR}/std_map.sip
    ${CMAKE_CURRENT_SOURCE_DIR}/Module.sip
    ${CMAKE_CURRENT_SOURCE_DIR}/std_pair.sip
    ${CMAKE_CURRENT_SOURCE_DIR}/std_set.sip
    ${CMAKE_CURRENT_SOURCE_DIR}/std_string.sip
    ${CMAKE_CURRENT_SOURCE_DIR}/std_vector.sip
    ${CMAKE_CURRENT_SOURCE_DIR}/std_ostream.sip
    CACHE INTERNAL ""
)

SET(STL_PYTHON_BINDINGS_SRC
    sip_stlpart0.cpp
    sip_stlpart1.cpp
    sip_stlpart2.cpp
    sip_stlpart3.cpp
)

ADD_CUSTOM_COMMAND(OUTPUT ${STL_PYTHON_BINDINGS_SRC}
                   COMMAND ${SIP_EXE} -I ${CMAKE_CURRENT_BINARY_DIR} -c ${CMAKE_CURRENT_BINARY_DIR} -j4 ${CMAKE_CURRENT_SOURCE_DIR}/Module.sip
                   COMMENT "Generating Python Bindings for stl"
                   DEPENDS ${STL_SIP_FILES} VERBATIM)
           
DISABLE_COMPILER_WARNINGS()

# Force to use SIP and headers located in thirdparty
# instead of those installed in the system
INCLUDE_DIRECTORIES(BEFORE ${SIP_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})

ADD_LIBRARY(${LibStlPythonBindingsName} SHARED ${STL_PYTHON_BINDINGS_SRC})

SET_TARGET_PROPERTIES(${LibStlPythonBindingsName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${TULIP_PYTHON_NATIVE_FOLDER})
SET_TARGET_PROPERTIES(${LibStlPythonBindingsName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TULIP_PYTHON_NATIVE_FOLDER})
SET_TARGET_PROPERTIES(${LibStlPythonBindingsName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TULIP_PYTHON_NATIVE_FOLDER})
SET_TARGET_PROPERTIES(${LibStlPythonBindingsName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${TULIP_PYTHON_NATIVE_FOLDER})
SET_TARGET_PROPERTIES(${LibStlPythonBindingsName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${TULIP_PYTHON_NATIVE_FOLDER})
SET_TARGET_PROPERTIES(${LibStlPythonBindingsName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TULIP_PYTHON_NATIVE_FOLDER})


IF(NOT SYSTEM_SIP)
ADD_DEPENDENCIES(${LibStlPythonBindingsName} ${SIP_LIB})
ENDIF(NOT SYSTEM_SIP)

SET_TARGET_PROPERTIES(${LibStlPythonBindingsName}
              PROPERTIES OUTPUT_NAME stl PREFIX "_")

IF(WIN32)
SET_TARGET_PROPERTIES(${LibStlPythonBindingsName}
		      PROPERTIES SUFFIX ".pyd")
ELSE(WIN32)
SET_TARGET_PROPERTIES(${LibStlPythonBindingsName}
		      PROPERTIES SUFFIX ".so")		      
ENDIF(WIN32)

# When building Python wheel for MacOS, don't link the C extension module with the Python library
# and use dynamic lookup for retrieving its symbols.
# That way, we can produce a C extension module that can be imported through the Python interpreter
# provided by Apple with the System and the one provided by Python.org
IF(APPLE AND ACTIVATE_PYTHON_WHEELS_TARGETS)
  SET_TARGET_PROPERTIES(${LibStlPythonBindingsName} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
ELSE(APPLE AND ACTIVATE_PYTHON_WHEELS_TARGETS)
  TARGET_LINK_LIBRARIES(${LibStlPythonBindingsName} ${PYTHON_LIBRARY})
ENDIF(APPLE AND ACTIVATE_PYTHON_WHEELS_TARGETS)

IF(NOT PYTHON_DIST_PACKAGE)
INSTALL(TARGETS ${LibStlPythonBindingsName}
        RUNTIME DESTINATION ${TulipBinInstallDir}/python/tulip/native COMPONENT tulip_python
        LIBRARY DESTINATION ${TulipLibInstallDir}/python/tulip/native COMPONENT tulip_python
        ARCHIVE DESTINATION ${TulipLibInstallDir}/python/tulip/native COMPONENT tulip_python)
ELSE(NOT PYTHON_DIST_PACKAGE)
INSTALL(TARGETS ${LibStlPythonBindingsName}
        RUNTIME DESTINATION ${TulipBinInstallDir}/../lib/python${PYTHON_VERSION}/dist-packages/tulip/native COMPONENT tulip_python
        LIBRARY DESTINATION ${TulipLibInstallDir}/python${PYTHON_VERSION}/dist-packages/tulip/native COMPONENT tulip_python
        ARCHIVE DESTINATION ${TulipLibInstallDir}/python${PYTHON_VERSION}/dist-packages/tulip/native COMPONENT tulip_python)
ENDIF(NOT PYTHON_DIST_PACKAGE)

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "sipAPI_stl.h")

ENDIF(SIP_OK)
ENDIF(PYTHONLIBS_FOUND)
