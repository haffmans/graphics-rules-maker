# Lowercase names of Win32 system libraries to ignore
set(win32_system_libs
    advapi32.dll crypt32.dll dnsapi.dll dwmapi.dll gdi32.dll kernel32.dll mpr.dll shell32.dll
    user32.dll ws2_32.dll ole32.dll msvcrt.dll mscvrt.dll winmm.dll netapi32.dll uxtheme.dll
    iphlpapi.dll version.dll userenv.dll opengl32.dll dxgi.dll d3d9.dll d3d10.dll d3d11.dll d3d12.dll
    imm32.dll oleaut32.dll wtsapi32.dll
)

function(_gp_is_win32_system_library library outvar)
    # Ignore case
    string(TOLOWER "${library}" library_lower)
    get_filename_component(library_name ${library_lower} NAME)

    list(FIND win32_system_libs ${library_name} _is_system_lib)

    if (${_is_system_lib} EQUAL -1)
        set(${outvar} 0 PARENT_SCOPE)
    else()
        set(${outvar} 1 PARENT_SCOPE)
    endif()
endfunction()

function(gp_resolve_item_override context item exepath dirs resolved_item_var resolved_var)
    # If already resolved, skip this
    set(override_resolved ${${resolved_var}})
    if (override_resolved)
        return()
    endif()

    # Override system libraries to a fake sysroot
    _gp_is_win32_system_library(${item} item_is_system_lib)
    if (${item_is_system_lib})
        set(${resolved_item_var} ${PROJECT_BINARY_DIR}/fake/system/${item} PARENT_SCOPE)
        set(${resolved_var} 1 PARENT_SCOPE)
        set(override_resolved 1)
    endif ()

    # Resolve in parent directories, required for plugins with dlls in their base
    set(exe_parent ${exepath})
    while (NOT override_resolved AND NOT exe_parent STREQUAL previous_parent)
        if (EXISTS ${exe_parent}/${item})
            set(${resolved_item_var} ${exe_parent}/${item} PARENT_SCOPE)
            set(${resolved_var} 1 PARENT_SCOPE)
            set(override_resolved 1)
        endif()

        # Keep previous to detect root (e.g. / on Linux, possibly e.g. C: on Windows)
        set(previous_parent ${exe_parent})
        get_filename_component(exe_parent ${exe_parent} DIRECTORY)

    endwhile()
endfunction()

function(gp_resolved_file_type_override resolved_item type_var)
    _gp_is_win32_system_library(${resolved_item} item_is_system_lib)
    if (${item_is_system_lib})
        message(STATUS "Marking ${resolved_item} as system library")
        set(${type_var} "system" PARENT_SCOPE)
    endif ()

    string(FIND "${resolved_item}" "${CMAKE_INSTALL_PREFIX}" subdirpos)
    if (subdirpos EQUAL 0)
        message(STATUS "Marking ${resolved_item} as local library")
        set(${type_var} "local" PARENT_SCOPE)
    endif()
endfunction()
