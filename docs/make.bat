@ECHO OFF

REM Command file for Sphinx documentation

if "%SPHINXBUILD%" == "" (
    set SPHINXBUILD=sphinx-build
)
set BUILDDIR=build
set SRCDIR=src
set ALLSPHINXOPTS=-d %BUILDDIR%/doctrees %SPHINXOPTS% %SRCDIR%
set I18NSPHINXOPTS=%SPHINXOPTS% %SRCDIR%
if NOT "%PAPER%" == "" (
    set ALLSPHINXOPTS=-D latex_paper_size=%PAPER% %ALLSPHINXOPTS%
    set I18NSPHINXOPTS=-D latex_paper_size=%PAPER% %I18NSPHINXOPTS%
)

if "%1" == "" goto help

if "%1" == "help" (
    :help
    echo.Please use `make ^<target^>` where ^<target^> is one of
    echo.  html       to make standalone HTML files
    goto end
)

if "%1" == "clean" (
    for /d %%i in (%BUILDDIR%\*) do rmdir /q /s %%i
    del /q /s %BUILDDIR%\*
    goto end
)

if "%1" == "html" (
    %SPHINXBUILD% -b html %ALLSPHINXOPTS% %BUILDDIR%/html
    if errorlevel 1 exit /b 1
    echo.
    echo.Build finished. The HTML pages are in %BUILDDIR%/html.
    goto end
)

:end
