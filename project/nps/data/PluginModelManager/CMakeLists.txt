project(PluginModelManager)
file(GLOB PluginModelManager_header_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/view/*.h
    ${CMAKE_CURRENT_LIST_DIR}/model/*.h
    ${CMAKE_CURRENT_LIST_DIR}/mng/*.h
    ${CMAKE_CURRENT_LIST_DIR}/data/*.h
    ${CMAKE_CURRENT_LIST_DIR}/widget/*.h
    )
file(GLOB PluginModelManager_cpp_files 
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/model/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mng/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/data/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/widget/*.cpp
    )
file(GLOB PluginModelManager_ui_files 
    ${CMAKE_CURRENT_LIST_DIR}/widget/*.ui
    )
set(PluginModelManager_qrc_files PluginModelManager.qrc)

add_library(PluginModelManager ${LIB_TYPE} 
    ${PluginModelManager_header_files}
    ${PluginModelManager_cpp_files}
    ${PluginModelManager_ui_files}
    ${PluginModelManager_qrc_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_include_directories(
    PluginModelManager
    PRIVATE
    ${SIMUNPS_SERVER_INCLUDE}/ModelManagerServer
    ${CMAKE_CURRENT_LIST_DIR}/model
    ${CMAKE_CURRENT_LIST_DIR}/view
    ${CMAKE_CURRENT_LIST_DIR}/mng
    ${CMAKE_CURRENT_LIST_DIR}/data
    ${CMAKE_CURRENT_LIST_DIR}/widget
)

target_link_libraries(
    PluginModelManager
    PRIVATE 
    KLModelDefinitionCore
    Qt5::Core
    Qt5::Gui    
    Qt5::Widgets
    QP::CoreLib
    QP::KLWidgets
    KLProject
)

target_compile_definitions(
    PluginModelManager
    PRIVATE 
    UNICODE
    WIN32
)

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})