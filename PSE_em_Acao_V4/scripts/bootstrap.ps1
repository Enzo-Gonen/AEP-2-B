$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build"
New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
$GccPathFile = Join-Path $BuildDir "gcc_path.txt"
Remove-Item $GccPathFile -Force -ErrorAction SilentlyContinue

function Write-Status([string]$Message) {
    Write-Host "[SANCTUM] $Message" -ForegroundColor Yellow
}

function Find-Gcc {
    $cmd = Get-Command gcc.exe -ErrorAction SilentlyContinue
    if ($cmd -and (Test-Path $cmd.Source)) { return $cmd.Source }

    $candidates = @(
        "C:\msys64\ucrt64\bin\gcc.exe",
        "C:\msys64\mingw64\bin\gcc.exe",
        "C:\msys64\mingw64\bin\gcc.exe",
        "$env:LOCALAPPDATA\Programs\msys64\ucrt64\bin\gcc.exe",
        "$env:LOCALAPPDATA\Programs\mingw64\bin\gcc.exe"
    )
    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path $candidate)) { return $candidate }
    }
    return $null
}

try {
    Write-Status "Verificando compilador C..."
    $gcc = Find-Gcc

    if (-not $gcc) {
        $winget = Get-Command winget.exe -ErrorAction SilentlyContinue
        if (-not $winget) {
            throw "O winget nao foi encontrado. Instale/atualize o aplicativo App Installer pela Microsoft Store e tente novamente."
        }

        Write-Status "GCC ausente. Instalando MSYS2 pelo winget..."
        & $winget.Source install --id MSYS2.MSYS2 --exact --silent --accept-package-agreements --accept-source-agreements
        if ($LASTEXITCODE -ne 0) {
            throw "winget retornou o codigo $LASTEXITCODE ao instalar MSYS2."
        }

        $gcc = Find-Gcc
        if (-not $gcc) {
            $bashCandidates = @(
                "C:\msys64\usr\bin\bash.exe",
                "$env:LOCALAPPDATA\Programs\msys64\usr\bin\bash.exe"
            )
            $bash = $bashCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
            if (-not $bash) {
                throw "MSYS2 nao foi localizado nos caminhos padrao. Verifique a instalacao e execute novamente."
            }

            Write-Status "Instalando GCC UCRT64 com pacman. Esta etapa pode levar alguns minutos..."
            & $bash --login -c "pacman -Sy --noconfirm && pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc"
            if ($LASTEXITCODE -ne 0) {
                throw "pacman retornou o codigo $LASTEXITCODE durante a instalacao do GCC."
            }
            $gcc = Find-Gcc
        }
    }

    if (-not $gcc) {
        throw "GCC nao foi encontrado apos a verificacao/instalacao."
    }

    $gcc = (Resolve-Path $gcc).Path
    $version = & $gcc --version 2>&1 | Select-Object -First 1
    if ($LASTEXITCODE -ne 0) { throw "O compilador foi localizado, mas nao conseguiu iniciar." }

    Set-Content -LiteralPath $GccPathFile -Value $gcc -Encoding ASCII
    Write-Status "Compilador validado: $version"
    Write-Status "Dependencias externas do projeto: nenhuma."
    exit 0
}
catch {
    Write-Host ""
    Write-Host "[ERRO SANCTUM] $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
