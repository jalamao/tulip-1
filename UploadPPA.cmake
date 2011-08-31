##
# CMake script to build a tulip source package for ubuntu and upload it to the Tulip PPA
# 
# Inspired from UploadPPA.cmake by Daniel Pfeifer <daniel@pfeifer-mail.de> 
# http://code.mjdsystems.ca/p/equalizer/source/tree/master/CMake/UploadPPA.cmake
# 
# and from DebSourcePPA.cmake by Rosen Diankov <rosen.diankov@gmail.com>
# https://openrave.svn.sourceforge.net/svnroot/openrave/trunk/modules-cmake/DebSourcePPA.cmake
# 
# author: Antoine Lambert <antoine.lambert@labri.fr>
#
# To call this script and upload the created packages, execute the following commands in the CMake build directory:
# > cmake -DENABLE_DPUT_TARGET=ON . (prepare the packaging : copying the source tree and generate package description files)  
# > make dput (build the source packages and upload them to the PPA)
#
# Once it is done, you should desactivate the dput target by reexecuting cmake the following way:
# > cmake -DENABLE_DPUT_TARGET=OFF .
# (otherwise the packaging preparation step will be executed each time make is called)
##

# some cmake options to set when building the binary packages 
SET(CPACK_DEBIAN_CMAKE_OPTIONS "-DUBUNTU_PPA_BUILD=ON -DCMAKE_BUILD_TYPE=Release")
# the directories to copy from the source tree
SET(SOURCE_DIRS_TO_COPY "library;software;thirdparty;plugins;textures;docs;externalplugins")
# the launcphad identifier
SET(LAUNCHPAD_ID "tulipsoftware")
# the destination ppa host
SET(DPUT_HOST "ppa:${LAUNCHPAD_ID}/ppa")
# the url of the uploaded source archives 
SET(PPA_ARCHIVES_URL "http://ppa.launchpad.net/${LAUNCHPAD_ID}/ppa/ubuntu/pool/main/t/tulip/")	
# the user name associated to the GPG key used to sign the source packages
SET(GPG_USERNAME "Tulip Dev Team")

SET(PACKAGE_WITH_DEBUG_INFOS "libtulip;libtulip-ogl;libtulip-qt;tulip;tulip-plugins;tulip-python;tulip-ogdf")

EXECUTE_PROCESS(COMMAND date -R  OUTPUT_VARIABLE DATE_TIME)

FIND_PROGRAM(DEBUILD_EXECUTABLE debuild)
FIND_PROGRAM(DPUT_EXECUTABLE dput)

IF(NOT DEBUILD_EXECUTABLE OR NOT DPUT_EXECUTABLE)
  MESSAGE("Executable debuild and dput are needed to construct the source package to upload to the Tulip PPA (install packages devscripts and dput to get them)")
  RETURN()
ENDIF(NOT DEBUILD_EXECUTABLE OR NOT DPUT_EXECUTABLE)

# DEBIAN/control
# debian policy enforce lower case for package name
# Package: (mandatory)
IF(NOT CPACK_DEBIAN_PACKAGE_NAME)
  STRING(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_DEBIAN_PACKAGE_NAME)
ENDIF(NOT CPACK_DEBIAN_PACKAGE_NAME)

# Section: (recommended)
IF(NOT CPACK_DEBIAN_PACKAGE_SECTION)
  SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")
ENDIF(NOT CPACK_DEBIAN_PACKAGE_SECTION)

# Priority: (recommended)
IF(NOT CPACK_DEBIAN_PACKAGE_PRIORITY)
  SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
ENDIF(NOT CPACK_DEBIAN_PACKAGE_PRIORITY)

# run a shell script to generate a version number for the package to upload (user input required)
EXECUTE_PROCESS(COMMAND bash ${CMAKE_SOURCE_DIR}/gen_debian_package_version_number.sh ${TulipVersion} ${PPA_ARCHIVES_URL}
		ERROR_VARIABLE PACKAGE_VERSION)

STRING(REPLACE "\n" "" PACKAGE_VERSION "${PACKAGE_VERSION}")

SET(CPACK_DEBIAN_PACKAGE_REVISION "-0ubuntu${PACKAGE_VERSION}")

######################################################################################
# copy the source tree (list of directories to copy must be provided in the SOURCE_DIRS_TO_COPY variable)
FILE(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/Debian)
EXECUTE_PROCESS(COMMAND mkdir ${CMAKE_BINARY_DIR}/Debian)
SET(DEBIAN_SOURCE_ORIG_DIR ${CMAKE_BINARY_DIR}/Debian/${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION})

EXECUTE_PROCESS(COMMAND ls -l  
                COMMAND grep ^-
		COMMAND awk "{print $8}"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE FILES_TO_COPY)

STRING(REPLACE "\n" ";" FILES_TO_COPY ${FILES_TO_COPY})

EXECUTE_PROCESS(COMMAND mkdir "${DEBIAN_SOURCE_ORIG_DIR}.orig")

FOREACH(FILE_TO_CP ${FILES_TO_COPY})
  EXECUTE_PROCESS(COMMAND cp ${CMAKE_SOURCE_DIR}/${FILE_TO_CP} "${DEBIAN_SOURCE_ORIG_DIR}.orig")
ENDFOREACH(FILE_TO_CP) 

FOREACH(DIR_TO_CP ${SOURCE_DIRS_TO_COPY})
  EXECUTE_PROCESS(COMMAND rsync -rC ${CMAKE_SOURCE_DIR}/${DIR_TO_CP} "${DEBIAN_SOURCE_ORIG_DIR}.orig")
ENDFOREACH(DIR_TO_CP) 

# Check if the original source tarball has been uploaded and download it
MESSAGE("Try to downloading tulip_${TulipVersion}.orig.tar.gz from ${PPA_ARCHIVES_URL}")
EXECUTE_PROCESS(COMMAND wget ${PPA_ARCHIVES_URL}tulip_${TulipVersion}.orig.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Debian
		RESULT_VARIABLE WGET_RET)

IF(NOT WGET_RET EQUAL 0)
  MESSAGE("tulip_${TulipVersion}.orig.tar.gz not found in ${PPA_ARCHIVES_URL}. It will be created and uploaded.")
  # create the original source tarball
  EXECUTE_PROCESS(COMMAND tar -czf "${CPACK_DEBIAN_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}.orig.tar.gz" "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.orig/" WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Debian)  
  SET(DEBUILD_OPTIONS "-sa")
ELSE()
  SET(DEBUILD_OPTIONS "-sd")
ENDIF()

FOREACH(RELEASE ${CPACK_UBUNTU_DISTRIBUTION_RELEASES})
  SET(DEBIAN_SOURCE_DIR "${DEBIAN_SOURCE_ORIG_DIR}${CPACK_DEBIAN_PACKAGE_REVISION}~${RELEASE}1")
  SET(RELEASE_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}${CPACK_DEBIAN_PACKAGE_REVISION}~${RELEASE}1")
  STRING(TOUPPER ${RELEASE} RELEASE_UPPER)
  EXECUTE_PROCESS(COMMAND cp -R ${DEBIAN_SOURCE_ORIG_DIR}.orig ${DEBIAN_SOURCE_DIR})
  EXECUTE_PROCESS(COMMAND mkdir ${DEBIAN_SOURCE_DIR}/debian)

  ############################################################################## 
  # debian/control
  SET(DEBIAN_CONTROL ${DEBIAN_SOURCE_DIR}/debian/control)
  FILE(WRITE ${DEBIAN_CONTROL}
    "Source: ${CPACK_DEBIAN_PACKAGE_NAME}\n"
    "Section: ${CPACK_DEBIAN_PACKAGE_SECTION}\n"
    "Priority: ${CPACK_DEBIAN_PACKAGE_PRIORITY}\n"
    "Maintainer: ${CPACK_DEBIAN_PACKAGE_MAINTAINER}\n"
    "Build-Depends: ")

  FOREACH(DEP ${CPACK_${RELEASE_UPPER}_BUILD_DEPENDS})
    FILE(APPEND ${DEBIAN_CONTROL} "${DEP}, ")
  ENDFOREACH(DEP ${CPACK_${RELEASE_UPPER}_BUILD_DEPENDS})  

  FILE(APPEND ${DEBIAN_CONTROL} "debhelper, cmake\n"
    "Standards-Version: 3.8.4\n"
    "Homepage: ${CPACK_PACKAGE_VENDOR}\n"
    "\n"
  )

  FOREACH(COMPONENT ${CPACK_COMPONENTS_ALL})
    LIST(FIND CPACK_DEBIAN_EXCLUDE_COMPONENTS ${COMPONENT} EXCLUDED)
    IF(${EXCLUDED} EQUAL -1)
      STRING(TOUPPER ${COMPONENT} UPPER_COMPONENT)
      SET(DEPENDS "\${shlibs:Depends}, \${misc:Depends}, ")
      FOREACH(DEP ${CPACK_COMPONENT_${UPPER_COMPONENT}_${RELEASE_UPPER}_DEPENDS})
	IF("${DEPENDS}" STREQUAL "")
	  SET(DEPENDS "${DEP}")
	ELSE()
	  SET(DEPENDS "${DEPENDS}, ${DEP}")
	ENDIF()
      ENDFOREACH(DEP ${CPACK_COMPONENT_${UPPER_COMPONENT}_${RELEASE_UPPER}_DEPENDS})
      FILE(APPEND ${DEBIAN_CONTROL} "\n"
	"Package: ${COMPONENT}\n"
	"Architecture: any\n"
	"Depends: ${DEPENDS}\n"
	"Description: ${CPACK_PACKAGE_DESCRIPTION_SUMMARY}"
	": ${CPACK_COMPONENT_${UPPER_COMPONENT}_DISPLAY_NAME}\n"
	" ${CPACK_COMPONENT_${UPPER_COMPONENT}_DESCRIPTION}\n")
      LIST(FIND PACKAGE_WITH_DEBUG_INFOS ${COMPONENT} DEBUG_INFOS)
      IF(NOT ${DEBUG_INFOS} EQUAL -1)
	FILE(APPEND ${DEBIAN_CONTROL} "\n"
	  "Package: ${COMPONENT}-dbg\n"
	  "Architecture: any\n"
	  "Section: debug\n"
	  "Priority: extra\n"
	  "Depends: ${COMPONENT} (= \${binary:Version}), \${misc:Depends}\n"
	  "Description: ${CPACK_PACKAGE_DESCRIPTION_SUMMARY}"
	  ": ${CPACK_COMPONENT_${UPPER_COMPONENT}_DISPLAY_NAME} - Debug Symbols\n"
	  " ${CPACK_COMPONENT_${UPPER_COMPONENT}_DESCRIPTION}\n")
      ENDIF()
    ENDIF()
  ENDFOREACH(COMPONENT ${CPACK_COMPONENTS_ALL})

  ##############################################################################
  # debian/copyright
  SET(DEBIAN_COPYRIGHT ${DEBIAN_SOURCE_DIR}/debian/copyright)
  EXECUTE_PROCESS(COMMAND cp ${CPACK_RESOURCE_FILE_LICENSE} ${DEBIAN_COPYRIGHT})
  
  ##############################################################################
  # debian/rules
  SET(DEBIAN_RULES ${DEBIAN_SOURCE_DIR}/debian/rules)
  FILE(WRITE ${DEBIAN_RULES}
    "#!/usr/bin/make -f\n"
    "\n"
    "BUILDDIR = build_dir\n"
    "\n"
    "build:\n"
    "	mkdir $(BUILDDIR)\n"
    "	cd $(BUILDDIR); cmake ${CPACK_DEBIAN_CMAKE_OPTIONS} ..\n"
    "	make -C $(BUILDDIR) preinstall\n"
    "	cd $(BUILDDIR); cmake ${CPACK_DEBIAN_CMAKE_OPTIONS} ..\n"
    "	make -C $(BUILDDIR)/docs doc\n"
    "	touch build\n"
    "\n"
    "binary: binary-indep binary-arch\n"
    "\n"
    "binary-indep: build\n"
    "\n"
    "binary-arch: build\n"
     "	cd $(BUILDDIR); cmake ${CPACK_DEBIAN_CMAKE_OPTIONS} -DCOMPONENT=Unspecified -DCMAKE_INSTALL_PREFIX=../debian/tmp/usr -P cmake_install.cmake\n"
    "	mkdir -p debian/tmp/DEBIAN\n"
    "	dpkg-gensymbols -p${CPACK_DEBIAN_PACKAGE_NAME}\n")

  FOREACH(COMPONENT ${CPACK_COMPONENTS_ALL})
    LIST(FIND CPACK_DEBIAN_EXCLUDE_COMPONENTS ${COMPONENT} EXCLUDED)
    IF(${EXCLUDED} EQUAL -1)
      SET(PATH debian/${COMPONENT})
      FILE(APPEND ${DEBIAN_RULES}
	"	cd $(BUILDDIR); cmake ${CPACK_DEBIAN_CMAKE_OPTIONS} -DCOMPONENT=${COMPONENT} -DCMAKE_INSTALL_PREFIX=../${PATH}/usr -P cmake_install.cmake\n"
	"	mkdir ${PATH}/DEBIAN\n"
	"	dpkg-gensymbols -p${COMPONENT} -P${PATH}\n")
    ENDIF()
  ENDFOREACH(COMPONENT ${CPACK_COMPONENTS_ALL})

  FILE(APPEND ${DEBIAN_RULES}
    "	dh_shlibdeps\n")
  FOREACH(COMPONENT ${PACKAGE_WITH_DEBUG_INFOS})  
    SET(PATH debian/${COMPONENT}-dbg)
    FILE(APPEND ${DEBIAN_RULES}
      "	dh_strip -p${COMPONENT} --dbg-package=${COMPONENT}-dbg\n"
      "	dpkg-gencontrol -p${COMPONENT}-dbg -P${PATH}\n"
      "	dpkg --build ${PATH} ..\n")
  ENDFOREACH(COMPONENT ${PACKAGE_WITH_DEBUG_INFOS})
  
  FOREACH(COMPONENT ${CPACK_COMPONENTS_ALL})
    LIST(FIND CPACK_DEBIAN_EXCLUDE_COMPONENTS ${COMPONENT} EXCLUDED)
    IF(${EXCLUDED} EQUAL -1)
      SET(PATH debian/${COMPONENT})
      FILE(APPEND ${DEBIAN_RULES}
	"	dpkg-gencontrol -p${COMPONENT} -P${PATH} -Tdebian/${COMPONENT}.substvars\n"
	"	dpkg --build ${PATH} ..\n")
    ENDIF()
  ENDFOREACH(COMPONENT ${CPACK_COMPONENTS_ALL})

  FILE(APPEND ${DEBIAN_RULES}
    "\n"
    "clean:\n"
    "	rm -f build\n"
    "	rm -rf $(BUILDDIR)\n"
    "\n"
    ".PHONY: binary binary-arch binary-indep clean\n")

  EXECUTE_PROCESS(COMMAND chmod +x ${DEBIAN_RULES})

  ##############################################################################
  # debian/compat
  FILE(WRITE ${DEBIAN_SOURCE_DIR}/debian/compat "7")

  ##############################################################################
  # debian/source/format
  FILE(WRITE ${DEBIAN_SOURCE_DIR}/debian/source/format "3.0 (quilt)")
  
  # calling accidentally cmake in the source directory can generate the sip binary in the source tree
  # add it to debian/source/include-binaries to prevent package upload rejection
  FILE(WRITE ${DEBIAN_SOURCE_DIR}/debian/source/include-binaries "thirdparty/sip-4.12.1/sipgen/sip")

  ##############################################################################
  # debian/changelog
  SET(DEBIAN_CHANGELOG ${DEBIAN_SOURCE_DIR}/debian/changelog)
  
  FILE(WRITE ${DEBIAN_CHANGELOG}
    "${CPACK_DEBIAN_PACKAGE_NAME} (${CPACK_PACKAGE_VERSION}${CPACK_DEBIAN_PACKAGE_REVISION}~${RELEASE}1) ${RELEASE}; urgency=medium\n\n"
    "  ${CPACK_DEBIAN_CHANGELOG}\n\n"
    " -- ${CPACK_DEBIAN_PACKAGE_MAINTAINER}  ${DATE_TIME}\n")

  ##############################################################################
  # debuild -S
  SET(SOURCE_CHANGES_FILE "${CPACK_DEBIAN_PACKAGE_NAME}_${RELEASE_PACKAGE_VERSION}_source.changes")
  SET(DEB_SOURCE_CHANGES ${DEB_SOURCE_CHANGES} "${SOURCE_CHANGES_FILE}")
  ADD_CUSTOM_COMMAND(OUTPUT "${SOURCE_CHANGES_FILE}" COMMAND ${DEBUILD_EXECUTABLE} -k${GPG_USERNAME} -S ${DEBUILD_OPTIONS} WORKING_DIRECTORY ${DEBIAN_SOURCE_DIR})
ENDFOREACH(RELEASE ${CPACK_DEBIAN_DISTRIBUTION_RELEASES})

##############################################################################
# dput ppa:your-lp-id/ppa <source.changes>
ADD_CUSTOM_TARGET(dput ${DPUT_EXECUTABLE} ${DPUT_HOST} ${DEB_SOURCE_CHANGES} 
                  DEPENDS ${DEB_SOURCE_CHANGES} 
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Debian)
