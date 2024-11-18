project(PluginStateMachineGraphics)


file(GLOB state_machine_system_header_files ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB state_machine_system_cpp_files ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB state_machine_system_ui_files ${CMAKE_CURRENT_LIST_DIR}/*.ui)


add_library(PluginStateMachineGraphics ${LIB_TYPE} 
    ${state_machine_system_header_files}
    ${state_machine_system_cpp_files}
    ${state_machine_system_ui_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

    
target_link_libraries(
    PluginStateMachineGraphics
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
    PluginStateMachineGraphics
    PRIVATE 
    UNICODE
    WIN32
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})