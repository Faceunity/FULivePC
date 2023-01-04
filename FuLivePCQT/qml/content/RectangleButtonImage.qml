import QtQuick 2.0
Rectangle{
    property var t_Text: ""
    property var v_Source_nor: ""
    property var v_Source_sel: ""
    property var b_Hover: false
    property var b_Selected: false
    property var b_Press: false
    property var v_Color_nor: "#959CB4"
    property var v_Color_press: "#797F90"
    property var v_Color_sel: "#7787E9"
    //外部修改点击事件
    function onButtonClick(){
    }
    id: m_bri
    width: 84
    height: 34
    radius: 5
    color: b_Hover ? "#F6F6FA" : "#FFFFFF"
    border.color: b_Selected ? v_Color_sel : b_Press ? v_Color_press : v_Color_nor
    Image{
        x: 17
        y: (m_bri.height - 14) / 2
        width: 14
        height: 14
        source: b_Selected ? v_Source_sel : v_Source_nor
    }
    TextBlack{
        x: 25
        y: 0
        width: m_bri.width - 25
        height: m_bri.height
        color: b_Selected ? v_Color_sel : b_Press ? v_Color_press : v_Color_nor
        text: t_Text
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
