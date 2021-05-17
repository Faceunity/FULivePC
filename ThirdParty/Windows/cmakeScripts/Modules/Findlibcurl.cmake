# Findlibcurl library import helper
function(Findlibcurl package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../libcurl ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win32/libcurl.lib PARENT_SCOPE)
    else()
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win64/libcurl.lib PARENT_SCOPE)
    endif()
endfunction(Findlibcurl)

Findlibcurl(libcurl)
