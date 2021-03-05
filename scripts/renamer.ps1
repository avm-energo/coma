$path = $args[0]
cd $path
$build = $args[1]
$name="-"+"$build"+".exe"
Write-Output $name
Get-Item .\*.* | ForEach-Object { Rename-Item $_ ($_.Name -replace ".exe", $name)}