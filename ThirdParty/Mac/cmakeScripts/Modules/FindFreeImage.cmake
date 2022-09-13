# FreeImage library import helper
function(FindFreeImage package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../freeimage ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
	if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(${package}_LIBRARIES ${${package}_HOME}/lib/x86_64/libfreeimage.a PARENT_SCOPE)
	else()
		set(${package}_LIBRARIES ${${package}_HOME}/lib/arm64/libfreeimage.a PARENT_SCOPE)
	endif()
endfunction(FindFreeImage)

FindFreeImage(FreeImage)
