$ErrorActionPreference = 'Stop'

$local_key       = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\*'
$machine_key     = 'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*'
$machine_key6432 = 'HKLM:\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*'

$key = Get-ItemProperty -Path @($machine_key6432, $machine_key, $local_key) `
                        -ErrorAction SilentlyContinue `
                        | ? { $_.DisplayName -like "PicoTorrent*" }

if ($key.Count -eq 1) {
  $key | % {
    Uninstall-ChocolateyPackage -PackageName "picotorrent" `
                                -FileType "EXE" `
                                -SilentArgs "/quiet" `
                                -ValidExitCodes @(0) `
                                -File $_.UninstallString
  }
} elseif ($key.Count -eq 0) {
  Write-Warning "PicoTorrent has already been uninstalled by other means."
} elseif ($key.Count -gt 1) {
  Write-Warning "$key.Count matches found!"
  Write-Warning "To prevent accidental data loss, no programs will be uninstalled."
  Write-Warning "Please alert package maintainer the following keys were matched:"
  $key | % {Write-Warning "- $_.DisplayName"}
}
