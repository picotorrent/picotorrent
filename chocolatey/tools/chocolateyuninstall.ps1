$app = Get-WmiObject -Class Win32_Product | Where-Object {$_.Name -eq 'PicoTorrent'}
  
if ($app) {
    $msiArgs = $('/x' + $app.IdentifyingNumber + ' /q REBOOT=ReallySuppress')
    Start-ChocolateyProcessAsAdmin $msiArgs 'msiexec'
}
