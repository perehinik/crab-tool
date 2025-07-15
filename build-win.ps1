# Go to script directory
Set-Location -Path $PSScriptRoot

$env:Path += ";C:\Qt\Tools\mingw1310_64\bin"

# Build directory
$buildDir = Join-Path $PSScriptRoot "build"

# Create and clean build directory
if (-Not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
} else {
    Remove-Item -Recurse -Force "$buildDir\*"
}

# Detect the latest Qt 6 version installed in C:\Qt
$qtRoot = "C:\Qt"
$qtInstall = Get-ChildItem -Path $qtRoot -Directory |
    Where-Object { $_.Name -match '^6\.\d+\.\d+$' } |
    Sort-Object Name -Descending |
    Select-Object -First 1

if (-not $qtInstall) {
    Write-Error "No Qt 6.x installations found in $qtRoot"
    exit 1
}

# Detect the compiler-specific subdirectory (e.g., msvc2019_64)
$qtCompilerDir = Get-ChildItem -Path $qtInstall.FullName -Directory |
    Where-Object { $_.Name -like 'msvc*' -or $_.Name -like 'mingw*' } |
    Select-Object -First 1

if (-not $qtCompilerDir) {
    Write-Error "No MSVC compiler subdirectory found under $($qtInstall.FullName)"
    exit 1
}

$qtBinDir = Join-Path $qtCompilerDir.FullName "bin"
$windeployqt = Join-Path $qtBinDir "windeployqt.exe"

if (-not (Test-Path $windeployqt)) {
    Write-Error "windeployqt.exe not found in $qtBinDir"
    exit 1
}

Write-Host "Using Qt from: $qtCompilerDir"

# Configure project with CMake
cmake -G "MinGW Makefiles" -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$qtCmakeDir"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Build the project
cmake --build build
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# App executable name (adjust if different)
$exeName = "crab-tool-1.0.0.exe"
$exePath = Join-Path $buildDir $exeName

# Run windeployqt to copy required Qt DLLs
Write-Host "Copy DLLs using windeployqt"
& $windeployqt --dir $buildDir $exePath
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Package the app using CPack
Write-Host "Create package using CPack"
Push-Location $buildDir
cpack
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Pop-Location

