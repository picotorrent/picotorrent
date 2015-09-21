//////////////////////////////////////////////////////////////////////
// ARGUMENTS
//////////////////////////////////////////////////////////////////////

var target        = Argument("target", "Default");
var configuration = Argument("configuration", "Release");

// Parameters
var buildDir = Directory("./build") + Directory(configuration);

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
            Arguments = ".. -G \"Visual Studio 14 Win64\" -T \"v140\"",
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

//////////////////////////////////////////////////////////////////////
// TASK TARGETS
//////////////////////////////////////////////////////////////////////

Task("Default")
    .IsDependentOn("Build");

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

RunTarget(target);
