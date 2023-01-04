import QtQuick 2.0

Rectangle{
    id: mrbt
    property alias t_Text: m_text.text
    property var b_Hover: false
    property var b_Press: false
    property var b_Select: false
    property var v_Color_Nor: "#959CB4" //浅灰色
    property var v_Color_Press: "#797F90" //深灰色
    property var v_Color_Select: "#7787E9" //蓝色
    width: 126
    height: 32
    radius: 5
    color: b_Select ? v_Color_Select : b_Hover ? "#F6F6FA" : "#FFFFFF"
    border.color: b_Press ? v_Color_Press : v_Color_Nor
    border.width: b_Select ? 0 : 1
    opacity: mrbt.enabled ? 1 : 0.3
    //外部修改点击事件
    function onButtonClick(){
    }
    TextBlack{
        id: m_text
        anchors.centerIn: parent
        color: b_Select ? "#FFFFFF" : b_Press ? v_Color_Press : v_Color_Nor
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            b_Hover = true
        }
        onExited: {
            b_Hover = false
        }
        onClicked: {
            b_Press = true
            onButtonClick()
            b_Press = false
        }
    }
}
