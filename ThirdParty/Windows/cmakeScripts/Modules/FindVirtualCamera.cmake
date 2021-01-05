# VirtualCamera library import helper
function(FindVirtualCamera package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../VirtualCamera ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win32/VirtualCamera.lib PARENT_SCOPE)
    else()
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win64/VirtualCamera.lib PARENT_SCOPE)
    endif()
endfunction(FindVirtualCamera)

FindVirtualCamera(VirtualCamera)
