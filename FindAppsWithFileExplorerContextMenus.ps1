# Get all installed app packages
$AppPackages = Get-AppxPackage

# Initialize an array to store the names of apps using windows.fileExplorerContextMenus
$AppsUsingContextMenus = @()

# Iterate through all app packages
foreach ($AppPackage in $AppPackages) {
    # Fetch the AppxManifest.xml content for the current app package
    $AppxManifestPath = Join-Path $AppPackage.InstallLocation 'AppxManifest.xml'
    
    # Check if the AppxManifest.xml file exists
    if (Test-Path $AppxManifestPath) {
        # Load the AppxManifest.xml content as an XML object
        $AppxManifest = [xml](Get-Content $AppxManifestPath)

        # Check if windows.fileExplorerContextMenus is present in the XML content
        $UsesFileExplorerContextMenus = $AppxManifest.DocumentElement.OuterXml -match 'windows.fileExplorerContextMenus'

        # If the app uses windows.fileExplorerContextMenus, add its name to the array
        if ($UsesFileExplorerContextMenus) {
            $AppsUsingContextMenus += $AppPackage.Name
        }
    }
}

# Output the names of apps using windows.fileExplorerContextMenus
$AppsUsingContextMenus
