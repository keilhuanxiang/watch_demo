param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$IdfArgs
)

$ErrorActionPreference = "Stop"

function Get-VsCodeSettingValue {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Key
    )

    $settingsPath = Join-Path $PSScriptRoot "..\.vscode\settings.json"
    if (-not (Test-Path $settingsPath)) {
        return $null
    }

    $settings = Get-Content $settingsPath -Raw | ConvertFrom-Json
    return $settings.$Key
}

function Resolve-IdfPath {
    if ($env:IDF_PATH -and (Test-Path $env:IDF_PATH)) {
        return $env:IDF_PATH
    }

    $idfPath = Get-VsCodeSettingValue -Key "idf.espIdfPathWin"
    if ($idfPath -and (Test-Path $idfPath)) {
        return $idfPath
    }

    throw "Unable to resolve IDF_PATH. Set IDF_PATH or configure idf.espIdfPathWin in .vscode/settings.json."
}

function Resolve-IdfToolsPath {
    if ($env:IDF_TOOLS_PATH -and (Test-Path $env:IDF_TOOLS_PATH)) {
        return $env:IDF_TOOLS_PATH
    }

    $toolsPath = Get-VsCodeSettingValue -Key "idf.toolsPathWin"
    if ($toolsPath -and (Test-Path $toolsPath)) {
        return $toolsPath
    }

    throw "Unable to resolve IDF_TOOLS_PATH. Set IDF_TOOLS_PATH or configure idf.toolsPathWin in .vscode/settings.json."
}

function Resolve-IdfTarget {
    $customVars = Get-VsCodeSettingValue -Key "idf.customExtraVars"
    if ($customVars -and $customVars.IDF_TARGET) {
        return [string]$customVars.IDF_TARGET
    }

    return $null
}

$idfPath = Resolve-IdfPath
$idfToolsPath = Resolve-IdfToolsPath
$idfTarget = Resolve-IdfTarget
$exportScript = Join-Path $idfPath "export.ps1"
$idfPy = Join-Path $idfPath "tools\idf.py"
$idfPython = $null

if (-not (Test-Path $exportScript)) {
    throw "ESP-IDF export script not found: $exportScript"
}

if (-not (Test-Path $idfPy)) {
    throw "idf.py not found: $idfPy"
}

$env:IDF_PATH = $idfPath
$env:IDF_TOOLS_PATH = $idfToolsPath

if ($idfTarget) {
    $env:IDF_TARGET = $idfTarget
}

. $exportScript

if (-not $IdfArgs -or $IdfArgs.Count -eq 0) {
    $IdfArgs = @("build")
}

if ($env:IDF_PYTHON_ENV_PATH) {
    $idfPython = Join-Path $env:IDF_PYTHON_ENV_PATH "Scripts\python.exe"
}

if (-not $idfPython -or -not (Test-Path $idfPython)) {
    $pythonCmd = Get-Command python -ErrorAction Stop
    $idfPython = $pythonCmd.Source
}

Write-Host "Using IDF_PATH=$env:IDF_PATH"
Write-Host "Using IDF_TOOLS_PATH=$env:IDF_TOOLS_PATH"
if ($env:IDF_TARGET) {
    Write-Host "Using IDF_TARGET=$env:IDF_TARGET"
}
Write-Host "Using Python=$idfPython"

& $idfPython $idfPy @IdfArgs
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
