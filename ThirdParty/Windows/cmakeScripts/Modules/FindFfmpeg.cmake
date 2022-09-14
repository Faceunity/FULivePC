# ffmpeg library import helper
function(Findffmpeg package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../ffmpeg ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win32/avcodec.lib
		${${package}_HOME}/lib/win32/avdevice.lib		
		${${package}_HOME}/lib/win32/avfilter.lib
		${${package}_HOME}/lib/win32/avformat.lib 
		${${package}_HOME}/lib/win32/avutil.lib 
		${${package}_HOME}/lib/win32/swresample.lib 
		${${package}_HOME}/lib/win32/swscale.lib
        PARENT_SCOPE)
    else()
        set(${package}_LIBRARIES ${${package}_HOME}/lib/win64/avcodec.lib
		${${package}_HOME}/lib/win64/avdevice.lib		
		${${package}_HOME}/lib/win64/avfilter.lib
		${${package}_HOME}/lib/win64/avformat.lib 
		${${package}_HOME}/lib/win64/avutil.lib 
		${${package}_HOME}/lib/win64/swresample.lib 
		${${package}_HOME}/lib/win64/swscale.lib
		PARENT_SCOPE)

    endif()
endfunction(Findffmpeg)

Findffmpeg(ffmpeg)
