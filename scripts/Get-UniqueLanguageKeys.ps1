
$lang = Get-Content lang/en-US.json
    | ConvertFrom-Json -AsHashtable

$out=@{}

Get-ChildItem -Recurse src/picotorrent
    | Select-String -Pattern 'i18n\(\"([a-zA-Z0-9_]+?)\"\)' -AllMatches
    | ForEach-Object { $_.Matches }
    | ForEach-Object { $_.Groups[1].Value }
    | Sort-Object
    | Get-Unique
    | ForEach-Object { Select-Object @{ "$_"="$lang[$_]" } }
