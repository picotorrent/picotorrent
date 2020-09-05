# GitVersion setup
set(GITVERSION_DIR     "${CMAKE_SOURCE_DIR}/tools")
set(GITVERSION_ARCHIVE "${GITVERSION_DIR}/gitversion.tar.gz")
set(GITVERSION_EXE     "${GITVERSION_DIR}/gitversion.exe")
set(GITVERSION_URL     "https://github.com/GitTools/GitVersion/releases/download/5.3.7/gitversion-win-x86-5.3.7.tar.gz")

function(gitversion_install)
    if (NOT EXISTS ${GITVERSION_ARCHIVE})
        message("Installing GitVersion")

        file(DOWNLOAD   ${GITVERSION_URL} ${GITVERSION_ARCHIVE}
             TIMEOUT    60
             TLS_VERIFY ON)

        file(ARCHIVE_EXTRACT
             INPUT       ${GITVERSION_ARCHIVE}
             DESTINATION ${GITVERSION_DIR}
             VERBOSE)
    endif()
endfunction()

function(gitversion_showvariable variable output)
    execute_process(
        COMMAND ${GITVERSION_EXE} /showvariable ${variable}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE TMP
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(${output} ${TMP} PARENT_SCOPE)
endfunction()
