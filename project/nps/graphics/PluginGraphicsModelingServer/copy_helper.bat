@echo off
set projectDir=%1
set dstDir=%2

IF NOT EXIST %dstDir% (
	md %dstDir%
)

set filelist=IPluginGraphicModelingServer.h;GraphicsModelingConst.h

for %%a in (%filelist%) do (
echo %%a
if exist "%projectDir%%%a" (
	xcopy "%projectDir%%%a" %dstDir% /y
	)
)