# Ffmpeg library import helper
function(FindFfmpeg package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../ffmpeg ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
	
  if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
	    set(${package}_LIBRARIES
    	${${package}_HOME}/lib/x86_64/libswresample.2.dylib
		${${package}_HOME}/lib/x86_64/libavutil.55.dylib
    	${${package}_HOME}/lib/x86_64/libavcodec.57.dylib
    	${${package}_HOME}/lib/x86_64/libavformat.57.dylib
    PARENT_SCOPE)
	else()
		set(${package}_LIBRARIES
		${${package}_HOME}/lib/arm64/libswresample.4.dylib
		${${package}_HOME}/lib/arm64/libavutil.57.dylib
    	${${package}_HOME}/lib/arm64/libavcodec.59.dylib
    	${${package}_HOME}/lib/arm64/libavformat.59.dylib
		PARENT_SCOPE)
	endif()
	
endfunction(FindFfmpeg)

FindFfmpeg(ffmpeg)
