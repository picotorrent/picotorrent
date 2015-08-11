//////////////////////////////////////////////////////////////////////
// ARGUMENTS
//////////////////////////////////////////////////////////////////////

var target        = Argument("target", "Default");
var configuration = Argument("configuration", "Release");
var version       = Argument("version", System.IO.File.ReadAllText("VERSION").Trim());

// Parameters
var buildDir = Directory("./build") + Directory(configuration);
var resDir   = Directory("./win32/res");

//////////////////////////////////////////////////////////////////////
// TASKS
//////////////////////////////////////////////////////////////////////

Task("Clean")
    .Does(() =>
{
    CleanDirectory(buildDir);
});

Task("Generate-Project")
    .IsDependentOn("Clean")
    .Does(() =>
{
    CreateDirectory("build");

    var exitCode = StartProcess("cmake",
        new ProcessSettings
        {
            Arguments = ".. -G \"Visual Studio 14\" -T \"v140_xp\"",
            WorkingDirectory = "./build"
        });

    if(exitCode != 0)
    {
        throw new CakeException("Failed to run CMake. Exit code: " + exitCode);
    }
});

Task("Build")
    .IsDependentOn("Generate-Project")
    .Does(() =>
{
    MSBuild("./build/PicoTorrent.sln",
        settings => settings.SetConfiguration(configuration));
});

Task("Create-Win32-Installer")
    .IsDependentOn("Build")
    .Does(() =>
    {
        WiXCandle("./win32/installer/**/*.wxs", new CandleSettings
        {
            Defines = new Dictionary<string, string>
            {
                { "BinariesDirectory",  buildDir },
                { "BuildVersion",       version },
                { "ResourcesDirectory", resDir }
            },
            OutputDirectory = "./build/wixobj"
        });

        WiXLight("./build/wixobj/*.wixobj", new LightSettings
        {
            OutputFile = buildDir + File("PicoTorrent.msi"),
            RawArguments = "-sice:ICE91"
        });
    });

//////////////////////////////////////////////////////////////////////
// TASK TARGETS
//////////////////////////////////////////////////////////////////////

Task("Default")
    .IsDependentOn("Build")
    .IsDependentOn("Create-Win32-Installer");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
