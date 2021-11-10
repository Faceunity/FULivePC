
if (CMAKE_VERSION VERSION_LESS 2.8.3)
    message(FATAL_ERROR "Qt 5 requires at least CMake version 2.8.3")
endif()

get_filename_component(_qt5OpenGL_install_prefix "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

# For backwards compatibility only. Use Qt5OpenGL_VERSION instead.
set(Qt5OpenGL_VERSION_STRING 5.9.8)

set(Qt5OpenGL_LIBRARIES Qt5::OpenGL)

macro(_qt5_OpenGL_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "The imported target \"Qt5::OpenGL\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
endmacro()

macro(_populate_OpenGL_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::OpenGL APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt5OpenGL_install_prefix}/bin/${LIB_LOCATION}")
    _qt5_OpenGL_check_file_exists(${imported_location})
    set_target_properties(Qt5::OpenGL PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_Qt5OpenGL_LIB_DEPENDENCIES}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_Qt5OpenGL_LIB_DEPENDENCIES}"
    )

    set(imported_implib "${_qt5OpenGL_install_prefix}/lib/${IMPLIB_LOCATION}")
    _qt5_OpenGL_check_file_exists(${imported_implib})
    if(NOT "${IMPLIB_LOCATION}" STREQUAL "")
        set_target_properties(Qt5::OpenGL PROPERTIES
        "IMPORTED_IMPLIB_${Configuration}" ${imported_implib}
        )
    endif()
endmacro()

if (NOT TARGET Qt5::OpenGL)

    set(_Qt5OpenGL_OWN_INCLUDE_DIRS "${_qt5OpenGL_install_prefix}/include/" "${_qt5OpenGL_install_prefix}/include/QtOpenGL")
    set(Qt5OpenGL_PRIVATE_INCLUDE_DIRS
        "${_qt5OpenGL_install_prefix}/include/QtOpenGL/5.9.8"
        "${_qt5OpenGL_install_prefix}/include/QtOpenGL/5.9.8/QtOpenGL"
    )

    foreach(_dir ${_Qt5OpenGL_OWN_INCLUDE_DIRS})
        _qt5_OpenGL_check_file_exists(${_dir})
    endforeach()

    # Only check existence of private includes if the Private component is
    # specified.
    list(FIND Qt5OpenGL_FIND_COMPONENTS Private _check_private)
    if (NOT _check_private STREQUAL -1)
        foreach(_dir ${Qt5OpenGL_PRIVATE_INCLUDE_DIRS})
            _qt5_OpenGL_check_file_exists(${_dir})
        endforeach()
    endif()

    set(Qt5OpenGL_INCLUDE_DIRS ${_Qt5OpenGL_OWN_INCLUDE_DIRS})

    set(Qt5OpenGL_DEFINITIONS -DQT_OPENGL_LIB)
    set(Qt5OpenGL_COMPILE_DEFINITIONS QT_OPENGL_LIB)
    set(_Qt5OpenGL_MODULE_DEPENDENCIES "Widgets;Gui;Core")


    set(Qt5OpenGL_OWN_PRIVATE_INCLUDE_DIRS ${Qt5OpenGL_PRIVATE_INCLUDE_DIRS})

    set(_Qt5OpenGL_FIND_DEPENDENCIES_REQUIRED)
    if (Qt5OpenGL_FIND_REQUIRED)
        set(_Qt5OpenGL_FIND_DEPENDENCIES_REQUIRED REQUIRED)
    endif()
    set(_Qt5OpenGL_FIND_DEPENDENCIES_QUIET)
    if (Qt5OpenGL_FIND_QUIETLY)
        set(_Qt5OpenGL_DEPENDENCIES_FIND_QUIET QUIET)
    endif()
    set(_Qt5OpenGL_FIND_VERSION_EXACT)
    if (Qt5OpenGL_FIND_VERSION_EXACT)
        set(_Qt5OpenGL_FIND_VERSION_EXACT EXACT)
    endif()

    set(Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS "")

    foreach(_module_dep ${_Qt5OpenGL_MODULE_DEPENDENCIES})
        if (NOT Qt5${_module_dep}_FOUND)
            find_package(Qt5${_module_dep}
                5.9.8 ${_Qt5OpenGL_FIND_VERSION_EXACT}
                ${_Qt5OpenGL_DEPENDENCIES_FIND_QUIET}
                ${_Qt5OpenGL_FIND_DEPENDENCIES_REQUIRED}
                PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
        endif()

        if (NOT Qt5${_module_dep}_FOUND)
            set(Qt5OpenGL_FOUND False)
            return()
        endif()

        list(APPEND Qt5OpenGL_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
        list(APPEND Qt5OpenGL_PRIVATE_INCLUDE_DIRS "${Qt5${_module_dep}_PRIVATE_INCLUDE_DIRS}")
        list(APPEND Qt5OpenGL_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
        list(APPEND Qt5OpenGL_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
        list(APPEND Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
    endforeach()
    list(REMOVE_DUPLICATES Qt5OpenGL_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5OpenGL_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5OpenGL_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5OpenGL_COMPILE_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS)

    set(_Qt5OpenGL_LIB_DEPENDENCIES "Qt5::Widgets;Qt5::Gui;Qt5::Core")


    add_library(Qt5::OpenGL SHARED IMPORTED)

    set_property(TARGET Qt5::OpenGL PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${_Qt5OpenGL_OWN_INCLUDE_DIRS})
    set_property(TARGET Qt5::OpenGL PROPERTY
      INTERFACE_COMPILE_DEFINITIONS QT_OPENGL_LIB)

    set(_Qt5OpenGL_PRIVATE_DIRS_EXIST TRUE)
    foreach (_Qt5OpenGL_PRIVATE_DIR ${Qt5OpenGL_OWN_PRIVATE_INCLUDE_DIRS})
        if (NOT EXISTS ${_Qt5OpenGL_PRIVATE_DIR})
            set(_Qt5OpenGL_PRIVATE_DIRS_EXIST FALSE)
        endif()
    endforeach()

    if (_Qt5OpenGL_PRIVATE_DIRS_EXIST
        AND NOT CMAKE_VERSION VERSION_LESS 3.0.0 )
        add_library(Qt5::OpenGLPrivate INTERFACE IMPORTED)
        set_property(TARGET Qt5::OpenGLPrivate PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${Qt5OpenGL_OWN_PRIVATE_INCLUDE_DIRS}
        )
        set(_Qt5OpenGL_PRIVATEDEPS)
        foreach(dep ${_Qt5OpenGL_LIB_DEPENDENCIES})
            if (TARGET ${dep}Private)
                list(APPEND _Qt5OpenGL_PRIVATEDEPS ${dep}Private)
            endif()
        endforeach()
        set_property(TARGET Qt5::OpenGLPrivate PROPERTY
            INTERFACE_LINK_LIBRARIES Qt5::OpenGL ${_Qt5OpenGL_PRIVATEDEPS}
        )
    endif()

    _populate_OpenGL_target_properties(RELEASE "Qt5OpenGL.dll" "Qt5OpenGL.lib" )



    _populate_OpenGL_target_properties(DEBUG "Qt5OpenGLd.dll" "Qt5OpenGLd.lib" )



    file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt5OpenGL_*Plugin.cmake")

    macro(_populate_OpenGL_plugin_properties Plugin Configuration PLUGIN_LOCATION)
        set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

        set(imported_location "${_qt5OpenGL_install_prefix}/plugins/${PLUGIN_LOCATION}")
        _qt5_OpenGL_check_file_exists(${imported_location})
        set_target_properties(Qt5::${Plugin} PROPERTIES
            "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
    endmacro()

    if (pluginTargets)
        foreach(pluginTarget ${pluginTargets})
            include(${pluginTarget})
        endforeach()
    endif()




_qt5_OpenGL_check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt5OpenGLConfigVersion.cmake")

endif()
