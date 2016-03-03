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
var PublishDirectory   = BuildDirectory + Directory("publish");
var RuntimeDirectory   = BuildDirectory + Directory("runtime");
var ResourceDirectory  = Directory("./res");
var SigningCertificate = EnvironmentVariable("PICO_SIGNING_CERTIFICATE");
var SigningPassword    = EnvironmentVariable("PICO_SIGNING_PASSWORD");
var Version            = System.IO.File.ReadAllText("VERSION").Trim();
var Installer          = string.Format("PicoTorrent-{0}-{1}.msi", Version, platform);
var InstallerBundle    = string.Format("PicoTorrent-{0}-{1}.exe", Version, platform);
var PortablePackage    = string.Format("PicoTorrent-{0}-{1}.zip", Version, platform);
var RuntimePackage     = string.Format("PicoTorrent-{0}-{1}.runtime.zip", Version, platform);
var SymbolsPackage     = string.Format("PicoTorrent-{0}-{1}.symbols.zip", Version, platform);

public void SignTool(FilePath file)
{
    var signTool = "C:\\Program Files (x86)\\Windows Kits\\8.1\\bin\\x64\\signtool.exe";
    var argsBuilder = new ProcessArgumentBuilder();

    argsBuilder.Append("sign");
    argsBuilder.Append("/d {0}", "PicoTorrent");
    argsBuilder.Append("/f {0}", SigningCertificate);
    argsBuilder.AppendSecret("/p {0}", SigningPassword);
    argsBuilder.Append("/t http://timestamp.digicert.com");
    argsBuilder.AppendQuoted("{0}", file);

    int exitCode = StartProcess(signTool, new ProcessSettings
    {
        Arguments = argsBuilder
    });

    if(exitCode != 0)
    {
        throw new CakeException("SignTool exited with error: " + exitCode);
    }
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
    var generator = "Visual Studio 14 Win64";

    if(platform == "x86")
    {
        generator = "Visual Studio 14";
    }

    CMake("./", new CMakeSettings {
      OutputPath = OutputDirectory,
      Generator = generator,
      Toolset = "v140"
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
        BuildDirectory + File("PicoTorrent.exe"),
        BuildDirectory + File("PicoTorrentCore.dll")
    };

    CreateDirectory(PublishDirectory);
    CopyFiles(files, PublishDirectory);
    CopyDirectory(Directory("lang"), PublishDirectory + Directory("lang"));
    DeleteFile(PublishDirectory + Directory("lang") + File("1033.json"));
});

Task("Setup-Runtime-Directory")
    .Does(() =>
{
    var VCRedist = Directory("C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\redist");
    var VCDir = VCRedist + Directory(platform) + Directory("Microsoft.VC140.CRT");

    var CRTRedist = Directory("C:\\Program Files (x86)\\Windows Kits\\10\\Redist\\ucrt\\DLLs");
    var CRTDir = CRTRedist + Directory(platform);

    var files = new FilePath[]
    {
        VCDir + File("msvcp140.dll"),
        VCDir + File("vcruntime140.dll"),
        CRTDir + File("api-ms-win-core-file-l1-2-0.dll"),
        CRTDir + File("api-ms-win-core-file-l2-1-0.dll"),
        CRTDir + File("api-ms-win-core-localization-l1-2-0.dll"),
        CRTDir + File("api-ms-win-core-processthreads-l1-1-1.dll"),
        CRTDir + File("api-ms-win-core-synch-l1-2-0.dll"),
        CRTDir + File("api-ms-win-core-timezone-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-conio-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-convert-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-environment-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-filesystem-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-heap-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-locale-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-math-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-multibyte-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-runtime-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-stdio-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-string-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-time-l1-1-0.dll"),
        CRTDir + File("api-ms-win-crt-utility-l1-1-0.dll"),
        CRTDir + File("ucrtbase.dll")
    };

    CreateDirectory(RuntimeDirectory);
    CopyFiles(files, RuntimeDirectory);
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

    WiXCandle("./installer/PicoTorrent.wxs", new CandleSettings
    {
        Architecture = arch,
        Defines = new Dictionary<string, string>
        {
            { "PublishDirectory", PublishDirectory },
            { "Platform", platform },
            { "ResourceDirectory", ResourceDirectory },
            { "Version", Version }
        },
        OutputDirectory = BuildDirectory
    });

    WiXLight(BuildDirectory + File("PicoTorrent.wixobj"), new LightSettings
    {
        OutputFile = BuildDirectory + File(Installer)
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

    WiXCandle("./installer/PicoTorrentBundle.wxs", new CandleSettings
    {
        Architecture = arch,
        Extensions = new [] { "WixBalExtension", "WixUtilExtension" },
        Defines = new Dictionary<string, string>
        {
            { "PicoTorrentInstaller", BuildDirectory + File(Installer) },
            { "Platform", platform },
            { "Version", Version }
        },
        OutputDirectory = BuildDirectory
    });

    WiXLight(BuildDirectory + File("PicoTorrentBundle.wixobj"), new LightSettings
    {
        Extensions = new [] { "WixBalExtension", "WixUtilExtension" },
        OutputFile = BuildDirectory + File(InstallerBundle)
    });
});

Task("Build-Portable-Package")
    .IsDependentOn("Build")
    .IsDependentOn("Setup-Publish-Directory")
    .Does(() =>
{
    Zip(PublishDirectory, BuildDirectory + File(PortablePackage));
});

Task("Build-Runtime-Package")
    .IsDependentOn("Setup-Runtime-Directory")
    .Does(() =>
{
    Zip(RuntimeDirectory, BuildDirectory + File(RuntimePackage));
});

Task("Build-Symbols-Package")
    .IsDependentOn("Build")
    .Does(() =>
{
    var files = new FilePath[]
    {
        BuildDirectory + File("PicoTorrent.pdb"),
        BuildDirectory + File("PicoTorrentCore.pdb")
    };

    Zip(BuildDirectory, BuildDirectory + File(SymbolsPackage), files);
});

Task("Build-Chocolatey-Package")
    .IsDependentOn("Build-Installer")
    .Does(() =>
{
    TransformTextFile("./chocolatey/tools/chocolateyinstall.ps1.template", "%{", "}")
        .WithToken("Installer", InstallerBundle)
        .WithToken("Version", Version)
        .Save("./chocolatey/tools/chocolateyinstall.ps1");

    var currentDirectory = MakeAbsolute(Directory("."));
    var cd = MakeAbsolute(BuildDirectory);
    var nuspec = MakeAbsolute(File("./chocolatey/picotorrent.nuspec"));

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
    SignTool(BuildDirectory + File("PicoTorrent.exe"));
    SignTool(BuildDirectory + File("PicoTorrentCore.dll"));
});

Task("Sign-Installer")
    .IsDependentOn("Build-Installer")
    .WithCriteria(() => SigningCertificate != null && SigningPassword != null)
    .Does(() =>
{
    var file = BuildDirectory + File(Installer);
    SignTool(file);
});

Task("Sign-Installer-Bundle")
    .IsDependentOn("Build-Installer-Bundle")
    .WithCriteria(() => SigningCertificate != null && SigningPassword != null)
    .Does(() =>
{
    var bundle = BuildDirectory + File(InstallerBundle);
    var insignia = Directory("tools")
                   + Directory("WiX.Toolset")
                   + Directory("tools")
                   + Directory("wix")
                   + File("insignia.exe");

    // Detach Burn engine
    StartProcess(insignia, "-ib \"" + bundle + "\" -o build/BurnEngine.exe");
    SignTool("build/BurnEngine.exe");
    StartProcess(insignia, "-ab build/BurnEngine.exe \"" + bundle + "\" -o \"" + bundle + "\"");

    // Sign the bundle
    SignTool(bundle);
});

//////////////////////////////////////////////////////////////////////
// TASK TARGETS
//////////////////////////////////////////////////////////////////////

Task("Default")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Build-Installer-Bundle")
    .IsDependentOn("Build-Chocolatey-Package")
    .IsDependentOn("Build-Portable-Package")
    .IsDependentOn("Build-Symbols-Package")
    .IsDependentOn("Build-Runtime-Package");

Task("Publish")
    .IsDependentOn("Build")
    .IsDependentOn("Sign")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Build-Installer-Bundle")
    .IsDependentOn("Sign-Installer")
    .IsDependentOn("Sign-Installer-Bundle")
    .IsDependentOn("Build-Chocolatey-Package")
    .IsDependentOn("Build-Portable-Package")
    .IsDependentOn("Build-Symbols-Package")
    .IsDependentOn("Build-Runtime-Package");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
