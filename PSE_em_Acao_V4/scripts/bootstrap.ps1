$ErrorActionPreference = "Stop"

function Write-Status([string]$Message) {
    Write-Host "[SANCTUM] $Message" -ForegroundColor Yellow
}

function Find-Gcc {
    $cmd = Get-Command gcc -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }

    $candidates = @(
        "C:\msys64\ucrt64\bin\gcc.exe",
        "C:\msys64\mingw64\bin\gcc.exe",
        "$env:LOCALAPPDATA\Programs\msys64\ucrt64\bin\gcc.exe"
    )
    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) { return $candidate }
    }
    return $null
}

Write-Status "Verificando compilador GCC..."
$gcc = Find-Gcc

if (-not $gcc) {
    Write-Status "GCC nao encontrado. O projeto nao utiliza bibliotecas externas; sera instalado apenas o toolchain C."
    $winget = Get-Command winget.exe -ErrorAction SilentlyContinue
    if (-not $winget) {
        throw "O Windows Package Manager (winget) nao esta disponivel. Instale/atualize o App Installer pela Microsoft Store e execute novamente."
    }

    Write-Status "Instalando MSYS2 pelo Windows Package Manager..."
    & $winget.Source install --id MSYS2.MSYS2 --exact --silent --accept-package-agreements --accept-source-agreements
    if ($LASTEXITCODE -ne 0) {
        throw "A instalacao do MSYS2 pelo winget falhou (codigo $LASTEXITCODE). Verifique permissao de instalacao e tente novamente."
    }

    $gcc = Find-Gcc
    if (-not $gcc) {
        $bashCandidates = @("C:\msys64\usr\bin\bash.exe", "$env:LOCALAPPDATA\Programs\msys64\usr\bin\bash.exe")
        $bash = $bashCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
        if (-not $bash) {
            throw "MSYS2 foi instalado, mas o executavel bash.exe nao foi localizado."
        }

        Write-Status "Instalando GCC e ferramentas de compilacao dentro do MSYS2..."
        & $bash --login -c "pacman -Sy --noconfirm && pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc"
        if ($LASTEXITCODE -ne 0) {
            throw "O gerenciador pacman nao conseguiu instalar o GCC (codigo $LASTEXITCODE)."
        }
        $gcc = Find-Gcc
    }
}

if (-not $gcc) {
    throw "GCC continua indisponivel depois da instalacao."
}

$gccDir = Split-Path -Parent $gcc
if (($env:PATH -split ';') -notcontains $gccDir) {
    $env:PATH = "$gccDir;$env:PATH"
}

$version = & $gcc --version 2>&1 | Select-Object -First 1
Write-Status "Compilador pronto: $version"
Write-Status "Nenhuma dependencia adicional de terceiros e necessaria."
exit 0
