//////////////////////////////////////////////////////////////////////
// ARGUMENTS
//////////////////////////////////////////////////////////////////////

var target        = Argument("target", "Default");
var configuration = Argument("configuration", "Release");
var version       = Argument("version", System.IO.File.ReadAllText("VERSION").Trim());

// Parameters
var binDir   = Directory("./bin");
var depsDir  = Directory("./deps");
var buildDir = Directory("./build") + Directory(configuration);
var resDir   = Directory("./win32/res");
var pyDir    = Directory("C:/Python34");
var baseName = string.Format("PicoTorrent-{0}", version);

//////////////////////////////////////////////////////////////////////
// TASKS
//////////////////////////////////////////////////////////////////////

Task("Clean")
    .Does(() =>
{
    CleanDirectory(binDir);
    CleanDirectory(buildDir);
    CleanDirectory(depsDir);
    CleanDirectory(depsDir + Directory("python34"));
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

Task("Prepare-Python-Runtime")
    .Does(() =>
{
    // Zip all files in the Python `Lib` directory (and skip __pycache__ dirs).
    Zip(pyDir + Directory("Lib"),
        "./deps/python34.zip",
        GetFiles((string)pyDir + "/Lib/**/*.*",
                 f => !f.Path.FullPath.Contains("__pycache__")));

    CopyFiles(
        new FilePath[]
        {
            pyDir + File("DLLs/_multiprocessing.pyd"),
            pyDir + File("DLLs/_overlapped.pyd"),
            pyDir + File("DLLs/_socket.pyd"),
            pyDir + File("DLLs/_ssl.pyd"),
            pyDir + File("DLLs/select.pyd"),
            pyDir + File("DLLs/unicodedata.pyd")
        },
        depsDir + Directory("python34"));
});

Task("Build")
    .IsDependentOn("Generate-Project")
    .IsDependentOn("Prepare-Python-Runtime")
    .Does(() =>
{
    MSBuild("./build/PicoTorrent.sln",
        settings => settings.SetConfiguration(configuration));
});

Task("Output-Lib-Files")
    .Does(() =>
{
    Zip("./lib",
        buildDir + File("lib.zip"),
        GetFiles("./lib/**/*.py"));
});

Task("Output-Python-Runtime")
    .Does(() =>
{
    CopyFiles(GetFiles("./deps/python34.zip"), buildDir);
    CopyFiles(GetFiles("./deps/python34/*.*"), buildDir);
});

Task("Create-Win32-Package")
    .IsDependentOn("Build")
    .IsDependentOn("Output-Lib-Files")
    .IsDependentOn("Output-Python-Runtime")
    .Does(() =>
{
    IEnumerable<FilePath> files = GetFiles(buildDir + File("PicoTorrent.exe"));
    files = files.Union(GetFiles((string)buildDir + "/*.pyd"));
    files = files.Union(GetFiles(buildDir + File("lib.zip")));
    files = files.Union(GetFiles(buildDir + File("python34.zip")));

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
