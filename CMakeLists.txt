# This version might be wrong
cmake_minimum_required(VERSION 2.8)
project(TetriTake)

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Set version information in config.h
set(TETRITAKE_VERSION_MAJOR 0)
set(TETRITAKE_VERSION_MINOR 0)

# TODO: move this to subdirectory
configure_file(
    "${PROJECT_SOURCE_DIR}/include/config.h.in"
    "${PROJECT_BINARY_DIR}/include/config.h"
    )
include_directories("${PROJECT_BINARY_DIR}/include")



# -----------------------------------------
#  Setup the main executable
# -----------------------------------------

add_subdirectory(source)
if(APPLE)
	add_subdirectory(osx)
endif()

# Setup the executable
set(EXECUTABLE_NAME "TetriTake")
add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${PROJECT_SOURCES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

# Set the project include directories
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics audio)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

# Detect and add Thor
find_package(Thor 2 REQUIRED)
if(THOR_FOUND)
    include_directories(${THOR_INCLUDE_DIR})
    target_link_libraries(${EXECUTABLE_NAME} ${THOR_LIBRARY})
endif()

if(APPLE)
	set(MAKE_INSTALL_NAME_DIR @executable_path)
	
	# Add Apple libraries
	FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(${EXECUTABLE_NAME} ${FOUNDATION_LIBRARY})
	
	# Add Info.plist to bundle
	set_target_properties(${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/osx/Info.plist.in)

	# Copy SFML frameworks and libthor into bundle
	set(BUNDLE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}.app)
	set(TARGET_FRAMEWORKS_DIR ${BUNDLE_PATH}/Contents/Frameworks)
	add_custom_command(TARGET ${EXECUTABLE_NAME}
		POST_BUILD
		COMMAND rm -Rf ${TARGET_FRAMEWORKS_DIR} && mkdir -p ${TARGET_FRAMEWORKS_DIR} && cp -Rn ${SFML_LIBRARIES} ${THOR_LIBRARY} ${TARGET_FRAMEWORKS_DIR}
		)
	
	# Attempt at using BundleUtilities, doesn't work
	#set(EXTERNAL_LIBRARIES ${THOR_LIBRARY} ${SFML_LIBRARIES})
	#install(CODE "include(BundleUtilities)
	#	fixup_bundle(\"${BUNDLE_PATH}/Contents/MacOS/${EXECUTABLE_NAME}\" \"\" \"${EXTERNAL_LIBRARIES};/System/Frameworks\")"
	#	COMPONENT Runtime)

	# Copy the resources into the bundle
	set(TARGET_RESOURCE_DIR ${BUNDLE_PATH}/Contents/Resources)
	set(RESOURCE_DIR ${PROJECT_SOURCE_DIR}/resources)
	add_custom_command(TARGET ${EXECUTABLE_NAME}
    	POST_BUILD
    	COMMAND mkdir -p ${TARGET_RESOURCE_DIR} && tar -c -C "${RESOURCE_DIR}" --exclude .DS_Store --exclude CVS --exclude .svn --exclude .git -H . | tar -x -C ${TARGET_RESOURCE_DIR}
		)
endif()
