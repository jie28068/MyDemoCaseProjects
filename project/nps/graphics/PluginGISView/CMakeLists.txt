project(PluginGISView)

set(KLPluginName PluginGISView)
create_kl_plugin()


file(GLOB gis_head_files
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)
file(GLOB gis_cpp_files
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)




add_library(PluginGISView ${LIB_TYPE}
    ${gis_head_files}
    ${gis_cpp_files}
)



target_link_libraries(
    PluginGISView
    PRIVATE
    Qt5::Gui
    Qt5::Core
    Qt5::Svg
    Qt5::WebEngineWidgets
    Qt5::WebChannel

    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib

    GraphicsModelingKernel
    KLModelDefinitionCore
    KLProject
)

target_compile_definitions(
    PluginGISView
    PRIVATE
    UNICODE
    WIN32
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})