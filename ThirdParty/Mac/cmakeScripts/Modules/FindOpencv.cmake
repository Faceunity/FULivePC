# Opencv library import helper
function(FindOpencv package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../opencv ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    set(${package}_LIBRARIES
    	${${package}_HOME}/lib/libopencv_core.3.4.dylib
    	${${package}_HOME}/lib/libopencv_highgui.3.4.dylib
    	${${package}_HOME}/lib/libopencv_imgcodecs.3.4.dylib
    	${${package}_HOME}/lib/libopencv_imgproc.3.4.dylib
    	${${package}_HOME}/lib/libopencv_video.3.4.dylib
    	${${package}_HOME}/lib/libopencv_videoio.3.4.dylib
    PARENT_SCOPE)
endfunction(FindOpencv)

FindOpencv(Opencv)
