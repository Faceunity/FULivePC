import QtQuick 2.0
import QtQuick.Window 2.0
import OpenGLUnderQML 1.0

Window {
    id:m_window
    title: "FU Live Demo PC Sim"
    visible: true
    x: 200
    y: 100
    width: 864
    height: 486
    CameraDisplay{
        id:m_CameraDisplay
        x: 0
        y: 0
        width: 864
        height: 486
        onWidthChanged: {
            UIBridge.cameraWidth = width
        }
        onHeightChanged: {
            UIBridge.cameraHeight = height
        }
    }
}
