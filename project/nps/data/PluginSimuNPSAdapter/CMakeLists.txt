project(PluginSimuNPSAdapter)

file(GLOB simunps_adapter_header_files ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB simunps_adapter_cpp_files ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB simunps_adapter_ui_files ${CMAKE_CURRENT_LIST_DIR}/*.ui)


add_library(PluginSimuNPSAdapter ${LIB_TYPE} 
    ${simunps_adapter_header_files}
    ${simunps_adapter_cpp_files}
    ${simunps_adapter_ui_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")


target_link_directories(
    PluginSimuNPSAdapter
    PRIVATE
)

target_link_libraries(
    PluginSimuNPSAdapter
    PRIVATE 
    Qt5::Core
    Qt5::Xml
    Qt5::Gui
    Qt5::Widgets
    QP::CoreLib
    QP::ToolsLib
    QP::CoreDataDictionary
)

target_compile_definitions(
    PluginSimuNPSAdapter
    PRIVATE 
    UNICODE
    WIN32
    QT_DLL
    QT_CORE_LIB
    QT_XML_LIB
    QT_GUI_LIB
    PLUGINSIMUNPSADAPTER_LIB
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})