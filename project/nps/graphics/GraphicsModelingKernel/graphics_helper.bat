@echo on
set projectDir=%1
set dstDir=%2

IF NOT EXIST %dstDir% (
	md %dstDir%
)

echo %projectDir%include
echo %dstDir%

xcopy %projectDir%include %dstDir%  /q /e /r /S /Y


::set filelist=Global.h;^
::CanvasDataStruct\SourceProxy.h;^
::CanvasDataStruct\ISourceProxy.h;^
::CanvasDataStruct\ConnectorWireContext.h;^
::ICanvasView.h;^
::InternalGraphicsObject\SourceGraphicsObject.h;^
::InternalGraphicsObject\PortGraphicsObject.h;^
::InternalGraphicsFactory\ICanvasGraphicsObjectFactory.h;^
::graphicsmodelingkernel.h;^
::ICanvasScene.h;^
::ICanvasView.h;^
::CanvasDataStruct\PortContext.h;^
::CanvasDataStruct\CanvasContext.h;^
::CanvasDataStruct\ICanvasContext.h;^
::InternalGraphicsObject\TransformProxyGraphicsObject.h;^
::InternalGraphicsObject\GraphicsLayer.h;^
::BusinessHooksServer.h;^
::Manager\ActionManager.h;^
::GraphicsKernelDefinition.h;^
::Manager\GraphicsLayerManager.h;^
::UI\ViewToolBar.h;^
::UI\BookMark.h
::
::for %%a in (%filelist%) do (
::echo %projectDir%%%a
::if exist "%projectDir%%%a" (
::	xcopy "%projectDir%%%a" %dstDir% /y
::	)
::)