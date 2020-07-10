# Nama library import helper
function(FindNama package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../FaceUnity-SDK-PC ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    STRING(FIND $ENV{CMAKE_GENERATOR} "Win64" IsFinded)
    if(${IsFinded} EQUAL -1)
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win32/CNamaSDK.lib PARENT_SCOPE)
    else()
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win64/CNamaSDK.lib PARENT_SCOPE)
    endif()
endfunction(FindNama)

FindNama(Nama)
