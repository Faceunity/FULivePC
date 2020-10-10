# Ffmpeg library import helper
function(FindFfmpeg package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../ffmpeg ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    set(${package}_LIBRARIES
    	${${package}_HOME}/lib/libswresample.a
   	${${package}_HOME}/lib/libavutil.a
    	${${package}_HOME}/lib/libavcodec.a
    	${${package}_HOME}/lib/libavformat.a
 

    PARENT_SCOPE)
endfunction(FindFfmpeg)

FindFfmpeg(ffmpeg)
