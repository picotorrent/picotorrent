set(ANTLR4_SOURCE_DIR "../../vendor/antlr4/runtime/Cpp")

file(GLOB libantlrcpp_SRC
  "${ANTLR4_SOURCE_DIR}/runtime/src/*.cpp"
  "${ANTLR4_SOURCE_DIR}/runtime/src/atn/*.cpp"
  "${ANTLR4_SOURCE_DIR}/runtime/src/dfa/*.cpp"
  "${ANTLR4_SOURCE_DIR}/runtime/src/misc/*.cpp"
  "${ANTLR4_SOURCE_DIR}/runtime/src/support/*.cpp"
  "${ANTLR4_SOURCE_DIR}/runtime/src/tree/*.cpp"
  "${ANTLR4_SOURCE_DIR}/runtime/src/tree/pattern/*.cpp"
  "${ANTLR4_SOURCE_DIR}/runtime/src/tree/xpath/*.cpp"
)

add_library(
    antlr4
    STATIC
    ${libantlrcpp_SRC})

target_compile_definitions(
    antlr4
    PRIVATE
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    -DANTLR4CPP_STATIC
)

target_compile_definitions(
    antlr4
    INTERFACE
    -D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
    -DANTLR4CPP_STATIC
)

target_include_directories(
    antlr4
    PRIVATE
    ${ANTLR4_SOURCE_DIR}/runtime/src
    ${ANTLR4_SOURCE_DIR}/runtime/src/atn
    ${ANTLR4_SOURCE_DIR}/runtime/src/dfa
    ${ANTLR4_SOURCE_DIR}/runtime/src/misc
    ${ANTLR4_SOURCE_DIR}/runtime/src/support
    ${ANTLR4_SOURCE_DIR}/runtime/src/tree
    ${ANTLR4_SOURCE_DIR}/runtime/src/tree/pattern
    ${ANTLR4_SOURCE_DIR}/runtime/src/tree/xpath
)

target_link_libraries(
    antlr4
    PUBLIC
    utf8cpp::utf8cpp
)

add_library(
    PicoTorrentPQL
    STATIC
    generated/QueryBaseVisitor.cpp
    generated/QueryLexer.cpp
    generated/QueryParser.cpp
    generated/QueryVisitor.cpp
)

target_include_directories(
    PicoTorrentPQL
    PUBLIC
    "../../vendor/antlr4/runtime/Cpp/runtime/src"
)

target_include_directories(
    PicoTorrentPQL
    INTERFACE
    generated/
)

target_link_libraries(
    PicoTorrentPQL
    PUBLIC
    antlr4
)
