@echo off
setlocal EnableExtensions
chcp 65001 >nul
title PSE em Ação - Protocolo Sanctum V4
cd /d "%~dp0"

echo.
echo  ================================================================
echo       ORDO INTEGRA-PSE  ^|  PROTOCOLO SANCTUM V4
echo       Bootstrap de dependencias e inicializacao automatica
echo  ================================================================
echo.

where powershell.exe >nul 2>nul
if errorlevel 1 (
  echo [FALHA] Windows PowerShell nao foi encontrado.
  pause
  exit /b 1
)

echo [1/3] Verificando e instalando dependencias...
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\bootstrap.ps1"
if errorlevel 1 (
  echo.
  echo [FALHA] Nao foi possivel preparar o ambiente.
  echo Consulte as mensagens acima e execute novamente.
  pause
  exit /b 1
)

if exist "C:\msys64\ucrt64\bin\gcc.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\mingw64\bin\gcc.exe" set "PATH=C:\msys64\mingw64\bin;%PATH%"
if exist "%LOCALAPPDATA%\Programs\msys64\ucrt64\bin\gcc.exe" set "PATH=%LOCALAPPDATA%\Programs\msys64\ucrt64\bin;%PATH%"

echo.
echo [2/3] Compilando o sistema...
if not exist build mkdir build
gcc -Iinclude -std=c99 -Wall -Wextra -Wpedantic -O2 src\main.c src\dominio.c src\repositorio.c src\armazenamento.c src\interface.c -o build\pse_em_acao.exe
if errorlevel 1 (
  echo.
  echo [FALHA] Compilacao interrompida. O codigo-fonte precisa ser verificado.
  pause
  exit /b 1
)

echo.
echo [3/3] Inicializando o Protocolo Sanctum...
echo.
build\pse_em_acao.exe
set "APP_EXIT=%ERRORLEVEL%"

echo.
if "%APP_EXIT%"=="0" (
  echo [SANCTUM] Sessao encerrada normalmente.
) else (
  echo [AVISO] O programa terminou com codigo %APP_EXIT%.
)
pause
exit /b %APP_EXIT%
