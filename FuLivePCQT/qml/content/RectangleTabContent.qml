import QtQuick 2.0

//右侧上方标题 美肤,美型,滤镜,美体,对应i_Tab_ID 0,1,2,3
Rectangle{
    property alias t_Text: m_text.text
    property var b_Selected: false
    property var i_Tab_ID: -1
    property var b_inMouse: false
    property var b_GreenScreen: false
    signal selectedChange(var id);
    id: m_tab
    width: b_GreenScreen ? 140 : 105
    height: 30
    color: b_Selected ? "#FFFFFF" : "#2D2956"
    TextBlack{
        id: m_text
        anchors.fill: parent
        color: b_Selected ? "#2F3658" : b_inMouse ? "#FFFFFF" : "#B3FFFFFF"
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
            selectedChange(i_Tab_ID)
        }
    }
}
