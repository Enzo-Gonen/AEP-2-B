@echo off
setlocal EnableExtensions
chcp 65001 >nul
title PSE em Acao - Protocolo Sanctum V5
cd /d "%~dp0"

echo.
echo  ======================================================================
echo       ORDO INTEGRA-PSE  ^|  PROTOCOLO SANCTUM V5
echo       DEPENDENCY MANAGER - BUILD - AUTO START
echo  ======================================================================
echo.

if not exist "scripts\bootstrap.ps1" (
    echo [ERRO] scripts\bootstrap.ps1 nao encontrado.
    echo Extraia TODO o conteudo do ZIP antes de executar.
    pause
    exit /b 1
)
if not exist "src\main.c" (
    echo [ERRO] src\main.c nao encontrado. Execute este BAT na pasta raiz do projeto.
    pause
    exit /b 1
)
where powershell.exe >nul 2>nul
if errorlevel 1 (
    echo [ERRO] Windows PowerShell nao foi encontrado.
    pause
    exit /b 1
)

echo [1/3] INSTALACAO E VALIDACAO DE DEPENDENCIAS
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\bootstrap.ps1"
if errorlevel 1 (
    echo.
    echo [ERRO] Nao foi possivel preparar o ambiente.
    echo Confira a mensagem [ERRO DE INSTALACAO] acima.
    pause
    exit /b 1
)

if not exist "build\gcc_path.txt" (
    echo [ERRO] O instalador nao criou build\gcc_path.txt.
    pause
    exit /b 1
)
set "GCC_EXE="
set /p GCC_EXE=<"build\gcc_path.txt"
if not exist "%GCC_EXE%" (
    echo [ERRO] GCC nao existe no caminho registrado:
    echo "%GCC_EXE%"
    pause
    exit /b 1
)

for %%I in ("%GCC_EXE%") do set "GCC_DIR=%%~dpI"
set "PATH=%GCC_DIR%;%PATH%"

if not exist "build" mkdir "build"

echo.
echo [2/3] COMPILANDO APLICACAO
"%GCC_EXE%" -Iinclude -std=c99 -Wall -Wextra -Wpedantic -O2 src\main.c src\dominio.c src\repositorio.c src\armazenamento.c src\interface.c -o build\pse_em_acao.exe
if errorlevel 1 (
    echo.
    echo [ERRO] A compilacao falhou. Verifique a saida do GCC.
    pause
    exit /b 1
)
if not exist "build\pse_em_acao.exe" (
    echo [ERRO] O executavel nao foi gerado.
    pause
    exit /b 1
)

echo.
echo [3/3] INICIANDO PROTOCOLO SANCTUM
echo.
"build\pse_em_acao.exe"
set "APP_EXIT=%ERRORLEVEL%"

echo.
if "%APP_EXIT%"=="0" (
    echo [SANCTUM] Programa encerrado normalmente.
) else (
    echo [SANCTUM] Programa encerrou com codigo %APP_EXIT%.
)
pause
exit /b %APP_EXIT%
