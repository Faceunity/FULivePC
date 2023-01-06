import QtQuick 2.0

Rectangle{
    property bool b_Selected: false
    property var v_Color: "#00FF00"
    //外部修改点击事件
    function onButtonClick(){
    }
    id: m_rGreen
    width: 26
    height: 26
    radius: 90
    border.color: b_Selected ? "#959CB4" : "#E9EAF2"
    border.width: 2
    Rectangle{
        x: 3
        y: 3
        width: 20
        height: 20
        radius: 90
        color: v_Color
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
            onButtonClick()
        }
    }
}
