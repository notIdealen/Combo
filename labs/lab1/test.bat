@echo off
setlocal enabledelayedexpansion
set PROG=qap.exe
set "RESULT="

echo ------------------------------------------
echo calculate matrix 4x4
set "EXPECTED=5050"
for /f "delims=" %%i in ('%PROG% .\data\qm4x4.txt .\data\cm4x4.txt 2^>nul') do (
    echo %%i
    if not defined RESULT set "RESULT=%%i"
)
set "RESULT=%RESULT: =%"
if "%RESULT%"=="%EXPECTED%" (echo [OK] valid) else (goto err)
echo ------------------------------------------ 
echo swap matrix
set "EXPECTED=5050"
for /f "delims=" %%i in ('%PROG% .\data\cm4x4.txt .\data\qm4x4.txt 2^>nul') do (
    echo %%i
    if not defined RESULT set "RESULT=%%i"
)
set "RESULT=%RESULT: =%"
if "%RESULT%"=="%EXPECTED%" (echo [OK] valid) else (goto err)
echo ------------------------------------------ 
echo diagonal != 0
echo ------------------------------------------
echo calculate matrix 10x10 
echo ------------------------------------------
echo calculate matrix 1x1 
echo ------------------------------------------
echo calculate matrix 2x2 

exit /b 0

:err
echo [FAIL] Result invalid: expected %EXPECTED%, recived %RESULT%
exit /b 1
