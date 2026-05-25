cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax 3.13
                                     # included policy CMP0077

project(mbclient VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(MBTOOLS_CLIENT_APP_NAME ${PROJECT_NAME} CACHE INTERNAL "Name of the ModbusTools client application")

message(STATUS "MBTOOLS: Start configure client '${MBTOOLS_CLIENT_APP_NAME}'")

# QT supporting turn ON
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

set(MBTOOLS_HEADERS
    core/client_global.h
    core/client.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_builder.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_device.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dom.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_port.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_project.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dataview.h
    gui/dialogs/settings/client_dialogsettings.h
    gui/dialogs/client_dialogdevice.h
    gui/dialogs/client_dialogport.h
    gui/dialogs/client_dialogdataviewitem.h
    gui/dialogs/client_dialogs.h
    gui/project/client_projectdelegate.h
    gui/project/client_projectmodel.h
    gui/project/client_projectui.h
    gui/dataview/client_dataviewdelegate.h
    gui/dataview/client_dataviewmanager.h
    gui/dataview/client_dataviewmodel.h
    gui/dataview/client_dataviewui.h
    gui/statistics/client_portstatisticsui.h
    gui/statistics/client_devicestatisticsui.h
    gui/statistics/client_statisticsmanager.h
    gui/sendmessage/client_sendmessagewidget.h
    gui/sendmessage/client_sendmessagedefaultwidget.h
    gui/sendmessage/client_sendmessagewritesinglecoilwidget.h
    gui/sendmessage/client_sendmessagewritesingleregisterwidget.h
    gui/sendmessage/client_sendmessagereadexceptionstatuswidget.h
    gui/sendmessage/client_sendmessagediagnosticswidget.h
    gui/sendmessage/client_sendmessagepggetcommeventcounterwidget.h
    gui/sendmessage/client_sendmessagepggetcommeventlogwidget.h
    gui/sendmessage/client_sendmessagereportserveridwidget.h
    gui/sendmessage/client_sendmessagefilerecordswidget.h
    gui/sendmessage/client_sendmessagemaskwriteregisterwidget.h
    gui/sendmessage/client_sendmessagereadfifoqueuewidget.h
    gui/sendmessage/client_sendmessagereadwritemultipleregisterswidget.h
    gui/sendmessage/client_sendmessagereaddeviceidwidget.h
    gui/sendmessage/client_sendmessagelistmodel.h
    gui/sendmessage/client_sendmessageui.h
    gui/sendbytes/client_sendbyteslistmodel.h
    gui/sendbytes/client_sendbytesui.h
    gui/scanner/client_dialogscannerrequest.h
    gui/scanner/client_dialogscannerhost.h
    gui/scanner/client_dialogscannerport.h
    gui/scanner/client_scanner.h
    gui/scanner/client_scannerunitmodel.h
    gui/scanner/client_scannerfuncmodel.h
    gui/scanner/client_scannerthread.h
    gui/scanner/client_scannerui.h
    gui/client_windowmanager.h
    gui/client_ui.h
    runtime/client_devicerunnable.h
    runtime/client_portrunnable.h
    runtime/client_rundevice.h
    runtime/client_runitem.h
    runtime/client_runmessage.h
    runtime/client_runport.h
    runtime/client_runthread.h
    runtime/client_runtime.h)

set(MBTOOLS_SOURCES
    core/client_global.cpp
    core/client.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_builder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_device.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dom.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_port.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_project.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dataview.cpp
    gui/dialogs/settings/client_dialogsettings.cpp
    gui/dialogs/client_dialogdevice.cpp
    gui/dialogs/client_dialogport.cpp
    gui/dialogs/client_dialogdataviewitem.cpp
    gui/dialogs/client_dialogs.cpp
    gui/project/client_projectdelegate.cpp
    gui/project/client_projectmodel.cpp
    gui/project/client_projectui.cpp
    gui/dataview/client_dataviewdelegate.cpp
    gui/dataview/client_dataviewmanager.cpp
    gui/dataview/client_dataviewmodel.cpp
    gui/dataview/client_dataviewui.cpp
    gui/statistics/client_portstatisticsui.cpp
    gui/statistics/client_devicestatisticsui.cpp
    gui/statistics/client_statisticsmanager.cpp
    gui/sendmessage/client_sendmessagewidget.cpp
    gui/sendmessage/client_sendmessagedefaultwidget.cpp
    gui/sendmessage/client_sendmessagewritesinglecoilwidget.cpp
    gui/sendmessage/client_sendmessagewritesingleregisterwidget.cpp
    gui/sendmessage/client_sendmessagereadexceptionstatuswidget.cpp
    gui/sendmessage/client_sendmessagediagnosticswidget.cpp
    gui/sendmessage/client_sendmessagepggetcommeventcounterwidget.cpp
    gui/sendmessage/client_sendmessagepggetcommeventlogwidget.cpp
    gui/sendmessage/client_sendmessagereportserveridwidget.cpp
    gui/sendmessage/client_sendmessagefilerecordswidget.cpp
    gui/sendmessage/client_sendmessagemaskwriteregisterwidget.cpp
    gui/sendmessage/client_sendmessagereadwritemultipleregisterswidget.cpp
    gui/sendmessage/client_sendmessagereadfifoqueuewidget.cpp
    gui/sendmessage/client_sendmessagereaddeviceidwidget.cpp
    gui/sendmessage/client_sendmessagelistmodel.cpp
    gui/sendmessage/client_sendmessageui.cpp
    gui/sendbytes/client_sendbyteslistmodel.cpp
    gui/sendbytes/client_sendbytesui.cpp
    gui/scanner/client_dialogscannerrequest.cpp
    gui/scanner/client_dialogscannerhost.cpp
    gui/scanner/client_dialogscannerport.cpp
    gui/scanner/client_scanner.cpp
    gui/scanner/client_scannerunitmodel.cpp
    gui/scanner/client_scannerfuncmodel.cpp
    gui/scanner/client_scannerthread.cpp
    gui/scanner/client_scannerui.cpp
    gui/client_windowmanager.cpp
    gui/client_ui.cpp
    runtime/client_devicerunnable.cpp
    runtime/client_portrunnable.cpp
    runtime/client_rundevice.cpp
    runtime/client_runitem.cpp
    runtime/client_runmessage.cpp
    runtime/client_runport.cpp
    runtime/client_runthread.cpp
    runtime/client_runtime.cpp
    main.cpp)

set(RESOURCES gui/client_rsc.qrc)

if(WIN32)
  set(MBTOOLS_WIN_RESOURCE_FILE win_resource.rc)
  message(STATUS "${PROJECT_NAME} resource file for Windows: '${MBTOOLS_WIN_RESOURCE_FILE}'")

  set(MBTOOLS_WIN_ICON
      "IDI_ICON1\tICON\tDISCARDABLE\t\"gui\\\\icons\\\\${PROJECT_NAME}.ico\"")
  set(MBTOOLS_WIN_FILE "${PROJECT_NAME}")
  set(MBTOOLS_WIN_OUTFILE "${MBTOOLS_WIN_FILE}.exe")
  configure_file(${CMAKE_CURRENT_LIST_DIR}/../${MBTOOLS_WIN_RESOURCE_FILE}.in
                 ${CMAKE_CURRENT_LIST_DIR}/${MBTOOLS_WIN_RESOURCE_FILE})

  set(RESOURCES ${RESOURCES}
                ${CMAKE_CURRENT_LIST_DIR}/${MBTOOLS_WIN_RESOURCE_FILE})
endif()

add_executable(${MBTOOLS_CLIENT_APP_NAME} ${MBTOOLS_HEADERS} ${MBTOOLS_SOURCES} ${RESOURCES})

include_directories(
  .
  ..
  ../../modbus/src
  ../core/sdk
  ../core/core
  ../core
  core)

target_compile_definitions(${MBTOOLS_CLIENT_APP_NAME} PRIVATE QT_NO_KEYWORDS)

set_target_properties(
  ${MBTOOLS_CLIENT_APP_NAME}
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}
             WIN32_EXECUTABLE true)

target_link_libraries(${MBTOOLS_CLIENT_APP_NAME} PRIVATE
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets
                      ${MB_LIBRARY_NAME}
                      ${MBTOOLS_CORE_LIB_NAME})
