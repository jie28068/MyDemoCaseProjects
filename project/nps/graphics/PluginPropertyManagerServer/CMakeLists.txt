project(PluginPropertyManagerServer)

# find_package(QtPropertyBrowser)
# if(NOT QtPropertyBrowser_FOUND)
# message(FATAL_ERROR "没有找到QtPropertyBrowser库")
# endif()
file(GLOB plugin_property_manager_server_header_files
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/assistant/*.h
    ${CMAKE_CURRENT_LIST_DIR}/models/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/commentBlock/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/controlBlock/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/electricalBlock/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/drawboard/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/common/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/slotBlock/*.h
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/*.h
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/blocks/*.h
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/comments/*.h
)

file(GLOB plugin_property_manager_server_cpp_files
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/assistant/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/models/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/commentBlock/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/controlBlock/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/electricalBlock/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/drawboard/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/common/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/slotBlock/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/blocks/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/comments/*.cpp
)

file(GLOB plugin_property_manager_server_ui_files
    ${CMAKE_CURRENT_LIST_DIR}/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/assistant/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/models/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/commentBlock/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/controlBlock/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/electricalBlock/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/drawboard/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/common/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/slotBlock/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/blocks/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/comments/*.ui
)

add_library(PluginPropertyManagerServer ${LIB_TYPE}
    ${plugin_property_manager_server_header_files}
    ${plugin_property_manager_server_cpp_files}
    ${plugin_property_manager_server_ui_files}
    PluginPropertyManagerServer.qrc
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_include_directories(
    PluginPropertyManagerServer
    PRIVATE
    ${SIMUNPS_INCLUDE_DIR}/KLModelDefinitionCore
    ${CMAKE_CURRENT_LIST_DIR}/UI
    ${CMAKE_CURRENT_LIST_DIR}/assistant
    ${CMAKE_CURRENT_LIST_DIR}/models
    ${CMAKE_CURRENT_LIST_DIR}/views
    ${CMAKE_CURRENT_LIST_DIR}/views/commentBlock
    ${CMAKE_CURRENT_LIST_DIR}/views/controlBlock
    ${CMAKE_CURRENT_LIST_DIR}/views/electricalBlock
    ${CMAKE_CURRENT_LIST_DIR}/views/drawboard
    ${CMAKE_CURRENT_LIST_DIR}/views/common
    ${CMAKE_CURRENT_LIST_DIR}/views/slotBlock
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/blocks
    ${CMAKE_CURRENT_LIST_DIR}/PropertyWrappers/comments
)

target_link_libraries(
    PluginPropertyManagerServer
    PRIVATE
    Qt5::Gui
    Qt5::Core
    Qt5::Xml
    Qt5::Svg
    Qt5::Widgets

    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib

    ${qtpropertybrowser_target}
    GraphicsModelingKernel
    KLModelDefinitionCore
    KLProject
)

target_compile_definitions(
    PluginPropertyManagerServer
    PRIVATE
    UNICODE
    WIN32
    QT_DLL
    QT_CORE_LIB
    PLUGINPROPERTYMANAGERSERVER_LIB
    QT_GUI_LIB
)

copy_updated_files(${PROJECT_NAME} PRE_BUILD ${CMAKE_CURRENT_LIST_DIR}/IPropertyManagerServer.h ${SIMUNPS_SERVER_INCLUDE}/PropertyManagerServer/)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})

add_dependencies(PluginPropertyManagerServer GraphicsModelingKernel ${qtpropertybrowser_target})