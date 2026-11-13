@echo off
chcp 65001 >nul
title PSE em Ação
where gcc >nul 2>nul
if errorlevel 1 (
  echo GCC nao encontrado. Instale MinGW-w64 e adicione bin ao PATH.
  pause
  exit /b 1
)
gcc -std=c99 -Wall -Wextra -pedantic -O2 src\main.c -o pse_em_acao.exe
if errorlevel 1 (
  echo Erro na compilacao.
  pause
  exit /b 1
)
pse_em_acao.exe
pause
