
add_library(Qt5::QTiffPlugin MODULE IMPORTED)

_populate_Gui_plugin_properties(QTiffPlugin RELEASE "imageformats/qtiff.dll")
_populate_Gui_plugin_properties(QTiffPlugin DEBUG "imageformats/qtiffd.dll")

list(APPEND Qt5Gui_PLUGINS Qt5::QTiffPlugin)
