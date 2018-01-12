@echo off
rem setLocal EnableDelayedExpansion

rem base path where all releases and updates go
set basepath=D:\data\SwissSystem\packages

rem retrieve Plastics version number from file version.dat
for /f %%v IN (..\version.dat) DO set version=%%v
echo version: %version%

rem retrieve svn revision number
rem for /F "tokens=2 delims= " %%a in ('svn info ^| findstr Revision') do set revision=%%a
for /F "tokens=1 delims= " %%a in ('svnversion -c -n ../../') do set revision=%%a
set revision=%revision::=_%
echo repository revision: %revision%


set versionpath=%version%_rev%revision%
set currentDir=%CD%
set devDocDestination=%basepath%\doc\%versionpath%\dev\
set progDocDestination=%basepath%\doc\%versionpath%\prog\

REM Set temp directory
IF NOT "%TEMP%"=="" set l_temp=%TEMP%\plasticsDocs
IF "%TEMP%"=="" set l_temp=C:\TEMP\plasticsDocs
IF NOT EXIST %l_temp% MD %l_temp%
echo temp path: %l_temp%



rem copy auxiliary files
xcopy /s /i /y gfx %devDocDestination%html\gfx\
xcopy /s /i /y gfx %progDocDestination%html\gfx\
copy *.htm %devDocDestination%html
copy *.htm %progDocDestination%html

rem replace '\' with '/' to make paths work with doxygen
set devDocDestinationUnix=%devDocDestination:\=/%
set progDocDestinationUnix=%progDocDestination:\=/%
set tempUnix=%l_temp:\=/%

rem clear temp directory
del /s /q "%l_temp%"

rem extract documentation from code
for /f "tokens=*" %%G in ('dir /b /a:d "..\modules\*"') do ("C:\Program Files (x86)\Saxe Swiss System Val3 Tools\extractDoc.exe" -dest "%l_temp%" -source "%cd%\..\modules\%%G" )
for /f "tokens=*" %%G in ('dir /b /a:d "..\coremodules\*"') do ("C:\Program Files (x86)\Saxe Swiss System Val3 Tools\extractDoc.exe" -dest "%l_temp%" -source "%cd%\..\coremodules\%%G" )

rem for /d %%D in (..\modules\*) do (
rem echo 1
rem "C:\Program Files (x86)\Saxe Swiss System Val3 Tools\extractDoc.exe" -dest "%l_temp%" -source %%G 

rem for /d %%D in (.\coremodules\*) do 
rem "C:\Program Files (x86)\Saxe Swiss System Val3 Tools\extractDoc.exe" -dest "%l_temp%" -source "%%G"

type DoxyfilePlasticProgrammer | _replaceStr #Temp# %tempUnix% | _replaceStr #Version# %version% | _replaceStr #ProgDocPath# %progDocDestinationUnix% > %l_temp%\DoxyfilePlasticProgrammer
type DoxyfilePlasticDevelopper | _replaceStr #Temp# %tempUnix% | _replaceStr #Version# %version% | _replaceStr #DevDocPath# %devDocDestinationUnix% > %l_temp%\DoxyfilePlasticDevelopper

IF NOT EXIST %progDocDestination% mkdir %progDocDestination%
IF NOT EXIST %devDocDestination% mkdir %devDocDestination%

"C:\Program Files\doxygen\bin\doxygen.exe" %l_temp%\DoxyfilePlasticProgrammer
rem "C:\Program Files\doxygen\bin\doxygen.exe" %l_temp%\DoxyfilePlasticDevelopper

rem copy index.html one level up
copy redirect.html %devDocDestination%index.html
copy redirect.html %progDocDestination%index.html
start %progDocDestination%