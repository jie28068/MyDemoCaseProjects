project(PluginGraphicsModelingServer)

file(GLOB graphics_modeling_server_header_files
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/Manager/*.h
    ${CMAKE_CURRENT_LIST_DIR}/Data/*.h
    ${CMAKE_CURRENT_LIST_DIR}/Business/*.h
    ${CMAKE_CURRENT_LIST_DIR}/BlockDelegate/*.h
    ${CMAKE_CURRENT_LIST_DIR}/UI/*.h
    ${CMAKE_CURRENT_LIST_DIR}/KernnelInterface/*.h
    ${CMAKE_CURRENT_LIST_DIR}/OtherInterface/*.h
)
file(GLOB graphics_modeling_server_cpp_files
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Manager/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Data/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Business/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BlockDelegate/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/UI/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/KernnelInterface/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/OtherInterface/*.cpp
)
file(GLOB graphics_modeling_server_ui_files
    ${CMAKE_CURRENT_LIST_DIR}/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/UI/*.ui)

add_library(PluginGraphicsModelingServer ${LIB_TYPE}
    ${graphics_modeling_server_header_files}
    ${graphics_modeling_server_cpp_files}
    ${graphics_modeling_server_ui_files}
    rc.qrc
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_include_directories(
    PluginGraphicsModelingServer
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/UI
)

target_link_libraries(
    PluginGraphicsModelingServer
    PRIVATE
    Qt5::Gui
    Qt5::Core
    Qt5::Svg

    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib

    GraphicsModelingKernel
    KLModelDefinitionCore
    KLProject
)

target_compile_definitions(
    PluginGraphicsModelingServer
    PRIVATE
    UNICODE
    WIN32
    PLUGINGRAPHICSMODELINGSERVER_LIB
)

# 拷贝文件到include目录
add_custom_command(TARGET PluginGraphicsModelingServer PRE_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/GraphicsModelingConst.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/IPluginGraphicModelingServer.h"
    "${SIMUNPS_SERVER_INCLUDE}/PluginGraphicsModelingServer"
    VERBATIM
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})