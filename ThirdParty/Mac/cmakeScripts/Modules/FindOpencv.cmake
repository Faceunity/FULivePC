# Opencv library import helper
function(FindOpencv package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../opencv ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
	if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "x86_64")
	    set(${package}_LIBRARIES
			${${package}_HOME}/lib/x86_64/libopencv_core.dylib
			${${package}_HOME}/lib/x86_64/libopencv_highgui.dylib
			${${package}_HOME}/lib/x86_64/libopencv_imgcodecs.dylib
			${${package}_HOME}/lib/x86_64/libopencv_imgproc.dylib
			${${package}_HOME}/lib/x86_64/libopencv_video.dylib
			${${package}_HOME}/lib/x86_64/libopencv_videoio.dylib
		PARENT_SCOPE)
	 else()
		set(${package}_LIBRARIES

			# ${${package}_HOME}/lib/arm64/libopencv_core.3.4.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_highgui.3.4.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_imgcodecs.3.4.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_imgproc.3.4.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_video.3.4.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_videoio.3.4.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_core.4.5.1.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_highgui.4.5.1.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_imgcodecs.4.5.1.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_imgproc.4.5.1.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_video.4.5.1.dylib
			# ${${package}_HOME}/lib/arm64/libopencv_videoio.4.5.1.dylib
			${${package}_HOME}/lib/arm64/libopencv_core.dylib
			${${package}_HOME}/lib/arm64/libopencv_highgui.dylib
			${${package}_HOME}/lib/arm64/libopencv_imgcodecs.dylib
			${${package}_HOME}/lib/arm64/libopencv_imgproc.dylib
			${${package}_HOME}/lib/arm64/libopencv_video.dylib
			${${package}_HOME}/lib/arm64/libopencv_videoio.dylib

			PARENT_SCOPE)
	endif()
endfunction(FindOpencv)

FindOpencv(Opencv)
