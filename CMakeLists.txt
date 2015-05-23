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
add_subdirectory(include)
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
	set(MAKE_INSTALL_NAME_DIR @rpath)
	
	# Add Apple libraries
	FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)
    target_link_libraries(${EXECUTABLE_NAME} ${FOUNDATION_LIBRARY})
	
	# Add Info.plist to bundle
	set_target_properties(${EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/osx/Info.plist.in)

	# Copy the assets and levels into the bundle
	set(BUNDLE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}.app)
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/resources/ DESTINATION ${BUNDLE_PATH}/Contents/Resources)

	# Make the bundle drag-n-drop
	get_filename_component(THOR_LIBRARY_DIR ${THOR_LIBRARY} DIRECTORY)
	get_filename_component(SFML_LIBRARY_DIR ${SFML_SYSTEM_LIBRARY} DIRECTORY)
	set(LIBRARIES_DIR ${THOR_LIBRARY_DIR};${SFML_LIBRARY_DIR})
	# Copy freetype manually, since it otherwise just causes problems (I think because it uses @executable_path instead of @rpath)
	install(DIRECTORY ${SFML_LIBRARY_DIR}/freetype.framework DESTINATION ${BUNDLE_PATH}/Contents/Frameworks)
	# Let BundleUtilities do everything else automaitcally
	install(CODE "
		include(BundleUtilities)
		fixup_bundle(\"${BUNDLE_PATH}\" \"\" \"${LIBRARIES_DIR}\")
		" COMPONENT Runtime)
endif()
