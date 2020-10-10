# Glfw library import helper
function(FindGlfw package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../GL ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win32/glfw3.lib PARENT_SCOPE)
    else()
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win64/glfw3.lib PARENT_SCOPE)
    endif()
endfunction(FindGlfw)

FindGlfw(Glfw)
