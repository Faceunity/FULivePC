import QtQuick 2.0

//底部道具列表 avator animoji 道具贴图等
Rectangle{
    property var icon_Name: ""
    property var b_Selected: false
    property var icon_Full: ""
    property alias t_Text: m_text.text
    signal selectedChange(var selectName);
    x: 0
    y: 0
    width: 100
    height: 150
    Component.onCompleted: {
        icon_Full = "qrc:/res/list_icon_"+ icon_Name + "_nor.png"
    }
    Rectangle{
        id: m_background
        x: 18
        y: 11
        width: 62
        height: 62
        radius: 90
        visible: false
        color: "#7989E6"
    }
    Image {
        id: m_image
        x: 23
        y: 16
        width: 52
        height: 52
        source: b_Selected ? "qrc:/res/list_icon_propmap_collapse.png" : icon_Full
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                m_background.visible = true
                icon_Full = "qrc:/res/list_icon_"+icon_Name + "_hover.png"
            }
            onExited: {
                m_background.visible = false
                icon_Full = "qrc:/res/list_icon_"+icon_Name + "_nor.png"
            }
            onClicked: {
                emit: selectedChange(icon_Name) //触发事件
            }
        }
    }
    TextBlack{
        id: m_text
        x: 10
        y: 75
        width: 80
        height: 30
    }
}
