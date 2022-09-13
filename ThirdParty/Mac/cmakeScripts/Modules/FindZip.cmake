# Zip library import helper
function(FindZip package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../libzip ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
	if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
	    set(${package}_LIBRARIES
			${${package}_HOME}/lib/x86_64/libz.dylib
			${${package}_HOME}/lib/x86_64/libzip.dylib
		PARENT_SCOPE)
	 else()
		set(${package}_LIBRARIES
			${${package}_HOME}/lib/arm64/libzstd.1.5.2.dylib
			${${package}_HOME}/lib/arm64/libzip.dylib
		PARENT_SCOPE)
	endif()
endfunction(FindZip)

FindZip(Zip)
