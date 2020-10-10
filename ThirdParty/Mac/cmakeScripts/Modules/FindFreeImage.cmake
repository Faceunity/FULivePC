# FreeImage library import helper
function(FindFreeImage package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../freeimage ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    set(${package}_LIBRARIES ${${package}_HOME}/lib/libfreeimage.a PARENT_SCOPE)
endfunction(FindFreeImage)

FindFreeImage(FreeImage)
