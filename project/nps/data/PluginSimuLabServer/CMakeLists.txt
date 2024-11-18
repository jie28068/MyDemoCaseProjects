project(PluginSimuLabServer)

file(GLOB simulab_server_header_files
    ${CMAKE_CURRENT_LIST_DIR}/*.h
)
    
file(GLOB simulab_server_cpp_files
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

add_library(PluginSimuLabServer ${LIB_TYPE} 
    ${simulab_server_header_files}
    ${simulab_server_cpp_files}
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_include_directories(
    PluginSimuLabServer
    PRIVATE
    ${SIMUNPS_SERVER_INCLUDE}/SimulationCore
)

target_link_libraries(
    PluginSimuLabServer
    PRIVATE
    KLModelDefinitionCore
    GraphicsModelingKernel
    KLProject
    Qt5::Gui
    Qt5::Core
    Qt5::Widgets
    QP::CoreLib
    QP::CoreUi
    QP::KLWidgets
    QP::ToolsLib
)

target_compile_definitions(
    PluginSimuLabServer
    PRIVATE 
    UNICODE
    WIN32
)

#copy_directory_files(PluginSimuLabServer "${CMAKE_CURRENT_LIST_DIR}" "${CMAKE_SOURCE_DIR}/common/include/ProjectManagerServer")

TARGET_AS_PLUGIN(PluginSimuLabServer)