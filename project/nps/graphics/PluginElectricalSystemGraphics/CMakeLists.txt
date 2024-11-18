project(PluginElectricalSystemGraphics)


file(GLOB electrical_system_header_files ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB electrical_system_cpp_files ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB electrical_system_ui_files ${CMAKE_CURRENT_LIST_DIR}/*.ui)


add_library(PluginElectricalSystemGraphics ${LIB_TYPE} 
    ${electrical_system_header_files}
    ${electrical_system_cpp_files}
    ${electrical_system_ui_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")


target_link_libraries(
    PluginElectricalSystemGraphics
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
    PluginElectricalSystemGraphics
    PRIVATE 
    UNICODE
    WIN32
    QT_DLL
    QT_CORE_LIB
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})