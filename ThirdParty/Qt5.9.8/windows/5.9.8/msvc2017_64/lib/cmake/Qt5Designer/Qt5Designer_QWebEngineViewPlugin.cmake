
add_library(Qt5::QWebEngineViewPlugin MODULE IMPORTED)

_populate_Designer_plugin_properties(QWebEngineViewPlugin RELEASE "designer/qwebengineview.dll")

list(APPEND Qt5Designer_PLUGINS Qt5::QWebEngineViewPlugin)
