set SolutionDir=%1
set SolutionName=%2
set ProjectDir=%3
set ProjectName=%4
set Platform=%5
set Configuration=%6
set TargetExt=%7

::拷贝配置文件
::xcopy ".\ExPlugins\%Platform%\%Configuration%\*.dll" "%SolutionDir%..\..\bin\%Platform%\%Configuration%\plugins\" /Y /F
::xcopy ".\ExPlugins\%Platform%\%Configuration%\depends\*.dll" "%SolutionDir%..\..\bin\%Platform%\%Configuration%\" /S /Y /F

::拷贝simucad配置文件
if exist ".\ExPlugins\simucad" (
	if exist "%SolutionDir%..\..\bin\%Platform%\%Configuration%\simucad" (
		rd /s /Q "%SolutionDir%..\..\bin\%Platform%\%Configuration%\simucad"
	)
	xcopy ".\ExPlugins\simucad" "%SolutionDir%..\..\bin\%Platform%\%Configuration%\simucad\" /S /Y /F
)