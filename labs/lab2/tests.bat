@echo off
set "PROG=bag.exe"

echo TEST1: standart test
set "EXPECTED=15"
set "WAY=./data/wp.txt"
set "W8=16"
for /f "delims=" %%i in ('%PROG% %WAY% %W8% 2^>nul') do (set "RESULT=%%i")
if "%RESULT%"=="%EXPECTED%" (echo [OK] valid) else (goto err)
echo.
echo ------------------------------------------
echo TEST2: weight = 0
set "EXPECTED=24"
set "WAY=data/w0.txt"
set "W8=16"
for /f "delims=" %%i in ('%PROG% %WAY% %W8% 2^>nul') do (set "RESULT=%%i")
if "%RESULT%"=="%EXPECTED%" (echo [OK] valid) else (goto err)
echo.
echo ------------------------------------------
echo TEST3: weight more than max
set "EXPECTED=0"
set "WAY=data/wover.txt"
set "W8=16"
for /f "delims=" %%i in ('%PROG% %WAY% %W8% 2^>nul') do (set "RESULT=%%i")
if "%RESULT%"=="%EXPECTED%" (echo [OK] valid) else (goto err)
echo.
echo ------------------------------------------


exit /b 0

:err
echo [FAIL] Result invalid: expected %EXPECTED%, recived %RESULT%
exit /b 1
