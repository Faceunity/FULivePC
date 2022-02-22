
if (CMAKE_VERSION VERSION_LESS 2.8.3)
    message(FATAL_ERROR "Qt 5 requires at least CMake version 2.8.3")
endif()

get_filename_component(_qt5Help_install_prefix "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

# For backwards compatibility only. Use Qt5Help_VERSION instead.
set(Qt5Help_VERSION_STRING 5.9.8)

set(Qt5Help_LIBRARIES Qt5::Help)

macro(_qt5_Help_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "The imported target \"Qt5::Help\" references the file
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

macro(_populate_Help_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::Help APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt5Help_install_prefix}/bin/${LIB_LOCATION}")
    _qt5_Help_check_file_exists(${imported_location})
    set_target_properties(Qt5::Help PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_Qt5Help_LIB_DEPENDENCIES}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_Qt5Help_LIB_DEPENDENCIES}"
    )

    set(imported_implib "${_qt5Help_install_prefix}/lib/${IMPLIB_LOCATION}")
    _qt5_Help_check_file_exists(${imported_implib})
    if(NOT "${IMPLIB_LOCATION}" STREQUAL "")
        set_target_properties(Qt5::Help PROPERTIES
        "IMPORTED_IMPLIB_${Configuration}" ${imported_implib}
        )
    endif()
endmacro()

if (NOT TARGET Qt5::Help)

    set(_Qt5Help_OWN_INCLUDE_DIRS "${_qt5Help_install_prefix}/include/" "${_qt5Help_install_prefix}/include/QtHelp")
    set(Qt5Help_PRIVATE_INCLUDE_DIRS
        "${_qt5Help_install_prefix}/include/QtHelp/5.9.8"
        "${_qt5Help_install_prefix}/include/QtHelp/5.9.8/QtHelp"
    )

    foreach(_dir ${_Qt5Help_OWN_INCLUDE_DIRS})
        _qt5_Help_check_file_exists(${_dir})
    endforeach()

    # Only check existence of private includes if the Private component is
    # specified.
    list(FIND Qt5Help_FIND_COMPONENTS Private _check_private)
    if (NOT _check_private STREQUAL -1)
        foreach(_dir ${Qt5Help_PRIVATE_INCLUDE_DIRS})
            _qt5_Help_check_file_exists(${_dir})
        endforeach()
    endif()

    set(Qt5Help_INCLUDE_DIRS ${_Qt5Help_OWN_INCLUDE_DIRS})

    set(Qt5Help_DEFINITIONS -DQT_HELP_LIB)
    set(Qt5Help_COMPILE_DEFINITIONS QT_HELP_LIB)
    set(_Qt5Help_MODULE_DEPENDENCIES "Widgets;Gui;Sql;Core")


    set(Qt5Help_OWN_PRIVATE_INCLUDE_DIRS ${Qt5Help_PRIVATE_INCLUDE_DIRS})

    set(_Qt5Help_FIND_DEPENDENCIES_REQUIRED)
    if (Qt5Help_FIND_REQUIRED)
        set(_Qt5Help_FIND_DEPENDENCIES_REQUIRED REQUIRED)
    endif()
    set(_Qt5Help_FIND_DEPENDENCIES_QUIET)
    if (Qt5Help_FIND_QUIETLY)
        set(_Qt5Help_DEPENDENCIES_FIND_QUIET QUIET)
    endif()
    set(_Qt5Help_FIND_VERSION_EXACT)
    if (Qt5Help_FIND_VERSION_EXACT)
        set(_Qt5Help_FIND_VERSION_EXACT EXACT)
    endif()

    set(Qt5Help_EXECUTABLE_COMPILE_FLAGS "")

    foreach(_module_dep ${_Qt5Help_MODULE_DEPENDENCIES})
        if (NOT Qt5${_module_dep}_FOUND)
            find_package(Qt5${_module_dep}
                5.9.8 ${_Qt5Help_FIND_VERSION_EXACT}
                ${_Qt5Help_DEPENDENCIES_FIND_QUIET}
                ${_Qt5Help_FIND_DEPENDENCIES_REQUIRED}
                PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
        endif()

        if (NOT Qt5${_module_dep}_FOUND)
            set(Qt5Help_FOUND False)
            return()
        endif()

        list(APPEND Qt5Help_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
        list(APPEND Qt5Help_PRIVATE_INCLUDE_DIRS "${Qt5${_module_dep}_PRIVATE_INCLUDE_DIRS}")
        list(APPEND Qt5Help_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
        list(APPEND Qt5Help_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
        list(APPEND Qt5Help_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
    endforeach()
    list(REMOVE_DUPLICATES Qt5Help_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5Help_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5Help_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5Help_COMPILE_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5Help_EXECUTABLE_COMPILE_FLAGS)

    set(_Qt5Help_LIB_DEPENDENCIES "Qt5::Widgets;Qt5::Gui;Qt5::Sql;Qt5::Core")


    add_library(Qt5::Help SHARED IMPORTED)

    set_property(TARGET Qt5::Help PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${_Qt5Help_OWN_INCLUDE_DIRS})
    set_property(TARGET Qt5::Help PROPERTY
      INTERFACE_COMPILE_DEFINITIONS QT_HELP_LIB)

    set(_Qt5Help_PRIVATE_DIRS_EXIST TRUE)
    foreach (_Qt5Help_PRIVATE_DIR ${Qt5Help_OWN_PRIVATE_INCLUDE_DIRS})
        if (NOT EXISTS ${_Qt5Help_PRIVATE_DIR})
            set(_Qt5Help_PRIVATE_DIRS_EXIST FALSE)
        endif()
    endforeach()

    if (_Qt5Help_PRIVATE_DIRS_EXIST
        AND NOT CMAKE_VERSION VERSION_LESS 3.0.0 )
        add_library(Qt5::HelpPrivate INTERFACE IMPORTED)
        set_property(TARGET Qt5::HelpPrivate PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${Qt5Help_OWN_PRIVATE_INCLUDE_DIRS}
        )
        set(_Qt5Help_PRIVATEDEPS)
        foreach(dep ${_Qt5Help_LIB_DEPENDENCIES})
            if (TARGET ${dep}Private)
                list(APPEND _Qt5Help_PRIVATEDEPS ${dep}Private)
            endif()
        endforeach()
        set_property(TARGET Qt5::HelpPrivate PROPERTY
            INTERFACE_LINK_LIBRARIES Qt5::Help ${_Qt5Help_PRIVATEDEPS}
        )
    endif()

    _populate_Help_target_properties(RELEASE "Qt5Help.dll" "Qt5Help.lib" )



    _populate_Help_target_properties(DEBUG "Qt5Helpd.dll" "Qt5Helpd.lib" )



    file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt5Help_*Plugin.cmake")

    macro(_populate_Help_plugin_properties Plugin Configuration PLUGIN_LOCATION)
        set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

        set(imported_location "${_qt5Help_install_prefix}/plugins/${PLUGIN_LOCATION}")
        _qt5_Help_check_file_exists(${imported_location})
        set_target_properties(Qt5::${Plugin} PROPERTIES
            "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
    endmacro()

    if (pluginTargets)
        foreach(pluginTarget ${pluginTargets})
            include(${pluginTarget})
        endforeach()
    endif()


    include("${CMAKE_CURRENT_LIST_DIR}/Qt5HelpConfigExtras.cmake")


_qt5_Help_check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt5HelpConfigVersion.cmake")

endif()
