import QtQuick 2.0

Rectangle{
    property alias t_Text: m_text.text
    property var b_Selected: false
    property var b_inMouse: false
    width: 120
    height: 40
    radius: 8
    color: b_Selected ? "#7888EA" : (b_inMouse ? "#7888EA" : "#FFFFFF")
    border.color: enabled ? "#969DB5" : "#E1E1E1"
    border.width: 3
    //外部修改点击事件
    function onButtonClick(){
    }
    TextBlack{
        id: m_text
        anchors.centerIn: parent
        color: enabled ? "#000000": "#E1E1E1"
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            b_inMouse = true
        }
        onExited: {
            b_inMouse = false
        }
        onClicked: {
            onButtonClick()
        }
    }
}
