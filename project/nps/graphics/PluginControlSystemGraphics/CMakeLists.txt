project(PluginControlSystemGraphics)


file(GLOB control_system_header_files ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB control_system_cpp_files ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB control_system_ui_files ${CMAKE_CURRENT_LIST_DIR}/*.ui)


add_library(PluginControlSystemGraphics ${LIB_TYPE} 
    ${control_system_header_files}
    ${control_system_cpp_files}
    ${control_system_ui_files}
    controlsystem.qrc
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")


target_link_libraries(
    PluginControlSystemGraphics
    PRIVATE 
    Qt5::Gui
    Qt5::Core
    Qt5::Svg
    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib
    GraphicsModelingKernel
)

target_compile_definitions(
    PluginControlSystemGraphics
    PRIVATE 
    UNICODE
    WIN32
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})