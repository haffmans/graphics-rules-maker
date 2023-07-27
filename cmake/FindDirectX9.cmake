#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Find DirectX9 SDK
# Define:
# DirectX9_FOUND
# DirectX9_INCLUDE_DIRS
# DirectX9_LIBRARIES

if(WIN32) # The only platform it makes sense to check for DirectX9 SDK
        include(FindPkgMacros)
        findpkg_begin(DirectX9)

        if( MSVC AND NOT MSVC90 )
                # Windows 10.x SDK
                get_filename_component(kit10_dir "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" REALPATH)
                file(GLOB W10SDK_VERSIONS RELATIVE ${kit10_dir}/Include ${kit10_dir}/Include/10.*) 			# enumerate pre-release and not yet known release versions
                list(APPEND W10SDK_VERSIONS "10.0.10240.0" "10.0.14393.0" "10.0.15063.0" "10.0.16299.0")	# enumerate well known release versions
                list(REMOVE_DUPLICATES W10SDK_VERSIONS)
                list(SORT W10SDK_VERSIONS)
                list(REVERSE W10SDK_VERSIONS)																# sort from high to low
                if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
                        list(INSERT W10SDK_VERSIONS 0 ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})				# prefer version passed by CMake, if any
                endif()
                foreach(W10SDK_VER ${W10SDK_VERSIONS})
                        find_path(DirectX9_INCLUDE_DIR NAMES d3d9.h HINTS "${kit10_dir}/Include/${W10SDK_VER}/um" "C:/Program Files (x86)/Windows Kits/10/Include/${W10SDK_VER}/um" "C:/Program Files/Windows Kits/10/Include/${W10SDK_VER}/um")
                endforeach()
                # Windows 8.1 SDK
                if(NOT DirectX9_INCLUDE_DIR)
                        find_path(DirectX9_INCLUDE_DIR NAMES d3d9.h HINTS "C:/Program Files (x86)/Windows Kits/8.1/include/um" "C:/Program Files/Windows Kits/8.1/include/um")
                endif()
                # Windows 8.0 SDK
                if(NOT DirectX9_INCLUDE_DIR)
                        find_path(DirectX9_INCLUDE_DIR NAMES d3d9.h HINTS "C:/Program Files (x86)/Windows Kits/8.0/include/um" "C:/Program Files/Windows Kits/8.0/include/um")
                endif()
        endif()

        if(DirectX9_INCLUDE_DIR)
                # No need to specify full path to libraries, proper version would be found as part of SDK, in one of the following places
                # "C:/Program Files (x86)/Windows Kits/10/lib/${W10SDK_VER}/um/${MSVC_CXX_ARCHITECTURE_ID}/"
                # "C:/Program Files (x86)/Windows Kits/8.1/lib/winv6.3/um/${MSVC_CXX_ARCHITECTURE_ID}/"
                # "C:/Program Files (x86)/Windows Kits/8.0/lib/win8/um/${MSVC_CXX_ARCHITECTURE_ID}/"
                # "C:/Program Files (x86)/Windows Phone Kits/8.1/lib/${MSVC_CXX_ARCHITECTURE_ID}/"
                # "C:/Program Files (x86)/Windows Phone Kits/8.0/lib/${MSVC_CXX_ARCHITECTURE_ID}/"
                set(DirectX9_LIBRARY d3d9.lib dxgi.lib dxguid.lib)
                set(DirectX9_D3D9_LIBRARY d3d9.lib)
                set(DirectX9_DXGI_LIBRARY dxgi.lib)
                set(DirectX9_DXGUID_LIBRARY dxguid.lib)
                # but it is usefull to output selected version to the log
                message(STATUS "Found DirectX9 headers: ${DirectX9_INCLUDE_DIR}")
        endif()

        # Legacy Direct X SDK
        if( NOT DirectX9_INCLUDE_DIR OR NOT DirectX9_LIBRARY )
                # Get path, convert backslashes as ${ENV_DXSDK_DIR}
                getenv_path(DXSDK_DIR)
                getenv_path(DIRECTX_HOME)
                getenv_path(DIRECTX_ROOT)
                getenv_path(DIRECTX_BASE)

                # construct search paths
                set(DirectX9_PREFIX_PATH
                "${DXSDK_DIR}" "${ENV_DXSDK_DIR}"
                "${DIRECTX_HOME}" "${ENV_DIRECTX_HOME}"
                "${DIRECTX_ROOT}" "${ENV_DIRECTX_ROOT}"
                "${DIRECTX_BASE}" "${ENV_DIRECTX_BASE}"
                "C:/apps_x86/Microsoft DirectX SDK*"
                "C:/Program Files (x86)/Microsoft DirectX SDK*"
                "C:/apps/Microsoft DirectX SDK*"
                "C:/Program Files/Microsoft DirectX SDK*"
                "$ENV{ProgramFiles}/Microsoft DirectX SDK*"
                )

                create_search_paths(DirectX9)
                # redo search if prefix path changed
                clear_if_changed(DirectX9_PREFIX_PATH
                        DirectX9_LIBRARY
                        DirectX9_INCLUDE_DIR
                )

                # dlls are in DirectX9_ROOT_DIR/Developer Runtime/x64|x86
                # lib files are in DirectX9_ROOT_DIR/Lib/x64|x86
                if(CMAKE_CL_64)
                        set(DirectX9_LIBPATH_SUFFIX "x64")
                else(CMAKE_CL_64)
                        set(DirectX9_LIBPATH_SUFFIX "x86")
                endif(CMAKE_CL_64)

                # look for d3d9 components
                find_path(DirectX9_INCLUDE_DIR NAMES d3d9.h HINTS ${DirectX9_INC_SEARCH_PATH})
                find_library(DirectX9_DXERR_LIBRARY NAMES DxErr HINTS ${DirectX9_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX9_LIBPATH_SUFFIX})
                find_library(DirectX9_DXGUID_LIBRARY NAMES dxguid HINTS ${DirectX9_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX9_LIBPATH_SUFFIX})
                find_library(DirectX9_DXGI_LIBRARY NAMES dxgi HINTS ${DirectX9_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX9_LIBPATH_SUFFIX})
                find_library(DirectX9_D3DCOMPILER_LIBRARY NAMES d3dcompiler HINTS ${DirectX9_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX9_LIBPATH_SUFFIX})
                find_library(DirectX9_D3D9_LIBRARY NAMES d3d9 HINTS ${DirectX9_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX9_LIBPATH_SUFFIX})
                find_library(DirectX9_D3DX9_LIBRARY NAMES d3dx9 HINTS ${DirectX9_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX9_LIBPATH_SUFFIX})

                if (DirectX9_INCLUDE_DIR AND DirectX9_D3D9_LIBRARY)
                  set(DirectX9_D3D9_LIBRARIES ${DirectX9_D3D9_LIBRARIES}
                        ${DirectX9_D3D9_LIBRARY}
                        ${DirectX9_DXGI_LIBRARY}
                        ${DirectX9_DXGUID_LIBRARY}
                        ${DirectX9_D3DCOMPILER_LIBRARY}
                  )
                endif ()
                if (DirectX9_D3DX9_LIBRARY)
                        set(DirectX9_d3d9_LIBRARIES ${DirectX9_d3d9_LIBRARIES} ${DirectX9_D3DX9_LIBRARY})
                endif ()
                if (DirectX9_DXERR_LIBRARY)
                        set(DirectX9_d3d9_LIBRARIES ${DirectX9_d3d9_LIBRARIES} ${DirectX9_DXERR_LIBRARY})
                endif ()

                set(DirectX9_LIBRARY
                        ${DirectX9_d3d9_LIBRARIES}
                )

                mark_as_advanced(DirectX9_d3d9_LIBRARY
                                                 DirectX9_D3DX9_LIBRARY
                                                 DirectX9_DXERR_LIBRARY
                                                 DirectX9_DXGUID_LIBRARY
                                                 DirectX9_DXGI_LIBRARY
                                                 DirectX9_D3DCOMPILER_LIBRARY)
        endif () # Legacy Direct X SDK

        findpkg_finish(DirectX9)

endif(WIN32)
