
add_library(Qt5::QWbmpPlugin MODULE IMPORTED)

_populate_Gui_plugin_properties(QWbmpPlugin RELEASE "imageformats/qwbmp.dll")
_populate_Gui_plugin_properties(QWbmpPlugin DEBUG "imageformats/qwbmpd.dll")

list(APPEND Qt5Gui_PLUGINS Qt5::QWbmpPlugin)
