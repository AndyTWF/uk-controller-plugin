param(
    [Parameter(Mandatory=$true)]
    [string]$BinDirectory
)

Write-Host "=== DLL Load Test Script ===" -ForegroundColor Cyan
Write-Host "Testing DLL loading from: $BinDirectory"

# Check if PowerShell is running as 32-bit or 64-bit
$is64Bit = [Environment]::Is64BitProcess
Write-Host "PowerShell process architecture: $( if ($is64Bit) { '64-bit' } else { '32-bit' } )" -ForegroundColor Cyan
Write-Host "OS architecture: $( if ([Environment]::Is64BitOperatingSystem) { '64-bit' } else { '32-bit' } )" -ForegroundColor Cyan

# Define the P/Invoke signatures
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;

public class Kernel32 {
    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    public static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr hReservedNull, uint dwFlags);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool FreeLibrary(IntPtr hModule);

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern uint GetLastError();

    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    public static extern uint FormatMessage(uint dwFlags, IntPtr lpSource, uint dwMessageId,
        uint dwLanguageId, StringBuilder lpBuffer, uint nSize, IntPtr Arguments);

    public const uint LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR = 0x00000100;
    public const uint LOAD_LIBRARY_SEARCH_DEFAULT_DIRS = 0x00001000;
    public const uint FORMAT_MESSAGE_FROM_SYSTEM = 0x00001000;
}
"@

function Get-Win32ErrorMessage {
    param([uint]$ErrorCode)

    $buffer = New-Object System.Text.StringBuilder 1024
    $result = [Kernel32]::FormatMessage(
        [Kernel32]::FORMAT_MESSAGE_FROM_SYSTEM,
        [IntPtr]::Zero,
        $ErrorCode,
        0,
        $buffer,
        $buffer.Capacity,
        [IntPtr]::Zero
    )

    if ($result -eq 0) {
        return "Unknown error"
    }

    return $buffer.ToString().Trim()
}

function Test-DllLoad {
    param(
        [string]$DllPath,
        [string]$DllName
    )

    Write-Host "`nTesting: $DllName" -ForegroundColor Yellow
    Write-Host "  Full path: $DllPath"

    if (-not (Test-Path $DllPath)) {
        Write-Host "  ERROR: File does not exist!" -ForegroundColor Red
        return $false
    }

    $handle = [Kernel32]::LoadLibraryEx(
        $DllPath,
        [IntPtr]::Zero,
        [Kernel32]::LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR -bor [Kernel32]::LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
    )

    if ($handle -eq [IntPtr]::Zero) {
        $errorCode = [Kernel32]::GetLastError()
        $errorMessage = Get-Win32ErrorMessage -ErrorCode $errorCode

        Write-Host "  FAILED to load!" -ForegroundColor Red
        Write-Host "  Error Code: $errorCode (0x$($errorCode.ToString('X8')))" -ForegroundColor Red
        Write-Host "  Error Message: $errorMessage" -ForegroundColor Red

        # Specific error code meanings
        switch ($errorCode) {
            126 { Write-Host "  >> Error 126: The specified module could not be found." -ForegroundColor Magenta
                  Write-Host "  >> This means a DEPENDENCY of this DLL is missing, not the DLL itself!" -ForegroundColor Magenta }
            127 { Write-Host "  >> Error 127: The specified procedure could not be found." -ForegroundColor Magenta }
            193 { Write-Host "  >> Error 193: Not a valid Win32 application (wrong architecture - 32-bit vs 64-bit mismatch)." -ForegroundColor Magenta }
        }

        return $false
    }

    Write-Host "  SUCCESS: Loaded successfully!" -ForegroundColor Green
    [Kernel32]::FreeLibrary($handle) | Out-Null
    return $true
}

# Test the DLLs in dependency order
$success = $true

# First test EuroScopePlugInDll.dll (no dependencies on our DLLs)
$euroScopeDll = Join-Path $BinDirectory "EuroScopePlugInDll.dll"
if (-not (Test-DllLoad -DllPath $euroScopeDll -DllName "EuroScopePlugInDll.dll")) {
    $success = $false
}

# Then test UKControllerPluginCore.dll (depends on EuroScopePlugInDll.dll)
$coreDll = Join-Path $BinDirectory "UKControllerPluginCore.dll"
if (-not (Test-DllLoad -DllPath $coreDll -DllName "UKControllerPluginCore.dll")) {
    $success = $false
}

# Summary
Write-Host "`n=== Summary ===" -ForegroundColor Cyan
if ($success) {
    Write-Host "All DLLs loaded successfully!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "One or more DLLs failed to load!" -ForegroundColor Red
    Write-Host "The error above indicates which dependency is missing." -ForegroundColor Red
    exit 1
}
