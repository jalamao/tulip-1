IF(NOT MSVC)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(NOT MSVC)

SET(SRCS
  QtAwesome.cpp
)

QTX_WRAP_CPP(MOC_SRCS
  QtAwesome.h
)

ADD_LIBRARY(${QtAwesomeLibrary} STATIC ${SRCS} ${MOC_SRCS})
