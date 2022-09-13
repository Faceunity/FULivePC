# Glew library import helper
function(FindGlfw package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../glfw ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
	if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(${package}_LIBRARIES ${${package}_HOME}/lib/x86_64/libglfw.3.3.dylib PARENT_SCOPE)
	else()
		set(${package}_LIBRARIES ${${package}_HOME}/lib/arm64/libglfw.3.3.dylib PARENT_SCOPE)
	endif()
endfunction(FindGlfw)

FindGlfw(Glfw)
