import QtQuick 2.0

//底部道具列表点击弹出列表 白猫,斗牛犬,青蛙等
Rectangle{
    property var icon_Name: ""
    property var b_Selected: false
    property var b_path_type: true //true用qrc资源路径,false用运行当前路径
    property var icon_Full: ""
    signal selectedChange(var selectName);
    x: 0
    y: 0
    width: 80
    height: 80
    color: "#00ffffff"
    Component.onCompleted: {
        if(b_path_type){
            icon_Full = "qrc:/res/"+ icon_Name + ".png"
        }else{
            icon_Full = ""
            icon_Full = "file:./" + icon_Name + ".png"
        }
    }
    Rectangle{
        id: m_background
        x: 6
        y: 6
        width: 68
        height: 68
        radius: 90
        visible: false
        color: "#FFFFFF"
    }
    Rectangle{
        id: m_selectCircle
        x: 6
        y: 6
        width: 68
        height: 68
        radius: 90
        visible: b_Selected
        color: "#00ffffff"
        border.color: "#7787E9"
        border.width: 3
    }
    Image {
        id: m_image
        x: 10
        y: 10
        width: 60
        height: 60
        source: icon_Full
        cache:false
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                m_background.visible = true
            }
            onExited: {
                m_background.visible = false
            }
            onClicked: {
                emit: selectedChange(icon_Name) //触发事件
            }
        }
    }
}
