project(PluginProjectManagerView)

file(GLOB projectmanager_view_header_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/views/*.h
    ${CMAKE_CURRENT_LIST_DIR}/models/*.h
    ${CMAKE_CURRENT_LIST_DIR}/managers/*.h
    )
file(GLOB projectmanager_view_cpp_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/views/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/models/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/managers/*.cpp
    )
file(GLOB projectmanager_view_ui_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/views/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/models/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/managers/*.ui
    )


set(projectmanager_view_qrc_files PluginProjectManagerView.qrc)


add_library(PluginProjectManagerView ${LIB_TYPE} 
    ${projectmanager_view_header_files}
    ${projectmanager_view_cpp_files}
    ${projectmanager_view_ui_files}
    ${projectmanager_view_qrc_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_include_directories(
    PluginProjectManagerView
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/models
    ${CMAKE_CURRENT_LIST_DIR}/views
    ${CMAKE_CURRENT_LIST_DIR}/managers
    ${SIMUNPS_SERVER_INCLUDE}/ProjectManagerViewServer
)

target_link_libraries(
    PluginProjectManagerView
    PRIVATE 
    KLModelDefinitionCore
    Qt5::Gui
    Qt5::Core
    Qt5::Svg
    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib
    ${quazip_target}
    KLProject
)

target_compile_definitions(
    PluginProjectManagerView
    PRIVATE 
    UNICODE
    PLUGINPROJECTMANAGERVIEW_LIB    
)

TARGET_AS_PLUGIN(PluginProjectManagerView)