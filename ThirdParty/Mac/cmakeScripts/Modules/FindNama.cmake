# Nama library import helper
function(FindNama package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../FaceUnity-SDK-Mac ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    set(${package}_LIBRARIES 
        ${${package}_HOME}/lib/libCNamaSDK.dylib
        ${${package}_HOME}/lib/libfuai.dylib
    PARENT_SCOPE)
endfunction(FindNama)

FindNama(Nama)
