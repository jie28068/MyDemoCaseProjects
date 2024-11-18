project(PluginMonitorPanelManagerForSimuNPS)

file(GLOB monitorpanelmanager_view_header_files
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/*.h
)

file(GLOB monitorpanelmanager_view_cpp_files
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/*.cpp
)

file(GLOB monitorpanelmanager_view_ui_files
    ${CMAKE_CURRENT_LIST_DIR}/*.ui
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/*.ui
)

set(monitorpanelmanager_view_qrc_files PluginMonitorPanelManagerView.qrc)

# 分组
set(PlotParts "PlotParts")
set(PlotPartSrc ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlot.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlot.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotCurve.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotCurve.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotCanvas.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotCanvas.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotLegend.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotLegend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotPicker.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ItemPlotPicker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotEllipse.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotEllipse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotToolbar.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotToolbar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotWidget.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotWidget.cpp)

source_group(${PlotParts} FILES ${PlotPartSrc})

set(ModelAndView "ModelAndView")
set(ModelAndViewSrc ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ColorButton.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ColorButton.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTableModel.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTableModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTableView.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTableView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTreeModel.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTreeModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTreeView.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CurveSettingTreeView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DataAnalyseTableView.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DataAnalyseTableView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DataAnalyseTreeView.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DataAnalyseTreeView.cpp)
source_group(${ModelAndView} FILES ${ModelAndViewSrc})

set(MonitorPanel "MonitorPanel")
set(MonitorPanelSrc ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MonitorPanel.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MonitorPanel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MonitorPanelPlotServer.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MonitorPanelPlotServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorWidget.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotLayoutChooseDialog.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotLayoutChooseDialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotScrollArea.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/PlotScrollArea.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CustomPlotSettingsDialog.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/CustomPlotSettingsDialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DataDictionaryDialog.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DataDictionaryDialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MarkerObserver.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MarkerObserver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MarkerSyncOperator.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/MarkerSyncOperator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/SyncOperator.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/SyncOperator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ScopeViewToolbar.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/ScopeViewToolbar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/def.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DragDataResolver.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel/DragDataResolver.cpp)
source_group(${MonitorPanel} FILES ${MonitorPanelSrc})

set(Server "Server")
set(ServerSrc ${CMAKE_CURRENT_LIST_DIR}/MonitorPanelPlotServer.h
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanelPlotServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pluginmonitorpanelmanagerview.h
    ${CMAKE_CURRENT_LIST_DIR}/pluginmonitorpanelmanagerview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pluginmonitorpanelmanagerview_global.h)
source_group(${Server} FILES ${ServerSrc})

add_library(${PROJECT_NAME} ${LIB_TYPE}
    ${monitorpanelmanager_view_header_files}
    ${monitorpanelmanager_view_cpp_files}
    ${monitorpanelmanager_view_ui_files}
    ${monitorpanelmanager_view_qrc_files}
)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/MonitorPanel
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Qt5::Gui
    Qt5::Core
    Qt5::Concurrent
    Qt5::Xml
    Qt5::Widgets
    QP::KLWidgets
    QP::CoreLib
    QP::CoreUi
    QP::ToolsLib
    ${qwt_target}
)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
    UNICODE
    WIN32
    QWT_DLL
    PLUGINMONITORPANELMANAGERVIEW_LIB
)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/Zc:wchar_t-") # no

# 指定输出目录和安装内容
TARGET_AS_PLUGIN(${PROJECT_NAME})