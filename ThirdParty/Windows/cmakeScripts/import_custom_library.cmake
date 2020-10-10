# Import custom library, update includes and libs
macro(import_custom_library target package)
    find_package(${package} REQUIRED)
    if(${package}_FOUND)
        message("# Package ${package}: ${${package}_INCLUDE_DIRS}")
        foreach(${package}_INCLUDE_DIR ${${package}_INCLUDE_DIRS})
            target_include_directories(${target}
                SYSTEM  # to suppress(if possible) warnings from this library
                PRIVATE ${${package}_INCLUDE_DIR})
        endforeach()
		if(${package}_LIBRARIES_DEBUG)
			target_link_libraries(${target} debug ${${package}_LIBRARIES_DEBUG} optimized ${${package}_LIBRARIES})
		else()
			target_link_libraries(${target} ${${package}_LIBRARIES})
		endif()
        
    else()
        message(SEND_ERROR "Package ${package} not found!")
    endif(${package}_FOUND)
endmacro(import_custom_library)
