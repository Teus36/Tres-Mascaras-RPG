@echo off
setlocal
cd /d "%~dp0"
for /f "tokens=2 delims=:" %%C in ('chcp') do set "TM_CODEPAGE=%%C"
chcp 65001 >nul
if not exist tres_mascaras.exe (
    echo Primeiro compile o projeto, conforme o README.md.
    if defined TM_CODEPAGE chcp %TM_CODEPAGE% >nul
    pause
    exit /b 1
)
tres_mascaras.exe %*
if defined TM_CODEPAGE chcp %TM_CODEPAGE% >nul
pause
endlocal
