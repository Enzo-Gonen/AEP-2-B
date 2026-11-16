# PSE em Acao V5 - instalador de dependencias
$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build"
New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
$GccPathFile = Join-Path $BuildDir "gcc_path.txt"
Remove-Item -LiteralPath $GccPathFile -Force -ErrorAction SilentlyContinue

function Status([string]$Text) {
    Write-Host "[SANCTUM SETUP] $Text" -ForegroundColor Cyan
}
function Fail([string]$Text) {
    throw $Text
}
function Find-MsysRoot {
    $roots = @(
        "C:\msys64",
        "$env:LOCALAPPDATA\Programs\MSYS2",
        "$env:LOCALAPPDATA\msys64"
    )
    foreach ($r in $roots) {
        if (Test-Path (Join-Path $r "usr\bin\bash.exe")) { return $r }
    }
    return $null
}
function Find-Gcc {
    $cmd = Get-Command gcc.exe -ErrorAction SilentlyContinue
    if ($cmd -and (Test-Path $cmd.Source)) { return $cmd.Source }
    $roots = @(
        "C:\msys64",
        "$env:LOCALAPPDATA\Programs\MSYS2",
        "$env:LOCALAPPDATA\msys64"
    )
    foreach ($r in $roots) {
        foreach ($rel in @("ucrt64\bin\gcc.exe", "mingw64\bin\gcc.exe")) {
            $candidate = Join-Path $r $rel
            if (Test-Path $candidate) { return $candidate }
        }
    }
    return $null
}

try {
    Status "Verificando sistema operacional e ferramentas..."
    if (-not $env:OS -or $env:OS -ne "Windows_NT") {
        Fail "Este instalador automatico foi preparado para Windows 10/11."
    }

    $gcc = Find-Gcc
    if (-not $gcc) {
        $msysRoot = Find-MsysRoot

        if (-not $msysRoot) {
            Status "MSYS2 nao encontrado. Verificando Windows Package Manager (winget)..."
            $winget = Get-Command winget.exe -ErrorAction SilentlyContinue
            if (-not $winget) {
                Fail "winget nao esta instalado. Instale ou atualize 'App Installer' pela Microsoft Store e execute novamente."
            }

            Status "Instalando MSYS2 automaticamente. Aguarde a conclusao..."
            & $winget.Source install --id MSYS2.MSYS2 --exact --silent --accept-package-agreements --accept-source-agreements
            $wingetExit = $LASTEXITCODE
            if ($wingetExit -ne 0) {
                Fail "winget falhou ao instalar MSYS2 (codigo $wingetExit)."
            }

            $msysRoot = Find-MsysRoot
            if (-not $msysRoot) {
                Fail "O instalador informou sucesso, mas MSYS2 nao foi encontrado nos locais padrao. Verifique a instalacao."
            }
        }

        $bash = Join-Path $msysRoot "usr\bin\bash.exe"
        $ucrtGcc = Join-Path $msysRoot "ucrt64\bin\gcc.exe"
        $mingwGcc = Join-Path $msysRoot "mingw64\bin\gcc.exe"

        if (-not (Test-Path $ucrtGcc) -and -not (Test-Path $mingwGcc)) {
            Status "Instalando toolchain GCC e bibliotecas de runtime pelo pacman..."
            & $bash --login -c "pacman -Sy --noconfirm"
            if ($LASTEXITCODE -ne 0) {
                Fail "Falha ao atualizar os indices de pacotes do MSYS2 (pacman)."
            }

            & $bash --login -c "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc"
            if ($LASTEXITCODE -ne 0) {
                Fail "Falha ao instalar GCC UCRT64 e suas dependencias pelo pacman."
            }
        }

        $gcc = Find-Gcc
    }

    if (-not $gcc) {
        Fail "GCC nao foi localizado mesmo apos a instalacao automatica."
    }

    $gcc = (Resolve-Path -LiteralPath $gcc).Path
    Status "Validando compilador..."
    $versionOutput = & $gcc --version 2>&1
    if ($LASTEXITCODE -ne 0) {
        Fail "GCC foi encontrado, mas nao conseguiu executar. Reinstale o toolchain MSYS2."
    }

    Set-Content -LiteralPath $GccPathFile -Value $gcc -Encoding ASCII
    Status ("Compilador pronto: " + (($versionOutput | Select-Object -First 1) -join ""))
    Status "Todas as dependencias necessarias para compilar o projeto estao disponiveis."
    exit 0
}
catch {
    Write-Host ""
    Write-Host "[ERRO DE INSTALACAO] $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
