# Ffmpeg library import helper
function(FindFfmpeg package)
	set(${package}_FOUND ON PARENT_SCOPE)
	get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../ffmpeg ABSOLUTE)

	if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(${package}_INCLUDE_DIRS ${${package}_HOME}/x86_64/include PARENT_SCOPE)
		set(${package}_LIBRARIES
			${${package}_HOME}/x86_64/lib/libswresample.2.dylib
			${${package}_HOME}/x86_64/lib/libavutil.55.dylib
			${${package}_HOME}/x86_64/lib/libavcodec.57.dylib
			${${package}_HOME}/x86_64/lib/libavformat.57.dylib
			PARENT_SCOPE)
	else()
		set(${package}_INCLUDE_DIRS ${${package}_HOME}/arm64/include PARENT_SCOPE)
		set(${package}_LIBRARIES
			${${package}_HOME}/arm64/lib/libswresample.dylib
			${${package}_HOME}/arm64/lib/libavutil.dylib
			${${package}_HOME}/arm64/lib/libavcodec.dylib
			${${package}_HOME}/arm64/lib/libavformat.dylib
			PARENT_SCOPE)
	endif()
endfunction(FindFfmpeg)

FindFfmpeg(ffmpeg)
