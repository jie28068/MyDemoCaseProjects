project(PluginProjectManagerServer)

file(GLOB projectmanager_server_header_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB projectmanager_server_cpp_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB projectmanager_server_ui_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.ui)

add_library(PluginProjectManagerServer ${LIB_TYPE} 
    ${projectmanager_server_header_files}
    ${projectmanager_server_cpp_files}
    ${projectmanager_server_ui_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")


target_link_libraries(
    PluginProjectManagerServer
    PRIVATE 
    KLModelDefinitionCore
    KLProject
    Qt5::Gui
    Qt5::Core
    Qt5::Widgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib
    QP::KLWidgets
    ${quazip_target}
)

target_compile_definitions(
    PluginProjectManagerServer
    PRIVATE 
    UNICODE
    WIN32
    QT_DLL
    QT_CORE_LIB
    PLUGINPROJECTMANAGERSERVER_LIB
)

#copy_directory_files(PluginProjectManagerServer "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_SOURCE_DIR}/common/include/ProjectManagerServer")

TARGET_AS_PLUGIN(PluginProjectManagerServer)