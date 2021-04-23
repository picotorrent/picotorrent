vcpkg_fail_port_install(ON_TARGET "uwp")

if(VCPKG_TARGET_IS_WINDOWS)
    # Building python bindings is currently broken on Windows
    if("python" IN_LIST FEATURES)
        message(FATAL_ERROR "The python feature is currently broken on Windows")
    endif()

    if(NOT "iconv" IN_LIST FEATURES)
        # prevent picking up libiconv if it happens to already be installed
        # set(ICONV_PATCH "no_use_iconv.patch")
    endif()

    if(VCPKG_CRT_LINKAGE STREQUAL "static")
        set(_static_runtime ON)
    endif()
endif()

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    deprfun     deprecated-functions
    examples    build_examples
    python      python-bindings
    test        build_tests
    tools       build_tools
)

# Note: the python feature currently requires `python3-dev` and `python3-setuptools` installed on the system
if("python" IN_LIST FEATURES)
    vcpkg_find_acquire_program(PYTHON3)
    get_filename_component(PYTHON3_PATH ${PYTHON3} DIRECTORY)
    vcpkg_add_to_path(${PYTHON3_PATH})

    file(GLOB BOOST_PYTHON_LIB "${CURRENT_INSTALLED_DIR}/lib/*boost_python*")
    string(REGEX REPLACE ".*(python)([0-9])([0-9]+).*" "\\1\\2\\3" _boost-python-module-name "${BOOST_PYTHON_LIB}")
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO arvidn/libtorrent
    REF af7a96c1df47fcc8fbe0d791c223b0ab8a7d2125 #v2.0.3
    SHA512 1c1a73f065e6c726ef6b87f6be139abb96bdb2d924e4c6eb3ed736ded3762b9f250c44bd4fc7b703975463bcca18d7518e0588703616e686021b575b8f1193f0
    HEAD_REF RC_2_0
    PATCHES
        ${ICONV_PATCH}
        fix-AppleClang-test.patch
)

vcpkg_from_github(
    OUT_SOURCE_PATH TRYSIGNAL_SOURCE_PATH
    REPO arvidn/try_signal
    REF 334fd139e2bb387017b42d36753a03935e3bca75
    SHA512 a25d439b2d979e975f9dd125a34072f70bfc7a08fab950e3829130742c05c584ae88d9f58fc0f1b4fa0b51df2c0e32c5b24c5828d53b121b4bc183a4c68d6a5a
    HEAD_REF master
)

file(
    COPY
    ${TRYSIGNAL_SOURCE_PATH}/signal_error_code.cpp
    ${TRYSIGNAL_SOURCE_PATH}/signal_error_code.hpp
    ${TRYSIGNAL_SOURCE_PATH}/try_signal.cpp
    ${TRYSIGNAL_SOURCE_PATH}/try_signal.hpp
    ${TRYSIGNAL_SOURCE_PATH}/try_signal_mingw.hpp
    ${TRYSIGNAL_SOURCE_PATH}/try_signal_msvc.hpp
    ${TRYSIGNAL_SOURCE_PATH}/try_signal_posix.hpp
    DESTINATION ${SOURCE_PATH}/deps/try_signal)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA # Disable this option if project cannot be built with Ninja
    OPTIONS
        ${FEATURE_OPTIONS}
        -Dboost-python-module-name=${_boost-python-module-name}
        -Dstatic_runtime=${_static_runtime}
        -DPython3_USE_STATIC_LIBS=ON
)

vcpkg_install_cmake()

vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/LibtorrentRasterbar TARGET_PATH share/LibtorrentRasterbar)

# Handle copyright
file(INSTALL ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

# Do not duplicate include files
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include ${CURRENT_PACKAGES_DIR}/debug/share ${CURRENT_PACKAGES_DIR}/share/cmake)
