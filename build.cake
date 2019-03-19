#r "./tools/Cake.CMake/lib/net45/Cake.CMake.dll"

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

var SigningCertificate = EnvironmentVariable("PICO_SIGNING_CERTIFICATE");
var SigningPassword    = EnvironmentVariable("PICO_SIGNING_PASSWORD");
var SigningPublisher   = EnvironmentVariable("PICO_SIGNING_PUBLISHER") ?? "CN=PicoTorrent TESTING";

var Version            = System.IO.File.ReadAllText("VERSION").Trim();
var Installer          = string.Format("PicoTorrent-{0}-{1}.msi", Version, platform);
var InstallerBundle    = string.Format("PicoTorrent-{0}-{1}.exe", Version, platform);
var AppXPackage        = string.Format("PicoTorrent-{0}-{1}.appx", Version, platform);
var PortablePackage    = string.Format("PicoTorrent-{0}-{1}.zip", Version, platform);
var SymbolsPackage     = string.Format("PicoTorrent-{0}-{1}.symbols.zip", Version, platform);

var LibrarySuffix      = configuration == "Release" ? "" : "d";

public void SignFile(FilePath file, string description = "")
{
    Sign(file,
        new SignToolSignSettings
        {
            Description = description,
            CertPath = SigningCertificate,
            Password = SigningPassword,
            TimeStampUri = new Uri("http://timestamp.digicert.com")
        });
}

//////////////////////////////////////////////////////////////////////
// TASKS
//////////////////////////////////////////////////////////////////////

Task("Clean")
    .Does(() =>
{
    CleanDirectory(BuildDirectory);
});

Task("Generate-Project")
    .IsDependentOn("Clean")
    .Does(() =>
{
    var generator = "Visual Studio 15 2017 Win64";

    if(platform == "x86")
    {
        generator = "Visual Studio 15 2017";
    }

    CMake("./", new CMakeSettings {
      OutputPath = OutputDirectory,
      Generator = generator
    });
});

Task("Build")
    .IsDependentOn("Generate-Project")
    .Does(() =>
{
    var settings = new MSBuildSettings()
                        .SetConfiguration(configuration);

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

    MSBuild(OutputDirectory + File("PicoTorrent.sln"), settings);
});

Task("Setup-Publish-Directory")
    .IsDependentOn("Build")
    .Does(() =>
{
    var files = new FilePath[]
    {
        MakeAbsolute(BuildDirectory + File("PicoTorrent.exe")),
        MakeAbsolute(BuildDirectory + File("PicoTorrent.Core.dll")),
        MakeAbsolute(BuildDirectory + File("PicoTorrent.GeoIP.dll")),
        MakeAbsolute(BuildDirectory + File("PicoTorrent.Http.dll")),

        MakeAbsolute(BuildDirectory + File("Qt5Core.dll")),
        MakeAbsolute(BuildDirectory + File("Qt5Gui.dll")),
        MakeAbsolute(BuildDirectory + File("Qt5Svg.dll")),
        MakeAbsolute(BuildDirectory + File("Qt5Widgets.dll")),

        MakeAbsolute(BuildDirectory + Directory("imageformats") + File("qico.dll")),
        MakeAbsolute(BuildDirectory + Directory("platforms")    + File("qwindows.dll")),
        MakeAbsolute(BuildDirectory + Directory("styles")       + File("qwindowsvistastyle.dll")),
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
    var UseBuildToolsPaths  = true;
    var VCRedistPath        = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\\Redist\\MSVC";
    var VCRedistVersionFile = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\\Auxiliary\\Build\\Microsoft.VCRedistVersion.default.txt";

    if (!System.IO.File.Exists(VCRedistVersionFile))
    {
        VCRedistPath        = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Redist\\MSVC";
        VCRedistVersionFile = "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\Microsoft.VCRedistVersion.default.txt";
    }

    var VCRedistVersion = System.IO.File.ReadAllText(VCRedistVersionFile).Trim();
    var VCRedist = Directory(VCRedistPath)
                 + Directory(VCRedistVersion)
                 + Directory(platform)
                 + Directory("Microsoft.VC141.CRT");

    var CRTRedist = Directory("C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\ucrt\\DLLs")
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
        .WithToken("Version", Version + ".0")
        .Save("./packaging/AppX/PicoTorrentManifest.xml");

    var argsBuilder = new ProcessArgumentBuilder();

    argsBuilder.Append("pack");
    argsBuilder.Append("/f {0}", MakeAbsolute(File("./packaging/AppX/PicoTorrent.mapping")));
    argsBuilder.Append("/p {0}", MakeAbsolute(PackagesDirectory + File(AppXPackage)));

    var makeAppXTool = "C:\\Program Files (x86)\\Windows Kits\\10\\bin\\10.0.17763.0\\x86\\makeappx.exe";
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
            { "Version", Version }
        },
        OutputDirectory = BuildDirectory
    });

    WiXLight(objFiles, new LightSettings
    {
        OutputFile = PackagesDirectory + File(Installer)
    });
});

Task("Build-Installer-Bundle")
    .IsDependentOn("Build")
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
            { "Version", Version }
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
        BuildDirectory + File("PicoTorrent.Core.pdb"),
        BuildDirectory + File("PicoTorrent.GeoIP.pdb"),
        BuildDirectory + File("PicoTorrent.Http.pdb"),
    };

    Zip(BuildDirectory, PackagesDirectory + File(SymbolsPackage), files);
});

Task("Build-Chocolatey-Package")
    .IsDependentOn("Build-Installer")
    .Does(() =>
{
    TransformTextFile("./packaging/Chocolatey/tools/chocolateyinstall.ps1.template", "%{", "}")
        .WithToken("Installer", InstallerBundle)
        .WithToken("Version", Version)
        .Save("./packaging/Chocolatey/tools/chocolateyinstall.ps1");

    var currentDirectory = MakeAbsolute(Directory("."));
    var cd = MakeAbsolute(PackagesDirectory);
    var nuspec = MakeAbsolute(File("./packaging/Chocolatey/picotorrent.nuspec"));

    System.IO.Directory.SetCurrentDirectory(cd.ToString());

    ChocolateyPack(nuspec, new ChocolateyPackSettings
    {
        Version = Version
    });

    System.IO.Directory.SetCurrentDirectory(currentDirectory.ToString());
});

Task("Sign")
    .IsDependentOn("Build")
    .WithCriteria(() => SigningCertificate != null && SigningPassword != null)
    .Does(() =>
{
    SignFile(BuildDirectory + File("PicoTorrent.exe"), "PicoTorrent");
});

Task("Sign-Installer")
    .IsDependentOn("Build-Installer")
    .WithCriteria(() => SigningCertificate != null && SigningPassword != null)
    .Does(() =>
{
    var file = PackagesDirectory + File(Installer);
    SignFile(file);
});

Task("Sign-Installer-Bundle")
    .IsDependentOn("Build-Installer-Bundle")
    .WithCriteria(() => SigningCertificate != null && SigningPassword != null)
    .Does(() =>
{
    var bundle = PackagesDirectory + File(InstallerBundle);
    var insignia = Directory("tools")
                   + Directory("WiX")
                   + Directory("tools")
                   + File("insignia.exe");

    // Detach Burn engine
    StartProcess(insignia, "-ib \"" + bundle + "\" -o build/BurnEngine.exe");
    SignFile("build/BurnEngine.exe");
    StartProcess(insignia, "-ab build/BurnEngine.exe \"" + bundle + "\" -o \"" + bundle + "\"");

    // Sign the bundle
    SignFile(bundle);
});

//////////////////////////////////////////////////////////////////////
// TASK TARGETS
//////////////////////////////////////////////////////////////////////

Task("Default")
    .IsDependentOn("Build")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Build-Installer-Bundle")
    .IsDependentOn("Build-AppX-Package")
    .IsDependentOn("Build-Chocolatey-Package")
    .IsDependentOn("Build-Portable-Package")
    .IsDependentOn("Build-Symbols-Package")
    ;

Task("Publish")
    .IsDependentOn("Build")
    .IsDependentOn("Sign")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Build-Installer-Bundle")
    .IsDependentOn("Sign-Installer")
    .IsDependentOn("Sign-Installer-Bundle")
    .IsDependentOn("Build-AppX-Package")
    .IsDependentOn("Build-Chocolatey-Package")
    .IsDependentOn("Build-Portable-Package")
    .IsDependentOn("Build-Symbols-Package");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
