################################################
# Implements install target!
# It will be included by ./src/CMakeLists.txt
################################################

SET(TRANSLATION_DESTINATION translations)
SET(DEFAULT_FILE_DESTINATION .)

IF(CMAKE_PREFIX_PATH)
	STRING(REPLACE "\\" "/" TOOLCHAIN_PATH ${CMAKE_PREFIX_PATH})
	SET(TOOLCHAIN_BIN_PATH ${TOOLCHAIN_PATH}/bin)
	SET(TOOLCHAIN_LIB_PATH ${TOOLCHAIN_PATH}/lib)
ENDIF()


SET(SEARCH_ADDITIONAL_DIRS "
			SET(CMAKE_MODULE_PATH \"${CMAKE_MODULE_PATH}\")
			INCLUDE(Helper)
			DIRLIST_OF_FILES(ADDITIONAL_DIRS ${CMAKE_BINARY_DIR}/src/*${CMAKE_SHARED_LIBRARY_SUFFIX})
")


IF(WIN32)
	IF(MSVC)
		SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
		IF(NOT CMAKE_VERSION VERSION_LESS "3.6")
			SET(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
		ENDIF()
		INCLUDE(InstallRequiredSystemLibraries)
	ENDIF()

	FETCH_TARGET_LOCATION(libSvg "Qt5::Svg")
	FETCH_TARGET_LOCATION(pluginSvg "Qt5::QSvgPlugin")
	FETCH_TARGET_LOCATION(pluginGif "Qt5::QGifPlugin")
	FETCH_TARGET_LOCATION(pluginJpeg "Qt5::QJpegPlugin")
	IF(WINDOWS_STORE)
		FETCH_TARGET_LOCATION(platformWin "Qt5::QWinRTIntegrationPlugin")
	ELSE()
		FETCH_TARGET_LOCATION(platformWin "Qt5::QWindowsIntegrationPlugin")
	ENDIF()

	INSTALL(TARGETS AusweisApp DESTINATION . COMPONENT Application)
	INSTALL(FILES ${libSvg} DESTINATION . COMPONENT Runtime)
	INSTALL(FILES ${pluginSvg} DESTINATION imageformats COMPONENT Runtime)
	INSTALL(FILES ${pluginGif} DESTINATION imageformats COMPONENT Runtime)
	INSTALL(FILES ${pluginJpeg} DESTINATION imageformats COMPONENT Runtime)
	INSTALL(FILES ${platformWin} DESTINATION platforms COMPONENT Runtime)

	INSTALL(CODE
		"
		${SEARCH_ADDITIONAL_DIRS}
		INCLUDE(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}\" \"\" \"${TOOLCHAIN_BIN_PATH};\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)



ELSEIF(APPLE AND NOT IOS)
	SET(MACOS_BUNDLE_PLUGINS_DIR ../PlugIns)
	SET(MACOS_BUNDLE_FRAMEWORKS_DIR ../Frameworks)
	SET(MACOS_BUNDLE_RESOURCES_DIR ../Resources)

	# We need to include the following (i.e. all) image format plug-ins,
	# since those seem to be loaded upon program start-up. Not including
	# them would cause the respective add-on from a Qt installation (if
	# any) to be loaded, which would in turn cause the Qt libraries they
	# depend on to be loaded as well, thus resulting in two sets of Qt
	# libraries being loaded (ours from the bundle and the ones from the
	# installation) and the program misbehaving (crashing).
	FETCH_TARGET_LOCATION(platformMac "Qt5::QCocoaIntegrationPlugin")

	FOREACH (qtComponent QtCore Qt5Gui Qt5Network Qt5Svg Qt5Widgets)
		FOREACH(plugin ${${qtComponent}_PLUGINS})
			GET_TARGET_PROPERTY(pluginPath ${plugin} LOCATION)
			GET_FILENAME_COMPONENT(pluginDir ${pluginPath} DIRECTORY)
			GET_FILENAME_COMPONENT(pluginName ${pluginPath} NAME)
			GET_FILENAME_COMPONENT(pluginDirName ${pluginDir} NAME)
			INSTALL(FILES ${pluginPath} DESTINATION ${MACOS_BUNDLE_PLUGINS_DIR}/${pluginDirName} COMPONENT Runtime)
			LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/PlugIns/${pluginDirName}/${pluginName}")
		ENDFOREACH()
	ENDFOREACH()

	FOREACH(entry QtQuick QtQuick.2 QtQml QtGraphicalEffects Qt)
		SET(_dir "${QT_HOST_PREFIX}/qml")
		FILE(GLOB_RECURSE DYLIB "${_dir}/${entry}/*.dylib")
		FOREACH(_lib ${DYLIB})
			FILE(RELATIVE_PATH _lib_dest "${_dir}" "${_lib}")
			IF(NOT _lib_dest MATCHES "XmlListModel|Particles.2|LocalStorage") # blacklist not needed stuff
				GET_FILENAME_COMPONENT(_lib_dest_dir ${_lib_dest} DIRECTORY)
				INSTALL(FILES ${_lib} DESTINATION ${MACOS_BUNDLE_RESOURCES_DIR}/qml_stationary/${_lib_dest_dir} COMPONENT Runtime)
				LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Resources/qml_stationary/${_lib_dest}")
			ENDIF()
		ENDFOREACH()
	ENDFOREACH()

	INSTALL(TARGETS AusweisApp DESTINATION . COMPONENT Application)
	INSTALL(FILES ${platformMac} DESTINATION ${MACOS_BUNDLE_PLUGINS_DIR}/platforms COMPONENT Runtime)

	INSTALL(CODE
		"
		${SEARCH_ADDITIONAL_DIRS}
		file(GLOB_RECURSE QTPLUGINS \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_PLUGINS_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		file(GLOB_RECURSE QtQuickPLUGINS \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_RESOURCES_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		INCLUDE(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${EXECUTABLE_NAME}\" \"\${QTPLUGINS};\${QtQuickPLUGINS}\" \"${TOOLCHAIN_LIB_PATH};\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)

	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtCore.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtGui.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtXml.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtNetwork.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtSvg.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtWidgets.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtPrintSupport.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQml.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQuick.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQuickControls2.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQuickTemplates2.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQuickWidgets.framework")

	FOREACH (OPENSSL_LIBRARY ${OPENSSL_LIBRARIES})
		GET_FILENAME_COMPONENT(OPENSSL_LIBRARY_REAL ${OPENSSL_LIBRARY} REALPATH)
		GET_FILENAME_COMPONENT(OPENSSL_LIBRARY_NAME ${OPENSSL_LIBRARY_REAL} NAME)
		LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/MacOS/${OPENSSL_LIBRARY_NAME}")
	ENDFOREACH()

	# set it to parent scope to be able to access it from Packaging.cmake
	SET(ADDITIONAL_BUNDLE_FILES_TO_SIGN ${ADDITIONAL_BUNDLE_FILES_TO_SIGN} PARENT_SCOPE)

ELSEIF(IOS)
	LIST(APPEND CMAKE_MODULE_PATH "${PACKAGING_DIR}/ios")



ELSEIF(ANDROID)
	SET(ANDROID_PACKAGE_SRC_DIR ${PROJECT_BINARY_DIR}/package-src-dir)
	SET(ANDROID_DEST libs/${CMAKE_ANDROID_ARCH_ABI})
	SET(PERMISSIONS PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	INSTALL(TARGETS AusweisApp DESTINATION ${ANDROID_DEST} ${PERMISSIONS} COMPONENT Application)

	SET(RESOURCES_IMG_ANDROID_DIR ${RESOURCES_DIR}/images/android)
	IF(IS_DEVELOPER_VERSION)
		SET(ANDROID_LAUNCHER_ICON "npa_beta.png")
		SET(ANDROID_PACKAGE_NAME "com.governikus.ausweisapp2.dev")
	ELSE()
		SET(ANDROID_LAUNCHER_ICON "npa.png")
		SET(ANDROID_PACKAGE_NAME "com.governikus.ausweisapp2")
	ENDIF()

	FOREACH(entry ldpi mdpi hdpi xhdpi xxhdpi xxxhdpi)
		INSTALL(FILES ${RESOURCES_IMG_ANDROID_DIR}/${entry}/${ANDROID_LAUNCHER_ICON} DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/res/drawable-${entry} COMPONENT Runtime RENAME npa.png)
	ENDFOREACH()

	INSTALL(FILES ${PACKAGING_DIR}/android/nfc_tech_filter.xml DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/res/xml COMPONENT Runtime)
	INSTALL(FILES ${PACKAGING_DIR}/android/styles.xml DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/res/values COMPONENT Runtime)

	FILE(GLOB_RECURSE JAVA_FILES "${SRC_DIR}/*.java")
	INSTALL(FILES ${JAVA_FILES} DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/src COMPONENT Runtime)
	INSTALL(FILES ${PACKAGING_DIR}/android/IAusweisApp2Sdk.aidl DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/src/com/governikus/ausweisapp2/ COMPONENT Runtime)
	INSTALL(FILES ${PACKAGING_DIR}/android/IAusweisApp2SdkCallback.aidl DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/src/com/governikus/ausweisapp2/ COMPONENT Runtime)

	IF(VERSION_DVCS)
		SET(ANDROID_VERSION_NAME ${VERSION_DVCS})
	ELSE()
		SET(ANDROID_VERSION_NAME ${PROJECT_VERSION})
	ENDIF()
	CONFIGURE_FILE(${PACKAGING_DIR}/android/AndroidManifest.xml.in ${ANDROID_PACKAGE_SRC_DIR}/AndroidManifest.xml @ONLY)

	GET_FILENAME_COMPONENT(ANDROID_TOOLCHAIN_MACHINE_NAME "${CMAKE_CXX_ANDROID_TOOLCHAIN_PREFIX}" NAME)
	STRING(REGEX REPLACE "-$" "" ANDROID_TOOLCHAIN_MACHINE_NAME "${ANDROID_TOOLCHAIN_MACHINE_NAME}")
	STRING(REGEX MATCH "/toolchains/(.*)/prebuilt/" _unused "${CMAKE_CXX_ANDROID_TOOLCHAIN_PREFIX}")
	STRING(REGEX REPLACE "-${CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION}$" "" ANDROID_TOOLCHAIN_PREFIX "${CMAKE_MATCH_1}")

	SET(ANDROID_DEPLOYMENT_SETTINGS ${PROJECT_BINARY_DIR}/libAusweisApp2.so-deployment-settings.json CACHE INTERNAL "apk deployment" FORCE)
	SET(ANDROID_APP_BINARY "${CMAKE_INSTALL_PREFIX}/${ANDROID_DEST}/libAusweisApp2.so")
	CONFIGURE_FILE(${PACKAGING_DIR}/android/libAusweisApp2.so-deployment-settings.json.in ${ANDROID_DEPLOYMENT_SETTINGS} @ONLY)

	SET(TRANSLATION_DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/assets/translations)
	SET(DEFAULT_FILE_DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/assets)

ELSEIF(UNIX)
	IF(BUILD_SHARED_LIBS)
		SET(CMAKE_INSTALL_RPATH "\$ORIGIN")
	ENDIF()

	SET(DEFAULT_FILE_DESTINATION bin)
	INSTALL(TARGETS AusweisApp DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Application)
	INSTALL(CODE
		"
		${SEARCH_ADDITIONAL_DIRS}
		INCLUDE(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}/${EXECUTABLE_NAME}\" \"\" \"\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)

	CONFIGURE_FILE(${PACKAGING_DIR}/linux/AusweisApp2.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/AusweisApp2.desktop @ONLY)
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/AusweisApp2.desktop DESTINATION share/applications COMPONENT Application)
	#INSTALL(FILES ${RESOURCES_DIR}/images/AusweisApp2.svg DESTINATION share/icons/hicolor/scalable/apps COMPONENT Application)
ENDIF()




IF(LINUX OR WIN32 OR MAC)
	OPTION(SELFPACKER "Compress executable with self packer like UPX")
	IF(SELFPACKER)
		FIND_PACKAGE(SelfPackers)
		IF(SELF_PACKER_FOR_EXECUTABLE)
			MESSAGE(STATUS "Using SelfPacker: ${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS}")
		ELSE()
			MESSAGE(FATAL_ERROR "Cannot find self packer")
		ENDIF()

		INSTALL(CODE
			"
			EXECUTE_PROCESS(COMMAND
				${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}/${EXECUTABLE_NAME}\")
			" COMPONENT Application)
	ENDIF()
ENDIF()

IF(WIN32)
	IF(SIGNTOOL_CMD)
		CONFIGURE_FILE(${CMAKE_MODULE_PATH}/SignFiles.cmake.in ${CMAKE_BINARY_DIR}/SignFiles.cmake @ONLY)
		INSTALL(CODE
			"
			EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" -DSIGN_EXT=*.exe -P \"${CMAKE_BINARY_DIR}/SignFiles.cmake\" WORKING_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}\")
			" COMPONENT Application)
	ENDIF()
ENDIF()



IF(LINUX)
	INSTALL(FILES ${QM_FILES} DESTINATION ${TRANSLATION_DESTINATION} COMPONENT Translations)
ELSE()
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/translations/ DESTINATION ${TRANSLATION_DESTINATION} COMPONENT Translations)
ENDIF()

# resources file
INSTALL(FILES ${RCC} DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)

# secure storage file
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.json DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)

# qtlogging.ini
INSTALL(FILES ${RESOURCES_DIR}/qtlogging.ini DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)

# qml directories
IF(DESKTOP)
	INSTALL(DIRECTORY ${RESOURCES_DIR}/qml_stationary DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
	FOREACH(entry QtQuick QtQuick.2 QtQml QtGraphicalEffects Qt)
		INSTALL(DIRECTORY ${QT_HOST_PREFIX}/qml/${entry} DESTINATION ${DEFAULT_FILE_DESTINATION}/qml_stationary COMPONENT Runtime PATTERN "*.dylib" EXCLUDE)
	ENDFOREACH()
ELSE()
	INSTALL(DIRECTORY ${RESOURCES_DIR}/qml DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
ENDIF()

# default-providers.json
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-providers.json DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)

# default-supported-devices.json
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/default-supported-devices.json DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
