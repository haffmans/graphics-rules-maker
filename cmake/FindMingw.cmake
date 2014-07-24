if(MINGW)
    include(FindPkgMacros)
    findpkg_begin(MingW)

    # Get path, convert backslashes as ${ENV_DXSDK_DIR}
    getenv_path(MINGW_DIR)

    # Determine based on CMAKE_C_COMPILER path
    GET_FILENAME_COMPONENT(MingW_COMPILER_DIR "${CMAKE_C_COMPILER}" DIRECTORY)

    SET(MingW_LIB_SEARCH_PATH
        "${MINGW_DIR}/bin" "$ENV{MINGW_DIR}/bin"
        "${MingW_COMPILER_DIR}"
        "C:/MingW/bin" "C:/Program Files/MingW/bin"
        "C:/Program Files (x86)/MingW/bin"
        "C:/Qt/Tools/mingw*/bin"
    )

    find_library(Mingw_stdc_LIBRARY NAMES "stdc++-6" HINTS ${MingW_LIB_SEARCH_PATH})
    find_library(Mingw_gcc_s_dw2_LIBRARY NAMES "gcc_s_dw2" HINTS ${MingW_LIB_SEARCH_PATH})
    find_library(Mingw_winpthread_LIBRARY NAMES "winpthread" HINTS ${MingW_LIB_SEARCH_PATH})

    findpkg_finish(DirectX)
endif(MINGW)
