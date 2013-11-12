@ECHO OFF

set l_targetPath=%1
set l_sourcePath=%2


REM Check if pre compiler is already installed
set l_file=""
IF EXIST Val3Check_orig.exe GOTO ALREADY_INSTALLED

IF "%l_targetPath%"=="" GOTO ERROR
IF "%l_sourcePath%"=="" GOTO ERROR


REM Copy Stäubli Val3Check to Val3Check_orig
rename %l_targetPath%\Val3Check.exe Val3Check_Orig.exe
goto END

:ERROR
ECHO.
ECHO ERROR IN PARAMETER. Format is :
ECHO installPreCompiler targetPath sourcePath 
EXIT /b 1

:ALREADY_INSTALLED
ECHO.
ECHO The pre compiler is already installed
EXIT /b 2

:END
EXIT /b 0
goto :eof