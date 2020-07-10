# FreeImage library import helper
function(FindFreeImage package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../FreeImage ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    STRING(FIND $ENV{CMAKE_GENERATOR} "Win64" IsFinded)
    if(${IsFinded} EQUAL -1)
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win32/FreeImage.lib PARENT_SCOPE)
    else()
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win64/FreeImage.lib PARENT_SCOPE)
    endif()
endfunction(FindFreeImage)

FindFreeImage(FreeImage)
