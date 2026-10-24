#addin "nuget:?package=Cake.CMake&version=1.4.0"
#tool  "dotnet:?package=GitVersion.Tool&version=6.8.2"
#tool  "nuget:?package=WixToolset.Util.wixext&version=5.0.2"
#tool  "nuget:?package=WixToolset.Firewall.wixext&version=5.0.2"
#tool  "nuget:?package=WixToolset.Netfx.wixext&version=5.0.2"

//////////////////////////////////////////////////////////////////////
// ARGUMENTS
//////////////////////////////////////////////////////////////////////

var target        = Argument("target", "Default");
var configuration = Argument("configuration", "Release");
var platform      = Argument("platform", "x64");
var vcpkgTriplet  = Argument("vcpkg-triplet", "x64-windows-static-md");

// Parameters
var OutputDirectory       = Directory("./build-" + platform);
var BuildDirectory        = OutputDirectory + Directory(configuration);
var PackagesDirectory     = BuildDirectory + Directory("packages");
var PublishDirectory      = BuildDirectory + Directory("publish");
var BootstrapperDirectory = Directory("./src/installer/bin") + Directory(configuration);
var ResourceDirectory     = Directory("./res");

var Version            = GitVersion();
var Installer          = string.Format("PicoTorrent-{0}-{1}.msi", Version.SemVer, platform);
var InstallerBundle    = string.Format("PicoTorrent-{0}-{1}.exe", Version.SemVer, platform);
var PortablePackage    = string.Format("PicoTorrent-{0}-{1}.zip", Version.SemVer, platform);
var SymbolsPackage     = string.Format("PicoTorrent-{0}-{1}.symbols.zip", Version.SemVer, platform);

var WixExtDir          = Directory("./tools");
var UtilWixExt         = MakeAbsolute(WixExtDir + File("WixToolset.Util.wixext.5.0.2/wixext5/WixToolset.Util.wixext.dll")).FullPath;
var FirewallWixExt     = MakeAbsolute(WixExtDir + File("WixToolset.Firewall.wixext.5.0.2/wixext5/WixToolset.Firewall.wixext.dll")).FullPath;
var NetfxWixExt        = MakeAbsolute(WixExtDir + File("WixToolset.Netfx.wixext.5.0.2/wixext5/WixToolset.Netfx.wixext.dll")).FullPath;

void WixBuild(string arguments)
{
    var exitCode = StartProcess("dotnet", new ProcessSettings
    {
        Arguments = "wix build " + arguments
    });

    if (exitCode != 0)
    {
        throw new Exception($"wix build failed with exit code {exitCode}");
    }
}

//////////////////////////////////////////////////////////////////////
// TASKS
//////////////////////////////////////////////////////////////////////

Task("Clean")
    .Does(() =>
{
    CleanDirectory(BootstrapperDirectory);
    CleanDirectory(BuildDirectory);
});

Task("Generate-Project")
    .IsDependentOn("Clean")
    .Does(() =>
{
    CMake(new CMakeSettings
    {
        SourcePath = ".",
        OutputPath = OutputDirectory,
        Generator = "Visual Studio 18 2026",
        Platform = "x64",
        Toolset = "v145",
        Options = new []
        {
            $"-DGITVERSION_VAR_BRANCHNAME={Version.BranchName}",
            $"-DGITVERSION_VAR_SEMVER={Version.SemVer}",
            $"-DGITVERSION_VAR_SHORTSHA={Version.Sha.Substring(0,7)}",
            $"-DGITVERSION_VAR_VERSION_MAJOR={Version.Major}",
            $"-DGITVERSION_VAR_VERSION_MINOR={Version.Minor}",
            $"-DGITVERSION_VAR_VERSION_PATCH={Version.Patch}",
            $"-DGITVERSION_VAR_VERSION={Version.MajorMinorPatch}",
            $"-DVCPKG_TARGET_TRIPLET={vcpkgTriplet}"
        }
    });
});

Task("Build")
    .IsDependentOn("Generate-Project")
    .Does(() =>
{
    var settings = new MSBuildSettings()
        .SetConfiguration(configuration)
        .SetMaxCpuCount(0)
        .UseToolVersion(MSBuildToolVersion.Default);
    settings.WithProperty("Platform", "x64")
            .SetPlatformTarget(PlatformTarget.x64);

    MSBuild(OutputDirectory + File("PicoTorrent.vcxproj"), settings);

    // Plugins
    MSBuild(OutputDirectory + File("Plugin_Updater.vcxproj"), settings);
});

Task("Setup-Publish-Directory")
    .IsDependentOn("Build")
    .Does(() =>
{
    var files = new FilePath[]
    {
        MakeAbsolute(BuildDirectory + File("PicoTorrent.exe")),
        MakeAbsolute(BuildDirectory + File("coredb.sqlite")),
        MakeAbsolute(BuildDirectory + File("Plugin_Updater.dll")),
    };

    CreateDirectory(PublishDirectory);

    CopyFiles(
        files,            // Source
        PublishDirectory, // Target
        true);            // Preserve folder structure
});

Task("Build-Installer")
    .IsDependentOn("Build")
    .IsDependentOn("Setup-Publish-Directory")
    .Does(() =>
{
    WixBuild(string.Join(" ",
        "-platform x64",
        $"-d Platform={platform}",
        $"-d Configuration={configuration}",
        $"-d PublishDirectory={MakeAbsolute(PublishDirectory).FullPath}",
        $"-d ResourceDirectory={MakeAbsolute(ResourceDirectory).FullPath}",
        $"-d Version={Version.MajorMinorPatch}",
        $"-ext \"{UtilWixExt}\"",
        $"-ext \"{FirewallWixExt}\"",
        "\"packaging/WiX/PicoTorrent.wxs\"",
        "\"packaging/WiX/PicoTorrent.Components.wxs\"",
        "\"packaging/WiX/PicoTorrent.Directories.wxs\"",
        $"-o \"{MakeAbsolute(PackagesDirectory + File(Installer)).FullPath}\""));
});

Task("Build-Installer-Bootstrapper")
    .Does(() =>
{
    var exitCode = StartProcess("dotnet", new ProcessSettings
    {
        Arguments = $"build \"./src/installer/PicoTorrentBootstrapper.csproj\" -c {configuration}"
    });

    if (exitCode != 0)
    {
        throw new Exception($"dotnet build failed with exit code {exitCode}");
    }
});

Task("Build-Installer-Bundle")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Build-Installer-Bootstrapper")
    .Does(() =>
{
    var baDir = MakeAbsolute(Directory("./src/installer/bin") + Directory(configuration)).FullPath;

    WixBuild(string.Join(" ",
        "-platform x64",
        $"-d Platform={platform}",
        $"-d Version={Version.MajorMinorPatch}",
        $"-d PicoTorrentInstaller={MakeAbsolute(PackagesDirectory + File(Installer)).FullPath}",
        $"-d BootstrapperApplication={baDir}/PicoTorrentBootstrapper.exe",
        $"-d BootstrapperApplicationDir={baDir}",
        $"-ext \"{UtilWixExt}\"",
        $"-ext \"{NetfxWixExt}\"",
        "\"packaging/WiX/PicoTorrentBundle.wxs\"",
        $"-o \"{MakeAbsolute(PackagesDirectory + File(InstallerBundle)).FullPath}\""));
});

Task("Build-Portable-Package")
    .IsDependentOn("Build")
    .IsDependentOn("Setup-Publish-Directory")
    .Does(() =>
{
    Zip(PublishDirectory, PackagesDirectory + File(PortablePackage));
});

Task("Build-Symbols-Package")
    .IsDependentOn("Build")
    .Does(() =>
{
    var files = new FilePath[]
    {
        BuildDirectory + File("PicoTorrent.pdb"),
    };

    Zip(BuildDirectory, PackagesDirectory + File(SymbolsPackage), files);
});

//////////////////////////////////////////////////////////////////////
// TASK TARGETS
//////////////////////////////////////////////////////////////////////

Task("Default")
    .IsDependentOn("Build")
    ;

Task("Publish")
    .IsDependentOn("Build")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Build-Installer-Bundle")
    .IsDependentOn("Build-Portable-Package")
    .IsDependentOn("Build-Symbols-Package");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
