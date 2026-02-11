@echo off
@REM setlocal enabledelayedexpansion
set PROG=qap.exe

echo ------------------------------------------
echo calculate matrix 4x4
set "EXPECTED=5050"
set "RESULT="
for /f "delims=" %%i in ('%PROG% .\data\qm4x4.txt .\data\cm4x4.txt 2^>nul') do (
    echo %%i
    if not defined RESULT set "RESULT=%%i"
)
set "RESULT=%RESULT: =%"
if "%RESULT%"=="%EXPECTED%" (echo [OK] valid) else (goto err)
echo ------------------------------------------ 
echo swap matrix
set "EXPECTED=5050"
set "RESULT="
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
set "EXPECTED=42386"
set "RESULT="
for /f "delims=" %%i in ('%PROG% .\data\qm10x10.txt .\data\cm10x10.txt 2^>nul') do (
    echo %%i
    if not defined RESULT set "RESULT=%%i"
)
set "RESULT=%RESULT: =%"
if "%RESULT%"=="%EXPECTED%" (echo [OK] valid) else (goto err)
echo ------------------------------------------
echo calculate matrix 1x1 
echo ------------------------------------------
echo calculate matrix 2x2 
echo ------------------------------------------
echo calculate matrix 20x20
%PROG% 20
echo ------------------------------------------
echo calculate matrix 50x50
%PROG% 50
echo Universe age: 1.38e+10
exit /b 0

:err
echo [FAIL] Result invalid: expected %EXPECTED%, recived %RESULT%
exit /b 1
