@echo off

REM call another cmd so that msbuild doesnt exit our cmd window
REM cmd.exe /k _make.bat

call _make.bat

REM wait for user input then exit
pause
