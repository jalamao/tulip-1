IF(CHECK_MEMORY)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CHECK_MEMORY_FLAGS}")
ENDIF(CHECK_MEMORY)

ADD_SUBDIRECTORY(bitmaps)
ADD_SUBDIRECTORY(crash_handling)
ADD_SUBDIRECTORY(tulip)
ADD_SUBDIRECTORY(tulip_perspective)
ADD_SUBDIRECTORY(plugin_server)
IF(APPLE)
ADD_SUBDIRECTORY(mac_launcher)
ENDIF(APPLE)
