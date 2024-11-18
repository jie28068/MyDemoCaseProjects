project(PluginComponentView)

file(GLOB component_view_header_files ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB component_view_cpp_files ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB component_view_ui_files ${CMAKE_CURRENT_LIST_DIR}/*.ui)


set(component_view_qrc_files PluginComponentView.qrc)


add_library(PluginComponentView ${LIB_TYPE} 
    ${component_view_header_files}
    ${component_view_cpp_files}
    ${component_view_ui_files}
    ${component_view_qrc_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_link_libraries(
    PluginComponentView
    PRIVATE 
    KLModelDefinitionCore
    Qt5::Gui
    Qt5::Core
    Qt5::Svg
    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib
    ${qtpropertybrowser_target}
    KLProject
)

target_compile_definitions(
    PluginComponentView
    PRIVATE 
    UNICODE
    WIN32
    PLUGINCOMPONENTVIEW_LIB
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})