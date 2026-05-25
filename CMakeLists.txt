cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

project(ModbusTools VERSION 0.5.1 LANGUAGES CXX)

set(MBTOOLS_VERSION ${PROJECT_VERSION})
set(MBTOOLS_ROOT_DIR ${CMAKE_SOURCE_DIR})

option(MBTOOLS_CLIENT_ENABLED "Enable client application build" ON)
option(MBTOOLS_SERVER_ENABLED "Enable server application build" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release")

if (NOT WIN32)
    #set(CMAKE_SKIP_BUILD_RPATH  FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
    #set(CMAKE_INSTALL_RPATH "\${ORIGIN}")
endif()

set(MB_QT_ENABLED ON)
set(MB_C_SUPPORT_DISABLE ON)
set(MB_NO_INSTALL ON)
# EXCLUDE_FROM_ALL: to not generate install data
add_subdirectory(modbus/src EXCLUDE_FROM_ALL)

add_subdirectory(src/core)

if (MBTOOLS_CLIENT_ENABLED)
    add_subdirectory(src/client)
endif()

if (MBTOOLS_SERVER_ENABLED)
    add_subdirectory(src/server)
endif()

add_subdirectory("doc")

install(TARGETS ${MB_LIBRARY_NAME}         DESTINATION .)
install(TARGETS ${MBTOOLS_CORE_LIB_NAME}   DESTINATION .)

if (MBTOOLS_CLIENT_ENABLED)
    install(TARGETS ${MBTOOLS_CLIENT_APP_NAME} DESTINATION .)
endif()

if (MBTOOLS_SERVER_ENABLED)
    install(TARGETS ${MBTOOLS_SERVER_APP_NAME} DESTINATION .)
    # Install scripts
    # Collect Python files using GLOB
    file(GLOB MBTOOLS_SCRIPT_FILES "${CMAKE_SOURCE_DIR}/src/server/python/*.py")
    # Install Python files
    install(FILES ${MBTOOLS_SCRIPT_FILES} DESTINATION "./script/server")
endif()

# Install help files
# Collect help files using GLOB
file(GLOB MBTOOLS_HELP_FILES "${CMAKE_BINARY_DIR}/doc/output/*.qch" "${CMAKE_BINARY_DIR}/doc/output/*.qhc")
install(FILES ${MBTOOLS_HELP_FILES} DESTINATION "./help")
