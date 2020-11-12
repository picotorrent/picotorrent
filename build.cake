#addin "nuget:?package=Cake.CMake&version=1.2.0"
#tool  "nuget:?package=GitVersion.CommandLine&version=5.3.7"
#tool  "nuget:?package=WiX&version=3.11.2"

//////////////////////////////////////////////////////////////////////
// ARGUMENTS
//////////////////////////////////////////////////////////////////////

var target        = Argument("target", "Default");
var configuration = Argument("configuration", "Release");
var platform      = Argument("platform", "x64");

// Parameters
var OutputDirectory    = Directory("./build-" + platform);
var BuildDirectory     = OutputDirectory + Directory(configuration);
var PackagesDirectory  = BuildDirectory + Directory("packages");
var PublishDirectory   = BuildDirectory + Directory("publish");
var ResourceDirectory  = Directory("./res");

var Version            = GitVersion();
var AppXPackage        = string.Format("PicoTorrent-{0}-{1}.appx", Version.SemVer, platform);
var Installer          = string.Format("PicoTorrent-{0}-{1}.msi", Version.SemVer, platform);
var InstallerBundle    = string.Format("PicoTorrent-{0}-{1}.exe", Version.SemVer, platform);
var PortablePackage    = string.Format("PicoTorrent-{0}-{1}.zip", Version.SemVer, platform);
var SigningPublisher   = EnvironmentVariable("PICO_SIGNING_PUBLISHER") ?? "CN=PicoTorrent TESTING";
var SymbolsPackage     = string.Format("PicoTorrent-{0}-{1}.symbols.zip", Version.SemVer, platform);

//////////////////////////////////////////////////////////////////////
// TASKS
//////////////////////////////////////////////////////////////////////

Task("Clean")
    .Does(() =>
{
    CleanDirectory(BuildDirectory);
    CleanDirectory(PackagesDirectory);
    CleanDirectory(PublishDirectory);
});

Task("Generate-Project")
    .IsDependentOn("Clean")
    .Does(() =>
{
    CMake(new CMakeSettings
    {
        SourcePath = ".",
        OutputPath = OutputDirectory,
        Generator = "Visual Studio 16 2019",
        Platform = platform == "x86" ? "Win32" : "x64",
        Toolset = "v142"
    });
});

Task("Build")
    .IsDependentOn("Generate-Project")
    .Does(() =>
{
    var settings = new MSBuildSettings()
        .SetConfiguration(configuration)
        .SetMaxCpuCount(0)
        .UseToolVersion(MSBuildToolVersion.VS2019);

    if(platform == "x86")
    {
        settings.WithProperty("Platform", "Win32")
                .SetPlatformTarget(PlatformTarget.x86);
    }
    else
    {
        settings.WithProperty("Platform", "x64")
                .SetPlatformTarget(PlatformTarget.x64);
    }

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
        MakeAbsolute(BuildDirectory + File("crashpad_handler.exe")),
        MakeAbsolute(BuildDirectory + File("Plugin_Updater.dll")),
    };

    CreateDirectory(PublishDirectory);

    CopyFiles(
        files,            // Source
        PublishDirectory, // Target
        true);            // Preserve folder structure
});

Task("Build-AppX-Package")
    .IsDependentOn("Build")
    .IsDependentOn("Setup-Publish-Directory")
    .Does(() =>
{
    var VCRedistPath        = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Redist\\MSVC";
    var VCRedistVersionFile = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Auxiliary\\Build\\Microsoft.VCRedistVersion.default.txt";

    if (!System.IO.File.Exists(VCRedistVersionFile))
    {
        VCRedistPath        = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Redist\\MSVC";
        VCRedistVersionFile = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\Microsoft.VCRedistVersion.default.txt";
    }

    var VCRedistVersion = System.IO.File.ReadAllText(VCRedistVersionFile).Trim();
    var VCRedist = Directory(VCRedistPath)
                 + Directory(VCRedistVersion)
                 + Directory(platform)
                 + Directory("Microsoft.VC142.CRT");

    var CRTRedist = Directory("C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\10.0.18362.0\\ucrt\\DLLs")
                  + Directory(platform);

    TransformTextFile("./packaging/AppX/PicoTorrent.mapping.template", "%{", "}")
        .WithToken("VCDir", VCRedist)
        .WithToken("CRTDir", CRTRedist)
        .WithToken("PublishDirectory", MakeAbsolute(PublishDirectory))
        .WithToken("ResourceDirectory", MakeAbsolute(ResourceDirectory))
        .WithToken("PackagingDirectory", MakeAbsolute(Directory("./packaging/AppX")))
        .Save("./packaging/AppX/PicoTorrent.mapping");

    TransformTextFile("./packaging/AppX/PicoTorrentManifest.xml.template", "%{", "}")
        .WithToken("Platform", platform)
        .WithToken("Publisher", SigningPublisher)
        .WithToken("Version", Version.MajorMinorPatch + ".0")
        .Save("./packaging/AppX/PicoTorrentManifest.xml");

    var argsBuilder = new ProcessArgumentBuilder();

    argsBuilder.Append("pack");
    argsBuilder.Append("/f {0}", File("./packaging/AppX/PicoTorrent.mapping"));
    argsBuilder.Append("/p {0}", PackagesDirectory + File(AppXPackage));

    var makeAppXTool = "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.18362.0\\x86\\makeappx.exe";
    int exitCode = StartProcess(makeAppXTool, new ProcessSettings
    {
        Arguments = argsBuilder
    });

    if(exitCode != 0)
    {
        throw new CakeException("makeappx.exe exited with error: " + exitCode);
    }
});

Task("Build-Installer")
    .IsDependentOn("Build")
    .IsDependentOn("Setup-Publish-Directory")
    .Does(() =>
{
    var arch = Architecture.X64;

    if(platform == "x86")
    {
        arch = Architecture.X86;
    }

    var sourceFiles = new FilePath[]
    {
        "./packaging/WiX/PicoTorrent.wxs",
        "./packaging/WiX/PicoTorrent.Components.wxs",
        "./packaging/WiX/PicoTorrent.Directories.wxs"
    };

    var objFiles = new FilePath[]
    {
        BuildDirectory + File("PicoTorrent.wixobj"),
        BuildDirectory + File("PicoTorrent.Components.wixobj"),
        BuildDirectory + File("PicoTorrent.Directories.wixobj")
    };

    WiXCandle(sourceFiles, new CandleSettings
    {
        Architecture = arch,
        Defines = new Dictionary<string, string>
        {
            { "Configuration", configuration },
            { "PublishDirectory", PublishDirectory },
            { "Platform", platform },
            { "ResourceDirectory", ResourceDirectory },
            { "Version", Version.MajorMinorPatch }
        },
        Extensions = new [] { "WixUtilExtension" },
        OutputDirectory = BuildDirectory
    });

    WiXLight(objFiles, new LightSettings
    {
        Extensions = new [] { "WixUtilExtension" },
        OutputFile = PackagesDirectory + File(Installer)
    });
});

Task("Build-Installer-Bundle")
    .IsDependentOn("Build-Installer")
    .Does(() =>
{
    var arch = Architecture.X64;

    if(platform == "x86")
    {
        arch = Architecture.X86;
    }

    WiXCandle("./packaging/WiX/PicoTorrentBundle.wxs", new CandleSettings
    {
        Architecture = arch,
        Extensions = new [] { "WixBalExtension", "WixNetFxExtension", "WixUtilExtension" },
        Defines = new Dictionary<string, string>
        {
            { "PicoTorrentInstaller", PackagesDirectory + File(Installer) },
            { "Platform", platform },
            { "Version", Version.MajorMinorPatch }
        },
        OutputDirectory = BuildDirectory
    });

    WiXLight(BuildDirectory + File("PicoTorrentBundle.wixobj"), new LightSettings
    {
        Extensions = new [] { "WixBalExtension", "WixNetFxExtension", "WixUtilExtension" },
        OutputFile = PackagesDirectory + File(InstallerBundle)
    });
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
    .IsDependentOn("Build-AppX-Package")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Build-Installer-Bundle")
    .IsDependentOn("Build-Portable-Package")
    .IsDependentOn("Build-Symbols-Package");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
