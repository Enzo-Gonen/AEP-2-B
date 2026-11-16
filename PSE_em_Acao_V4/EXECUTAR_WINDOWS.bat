@echo off
setlocal EnableExtensions
chcp 65001 >nul
title PSE em Ação - Protocolo Sanctum V4
cd /d "%~dp0"

echo.
echo  ================================================================
echo       ORDO INTEGRA-PSE  ^|  PROTOCOLO SANCTUM V4
echo       VERIFICACAO ^> COMPILACAO ^> INICIALIZACAO
echo  ================================================================
echo.

if not exist "scripts\bootstrap.ps1" (
    echo [ERRO] Arquivo scripts\bootstrap.ps1 nao encontrado.
    echo Extraia o ZIP inteiro antes de executar este BAT.
    pause
    exit /b 1
)

if not exist "src\main.c" (
    echo [ERRO] Codigo-fonte src\main.c nao encontrado.
    echo Execute o BAT a partir da pasta raiz PSE_em_Acao_V4.
    pause
    exit /b 1
)

where powershell.exe >nul 2>nul
if errorlevel 1 (
    echo [ERRO] Windows PowerShell nao encontrado.
    pause
    exit /b 1
)

echo [1/3] Verificando dependencias...
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\bootstrap.ps1"
if errorlevel 1 (
    echo.
    echo [ERRO] A preparacao do ambiente falhou.
    echo Leia a mensagem [ERRO SANCTUM] exibida acima.
    pause
    exit /b 1
)

if not exist "build\gcc_path.txt" (
    echo [ERRO] O bootstrap nao gerou build\gcc_path.txt.
    pause
    exit /b 1
)

set "GCC_EXE="
set /p GCC_EXE=<"build\gcc_path.txt"
if not exist "%GCC_EXE%" (
    echo [ERRO] Compilador nao encontrado no caminho:
    echo "%GCC_EXE%"
    pause
    exit /b 1
)

if not exist "build" mkdir "build"

echo.
echo [2/3] Compilando o projeto...
"%GCC_EXE%" -Iinclude -std=c99 -Wall -Wextra -Wpedantic -O2 src\main.c src\dominio.c src\repositorio.c src\armazenamento.c src\interface.c -o build\pse_em_acao.exe
if errorlevel 1 (
    echo.
    echo [ERRO] A compilacao falhou. Verifique as mensagens do GCC.
    pause
    exit /b 1
)

if not exist "build\pse_em_acao.exe" (
    echo [ERRO] O executavel nao foi criado.
    pause
    exit /b 1
)

echo.
echo [3/3] Iniciando o Protocolo Sanctum...
echo.
"build\pse_em_acao.exe"
set "APP_EXIT=%ERRORLEVEL%"

echo.
if "%APP_EXIT%"=="0" (
    echo [SANCTUM] Aplicacao encerrada normalmente.
) else (
    echo [SANCTUM] Aplicacao finalizada com codigo %APP_EXIT%.
)
pause
exit /b %APP_EXIT%
