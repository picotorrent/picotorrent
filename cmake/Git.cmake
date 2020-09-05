function(git_abbreviated_hash path output)
    execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${path}
        OUTPUT_VARIABLE TMP
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(${output} ${TMP} PARENT_SCOPE)
endfunction()
