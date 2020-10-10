# Glew library import helper
function(FindGlfw package)
    set(${package}_FOUND ON PARENT_SCOPE)
    get_filename_component(${package}_HOME ${CMAKE_CURRENT_LIST_DIR}/../../glfw ABSOLUTE)
    set(${package}_INCLUDE_DIRS ${${package}_HOME}/include PARENT_SCOPE)
    set(${package}_LIBRARIES ${${package}_HOME}/lib/libglfw.3.3.dylib PARENT_SCOPE)
endfunction(FindGlfw)

FindGlfw(Glfw)
