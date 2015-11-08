#r "./tools/Cake.CMake/lib/net45/Cake.CMake.dll"

//////////////////////////////////////////////////////////////////////
// ARGUMENTS
//////////////////////////////////////////////////////////////////////

var target        = Argument("target", "Default");
var configuration = Argument("configuration", "Release");

// Parameters
var BuildDirectory     = Directory("./build") + Directory(configuration);
var ResourceDirectory  = Directory("./res");
var SigningCertificate = EnvironmentVariable("PICO_SIGNING_CERTIFICATE");
var SigningPassword    = EnvironmentVariable("PICO_SIGNING_PASSWORD");
var Version            = System.IO.File.ReadAllText("VERSION").Trim();

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
    CreateDirectory("build");

    CMake("./", new CMakeSettings {
      OutputPath = "./build",
      Generator = "Visual Studio 14 Win64",
      Toolset = "v140"
    });
});

Task("Build")
    .IsDependentOn("Generate-Project")
    .Does(() =>
{
    MSBuild("./build/PicoTorrent.sln",
        settings => settings.SetConfiguration(configuration));
});

Task("Build-Installer")
    .IsDependentOn("Build")
    .Does(() =>
{
    WiXCandle("./installer/PicoTorrent.wxs", new CandleSettings
    {
        Architecture = Architecture.X64,
        Defines = new Dictionary<string, string>
        {
            { "BuildDirectory", BuildDirectory },
            { "ResourceDirectory", ResourceDirectory },
            { "Version", Version }
        },
        OutputDirectory = BuildDirectory
    });

    WiXLight(BuildDirectory + File("PicoTorrent.wixobj"), new LightSettings
    {
        OutputFile = BuildDirectory + File("PicoTorrent.msi")
    });
});

Task("Sign")
    .IsDependentOn("Build")
    .WithCriteria(() => SigningCertificate != null && SigningPassword != null)
    .Does(() =>
{
    var file = BuildDirectory + File("PicoTorrent.exe");
    SignTool(file);
});

Task("Sign-Installer")
    .IsDependentOn("Build-Installer")
    .WithCriteria(() => SigningCertificate != null && SigningPassword != null)
    .Does(() =>
{
    var file = BuildDirectory + File("PicoTorrent.msi");
    SignTool(file);
});

//////////////////////////////////////////////////////////////////////
// TASK TARGETS
//////////////////////////////////////////////////////////////////////

Task("Default")
    .IsDependentOn("Build-Installer");

Task("Publish")
    .IsDependentOn("Build")
    .IsDependentOn("Sign")
    .IsDependentOn("Build-Installer")
    .IsDependentOn("Sign-Installer");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
