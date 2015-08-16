//////////////////////////////////////////////////////////////////////
// ARGUMENTS
//////////////////////////////////////////////////////////////////////

var target        = Argument("target", "Default");
var configuration = Argument("configuration", "Release");
var version       = Argument("version", System.IO.File.ReadAllText("VERSION").Trim());

// Parameters
var binDir   = Directory("./bin");
var buildDir = Directory("./build") + Directory(configuration);
var resDir   = Directory("./win32/res");
var baseName = string.Format("PicoTorrent-{0}", version);

//////////////////////////////////////////////////////////////////////
// TASKS
//////////////////////////////////////////////////////////////////////

Task("Clean")
    .Does(() =>
{
    CleanDirectory(binDir);
    CleanDirectory(buildDir);
});

Task("Generate-Project")
    .IsDependentOn("Clean")
    .Does(() =>
{
    var exitCode = StartProcess("cmake",
        new ProcessSettings
        {
            Arguments = ".. -G \"Visual Studio 14\" -T \"v140\"",
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

Task("Output-Lib-Files")
    .Does(() =>
{
    Zip("./lib", buildDir + File("lib.zip"));
});

Task("Output-Python-Runtime")
    .Does(() =>
{
    // Copy python27.zip and all .pyd files to buildDir
    CopyFiles(GetFiles("./deps/python27.zip"), buildDir);
    CopyFiles(GetFiles("./deps/python27/*.*"), buildDir);
});

Task("Create-Win32-Package")
    .IsDependentOn("Build")
    .IsDependentOn("Output-Lib-Files")
    .IsDependentOn("Output-Python-Runtime")
    .Does(() =>
{
    IEnumerable<FilePath> files = GetFiles(buildDir + File("PicoTorrent.exe"));
    files = files.Union(GetFiles(buildDir + File("_socket.pyd")));
    files = files.Union(GetFiles(buildDir + File("_ssl.pyd")));
    files = files.Union(GetFiles(buildDir + File("lib.zip")));
    files = files.Union(GetFiles(buildDir + File("python27.zip")));

    Zip(buildDir,
        binDir + File(baseName + ".zip"),
        files);
});

Task("Create-Win32-Installer")
    .IsDependentOn("Build")
    .IsDependentOn("Output-Lib-Files")
    .IsDependentOn("Output-Python-Runtime")
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
            OutputFile = binDir + File(baseName + ".msi"),
            RawArguments = "-sice:ICE91"
        });
    });

//////////////////////////////////////////////////////////////////////
// TASK TARGETS
//////////////////////////////////////////////////////////////////////

Task("Default")
    .IsDependentOn("Build")
    .IsDependentOn("Create-Win32-Package")
    .IsDependentOn("Create-Win32-Installer");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
