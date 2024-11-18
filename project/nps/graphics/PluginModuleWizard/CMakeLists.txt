project(PluginModuleWizard)
file(GLOB Module_wizard_header_files
${CMAKE_CURRENT_LIST_DIR}/*.h
${CMAKE_CURRENT_LIST_DIR}/TableModel/*.h
${CMAKE_CURRENT_LIST_DIR}/TableDelegate/*.h
)
file(GLOB Module_wizard_cpp_files
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TableModel/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TableDelegate/*.cpp
)
file(GLOB Module_wizard_ui_files
	${CMAKE_CURRENT_LIST_DIR}/*.ui
)

add_library(PluginModuleWizard ${LIB_TYPE}
	${Module_wizard_header_files}
	${Module_wizard_cpp_files}
	${Module_wizard_ui_files}
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_link_libraries(
    PluginModuleWizard
    PRIVATE 
    Qt5::Gui
    Qt5::Core
    Qt5::Widgets
    Qt5::Svg
    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    KLModelDefinitionCore
    KLProject
)

TARGET_AS_PLUGIN(PluginModuleWizard)
