import QtQuick 2.0

Rectangle{
    id: m_rDefault
    property var t_Text: ""
    //外部修改点击事件
    function onButtonClick(){
    }
    color: "#7787E9"
    radius: 20
    TextBlack{
        anchors.fill: parent
        color: "#FFFFFF"
        text: t_Text
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
           onButtonClick()
        }
    }
}
