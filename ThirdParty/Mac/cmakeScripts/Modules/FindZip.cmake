# Zip library import helper
function(FindZip package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../libzip ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    set(${package}_LIBRARIES 
        ${${package}_HOME}/lib/libz.1.2.11.dylib
        ${${package}_HOME}/lib/libzip.5.dylib
    PARENT_SCOPE)
endfunction(FindZip)

FindZip(Zip)
