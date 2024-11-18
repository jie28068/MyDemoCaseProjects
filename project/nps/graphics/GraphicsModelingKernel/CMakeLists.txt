project(GraphicsModelingKernel)

# 把protobuf的IDL文件转成c++代码
protobuf_to_cpp("${CMAKE_CURRENT_LIST_DIR}/proto" "${CMAKE_CURRENT_LIST_DIR}/Serialize")

set(graphics_sub_dirs 
	${CMAKE_CURRENT_LIST_DIR}/Algorithm
	${CMAKE_CURRENT_LIST_DIR}/CanvasDataStruct
	${CMAKE_CURRENT_LIST_DIR}/Commands
	${CMAKE_CURRENT_LIST_DIR}/InternalGraphicsFactory
	${CMAKE_CURRENT_LIST_DIR}/InternalGraphicsObject
	${CMAKE_CURRENT_LIST_DIR}/InternalGraphicsObject/Annotation
	${CMAKE_CURRENT_LIST_DIR}/InternalGraphicsObject/ConnectorWire
	${CMAKE_CURRENT_LIST_DIR}/InternalGraphicsObject/Control
	${CMAKE_CURRENT_LIST_DIR}/InternalGraphicsObject/Electrical
	${CMAKE_CURRENT_LIST_DIR}/InternalGraphicsObject/Text
	${CMAKE_CURRENT_LIST_DIR}/Manager
	${CMAKE_CURRENT_LIST_DIR}/Serialize
	${CMAKE_CURRENT_LIST_DIR}/UI
	${CMAKE_CURRENT_LIST_DIR}/PrivateHeader
	${CMAKE_CURRENT_LIST_DIR}/include
)

file(GLOB_RECURSE graphics_kernel_header_files ${CMAKE_CURRENT_LIST_DIR}/*.h )

file(GLOB_RECURSE graphics_kernel_cpp_files ${CMAKE_CURRENT_LIST_DIR}/*.cpp ${CMAKE_CURRENT_LIST_DIR}/*.cc)

file(GLOB graphics_ui_files ${CMAKE_CURRENT_LIST_DIR}/*.ui)


set(graphics_qrc_files graphicssource.qrc)


add_library(GraphicsModelingKernel ${LIB_TYPE} 
    ${graphics_kernel_header_files}
    ${graphics_kernel_cpp_files}
    ${graphics_ui_files}
    ${graphics_qrc_files}
    )
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${GROUP_NAME}")


target_include_directories(
    GraphicsModelingKernel
    PRIVATE
	${graphics_sub_dirs}
)



target_link_libraries(
    GraphicsModelingKernel
    PRIVATE 
    Qt5::Gui
    Qt5::Core
    Qt5::Svg  
    Qt5::PrintSupport
    Qt5::Concurrent
    QP::KLWidgets
    ${ThirdPartyTargetPrefix}qwt
    ${ThirdPartyTargetPrefix}qwtmathml
    ${libprotobuf_target}
)

target_compile_definitions(
    GraphicsModelingKernel
    PRIVATE 
    WIN32
    QT_DLL
    QT_CORE_LIB
    QT_GUI_LIB
    GRAPHICSMODELINGKERNEL_LIB
    QT_SVG_LIB
    PROTOBUF_USE_DLLS
    QT_DISABLE_DEPRECATED_BEFORE=0
    QT_DEPRECATED_WARNINGS 
)



copy_directory_files(GraphicsModelingKernel "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_SOURCE_DIR}/common/include/GraphicsModelingKernel")

# 指定输出目录和安装内容
TARGET_AS_BIN(${PROJECT_NAME})