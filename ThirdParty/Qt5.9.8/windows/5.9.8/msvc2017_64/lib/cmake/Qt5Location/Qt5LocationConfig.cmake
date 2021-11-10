
if (CMAKE_VERSION VERSION_LESS 2.8.3)
    message(FATAL_ERROR "Qt 5 requires at least CMake version 2.8.3")
endif()

get_filename_component(_qt5Location_install_prefix "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

# For backwards compatibility only. Use Qt5Location_VERSION instead.
set(Qt5Location_VERSION_STRING 5.9.8)

set(Qt5Location_LIBRARIES Qt5::Location)

macro(_qt5_Location_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "The imported target \"Qt5::Location\" references the file
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

macro(_populate_Location_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::Location APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt5Location_install_prefix}/bin/${LIB_LOCATION}")
    _qt5_Location_check_file_exists(${imported_location})
    set_target_properties(Qt5::Location PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_Qt5Location_LIB_DEPENDENCIES}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_Qt5Location_LIB_DEPENDENCIES}"
    )

    set(imported_implib "${_qt5Location_install_prefix}/lib/${IMPLIB_LOCATION}")
    _qt5_Location_check_file_exists(${imported_implib})
    if(NOT "${IMPLIB_LOCATION}" STREQUAL "")
        set_target_properties(Qt5::Location PROPERTIES
        "IMPORTED_IMPLIB_${Configuration}" ${imported_implib}
        )
    endif()
endmacro()

if (NOT TARGET Qt5::Location)

    set(_Qt5Location_OWN_INCLUDE_DIRS "${_qt5Location_install_prefix}/include/" "${_qt5Location_install_prefix}/include/QtLocation")
    set(Qt5Location_PRIVATE_INCLUDE_DIRS
        "${_qt5Location_install_prefix}/include/QtLocation/5.9.8"
        "${_qt5Location_install_prefix}/include/QtLocation/5.9.8/QtLocation"
    )

    foreach(_dir ${_Qt5Location_OWN_INCLUDE_DIRS})
        _qt5_Location_check_file_exists(${_dir})
    endforeach()

    # Only check existence of private includes if the Private component is
    # specified.
    list(FIND Qt5Location_FIND_COMPONENTS Private _check_private)
    if (NOT _check_private STREQUAL -1)
        foreach(_dir ${Qt5Location_PRIVATE_INCLUDE_DIRS})
            _qt5_Location_check_file_exists(${_dir})
        endforeach()
    endif()

    set(Qt5Location_INCLUDE_DIRS ${_Qt5Location_OWN_INCLUDE_DIRS})

    set(Qt5Location_DEFINITIONS -DQT_LOCATION_LIB)
    set(Qt5Location_COMPILE_DEFINITIONS QT_LOCATION_LIB)
    set(_Qt5Location_MODULE_DEPENDENCIES "Positioning;Quick;Gui;Qml;Network;Core")


    set(Qt5Location_OWN_PRIVATE_INCLUDE_DIRS ${Qt5Location_PRIVATE_INCLUDE_DIRS})

    set(_Qt5Location_FIND_DEPENDENCIES_REQUIRED)
    if (Qt5Location_FIND_REQUIRED)
        set(_Qt5Location_FIND_DEPENDENCIES_REQUIRED REQUIRED)
    endif()
    set(_Qt5Location_FIND_DEPENDENCIES_QUIET)
    if (Qt5Location_FIND_QUIETLY)
        set(_Qt5Location_DEPENDENCIES_FIND_QUIET QUIET)
    endif()
    set(_Qt5Location_FIND_VERSION_EXACT)
    if (Qt5Location_FIND_VERSION_EXACT)
        set(_Qt5Location_FIND_VERSION_EXACT EXACT)
    endif()

    set(Qt5Location_EXECUTABLE_COMPILE_FLAGS "")

    foreach(_module_dep ${_Qt5Location_MODULE_DEPENDENCIES})
        if (NOT Qt5${_module_dep}_FOUND)
            find_package(Qt5${_module_dep}
                5.9.8 ${_Qt5Location_FIND_VERSION_EXACT}
                ${_Qt5Location_DEPENDENCIES_FIND_QUIET}
                ${_Qt5Location_FIND_DEPENDENCIES_REQUIRED}
                PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
        endif()

        if (NOT Qt5${_module_dep}_FOUND)
            set(Qt5Location_FOUND False)
            return()
        endif()

        list(APPEND Qt5Location_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
        list(APPEND Qt5Location_PRIVATE_INCLUDE_DIRS "${Qt5${_module_dep}_PRIVATE_INCLUDE_DIRS}")
        list(APPEND Qt5Location_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
        list(APPEND Qt5Location_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
        list(APPEND Qt5Location_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
    endforeach()
    list(REMOVE_DUPLICATES Qt5Location_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5Location_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5Location_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5Location_COMPILE_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5Location_EXECUTABLE_COMPILE_FLAGS)

    set(_Qt5Location_LIB_DEPENDENCIES "Qt5::Positioning;Qt5::Quick;Qt5::Gui;Qt5::Qml;Qt5::Network;Qt5::Core")


    add_library(Qt5::Location SHARED IMPORTED)

    set_property(TARGET Qt5::Location PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${_Qt5Location_OWN_INCLUDE_DIRS})
    set_property(TARGET Qt5::Location PROPERTY
      INTERFACE_COMPILE_DEFINITIONS QT_LOCATION_LIB)

    set(_Qt5Location_PRIVATE_DIRS_EXIST TRUE)
    foreach (_Qt5Location_PRIVATE_DIR ${Qt5Location_OWN_PRIVATE_INCLUDE_DIRS})
        if (NOT EXISTS ${_Qt5Location_PRIVATE_DIR})
            set(_Qt5Location_PRIVATE_DIRS_EXIST FALSE)
        endif()
    endforeach()

    if (_Qt5Location_PRIVATE_DIRS_EXIST
        AND NOT CMAKE_VERSION VERSION_LESS 3.0.0 )
        add_library(Qt5::LocationPrivate INTERFACE IMPORTED)
        set_property(TARGET Qt5::LocationPrivate PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${Qt5Location_OWN_PRIVATE_INCLUDE_DIRS}
        )
        set(_Qt5Location_PRIVATEDEPS)
        foreach(dep ${_Qt5Location_LIB_DEPENDENCIES})
            if (TARGET ${dep}Private)
                list(APPEND _Qt5Location_PRIVATEDEPS ${dep}Private)
            endif()
        endforeach()
        set_property(TARGET Qt5::LocationPrivate PROPERTY
            INTERFACE_LINK_LIBRARIES Qt5::Location ${_Qt5Location_PRIVATEDEPS}
        )
    endif()

    _populate_Location_target_properties(RELEASE "Qt5Location.dll" "Qt5Location.lib" )



    _populate_Location_target_properties(DEBUG "Qt5Locationd.dll" "Qt5Locationd.lib" )



    file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt5Location_*Plugin.cmake")

    macro(_populate_Location_plugin_properties Plugin Configuration PLUGIN_LOCATION)
        set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

        set(imported_location "${_qt5Location_install_prefix}/plugins/${PLUGIN_LOCATION}")
        _qt5_Location_check_file_exists(${imported_location})
        set_target_properties(Qt5::${Plugin} PROPERTIES
            "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
    endmacro()

    if (pluginTargets)
        foreach(pluginTarget ${pluginTargets})
            include(${pluginTarget})
        endforeach()
    endif()




_qt5_Location_check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt5LocationConfigVersion.cmake")

endif()
