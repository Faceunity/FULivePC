# Libcurl library import helper
function(FindLibcurl package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../libcurl ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    set(${package}_LIBRARIES
            ${${package}_HOME}/lib/libcurl.4.dylib
            ${${package}_HOME}/lib/libcrypto.3.dylib
            ${${package}_HOME}/lib/libssl.3.dylib
            ${${package}_HOME}/lib/librtmp.1.dylib
            ${${package}_HOME}/lib/libidn2.0.dylib
            ${${package}_HOME}/lib/libunistring.2.dylib

    PARENT_SCOPE)
endfunction(FindLibcurl)

FindLibcurl(libcurl)