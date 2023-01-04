import QtQuick 2.0

//底部道具列表 avator animoji 道具贴图等
Rectangle{
    property var icon_Name: ""
    property var b_Enabled: false
    //选中展示状态
    property var b_Selected: false
    //有选中道具
    property var b_SelectedItem: false
    property var icon_Full: ""
    property alias t_Text: m_text.text
    signal selectedChange(var selectName);
    x: 0
    y: 0
    width: 90
    height: 120
    enabled: b_Enabled
    Component.onCompleted: {
        icon_Full = "qrc:/res/list_icon_" + icon_Name + "_nor.png"
    }
    Image {
        id: m_image
        x: 14
        y: 11
        width: 62
        height: 62
        source: !b_Enabled ? "qrc:/res/list_icon_" + icon_Name + "_dis.png":
                             b_Selected ? "qrc:/res/list_icon_propmap_collapse.png" :
                             b_SelectedItem ? "qrc:/res/list_icon_" + icon_Name + "_hover.png" : icon_Full
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                icon_Full = "qrc:/res/list_icon_" + icon_Name + "_hover.png"
            }
            onExited: {
                icon_Full = "qrc:/res/list_icon_" + icon_Name + "_nor.png"
            }
            onClicked: {
                emit: selectedChange(icon_Name) //触发事件
            }
        }
    }
    TextBlack{
        id: m_text
        x: 0
        y: 75
        width: 90
        height: 30
    }
}
