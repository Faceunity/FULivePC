import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

//右侧美肤,美型,美体列表
Rectangle{
    property var icon_Name: ""
    property var icon_Full: ""
    property var t_Text: ""
    property bool b_Type: false  //false是滑动条,true是按钮
    property var i_Button_Num: 0 //按钮数量,滑动条为0
    property var i_Value: 0      //滑动条值或者选择第几个按钮
    property var b_RestoreDefault: false //恢复默认
    property var b_Middle: false //滑动条是否从中间开始,true中间[-50,50],false[0,100]
    signal iValueChanged(var value)
    signal clickRestoreDefault()
    x: 0
    y: 0
    width: 415
    height: 85
    function resetValue(ivalue) {
        i_Value = ivalue
        m_lButton.currentIndex = i_Button_Num - i_Value
    }
    //如果是精准美肤,选中关闭按钮时图标变灰,其他都是蓝
    function updataImage(ivalue){
        if(b_Type && m_text.text == "精准美肤"){
            if(ivalue == 0){
                icon_Full = "qrc:/res/list_icon_"+ icon_Name + "_close.png"
            }else{
                icon_Full = "qrc:/res/list_icon_"+ icon_Name + "_open.png"
            }
        }else if(b_Type && m_text.text !== "精准美肤"){
            icon_Full = "qrc:/res/list_icon_"+ icon_Name + "_open.png"
        }else{
            if(ivalue == 0){
                icon_Full = "qrc:/res/list_icon_"+ icon_Name + "_close.png"
            }else{
                icon_Full = "qrc:/res/list_icon_"+ icon_Name + "_open.png"
            }
        }
    }
    Component.onCompleted: {
        //如果有|则为按钮
        if(t_Text.indexOf("|") > 0){
            b_Type = true
        }
        if(t_Text == "恢复默认"){
            b_RestoreDefault = true
        }
        //分割字符 均匀磨皮|精细磨皮|朦胧磨皮|清晰磨皮 生成4个按钮
        if(b_Type){
            m_text.text = t_Text.substring(0,t_Text.indexOf(" "))
            var str2 = t_Text.substring(t_Text.indexOf(" ") + 1,t_Text.length)
            while(str2.indexOf("|") > 0)
            {
                var str3 = str2.substring(0, str2.indexOf("|"))
                m_lmButton.append({"tText": str3})
                str2 = str2.substring(str2.indexOf("|") + 1 , str2.length)
                i_Button_Num++
            }
            m_lmButton.append({"tText": str2})
            m_lButton.currentIndex = i_Button_Num - i_Value
        }
        updataImage(i_Value)
    }
    //显示恢复默认
    Rectangle{
        visible: b_RestoreDefault
        id: m_rDefault
        anchors.centerIn: parent
        width: 130
        height: 40
        border.color: "#969DB4"
        border.width: 2
        color:"#FFFFFF"
        radius: 8
        TextBlack{
            anchors.fill: parent
            text:"恢复默认"
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
                m_rDefault.color = "#98A5F5"
            }
            onExited: {
                m_rDefault.color = "#FFFFFF"
            }
            onClicked: {
                clickRestoreDefault()
            }
        }
    }
    //显示按钮组或者滑块
    Item{
        visible: !b_RestoreDefault
        ListView{
            id: m_lButton
            x: 115
            y: 15
            width: 280
            height: 35
            visible: b_Type
            spacing: 15
            orientation: ListView.Horizontal
            model: ListModel
            {
                id: m_lmButton
            }
            delegate: Rectangle{
                //鼠标是否处在此范围内
                property var b_inMouse: false
                id: m_rButton
                width: (280 - (m_lmButton.count - 1) * 15) / m_lmButton.count
                height: 35
                radius: 8
                color: b_inMouse ? "#7FB8FC" : (m_lButton.currentIndex == index ? "#7888EA" : "#E0E3EE")
                TextBlack {
                    id: m_tButton
                    anchors.fill: parent
                    text: tText
                    color:  m_lButton.currentIndex == index ? "#FFFFFF" : "#000000"
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
                    //按钮更改隐藏滑块值
                    onClicked: {
                        i_Value = i_Button_Num - index
                        m_lButton.currentIndex = index
                    }
                }
            }
        }
        Slider{
            id: m_slider
            x: 125
            y: 25
            width: 255
            height: 20
            maximumValue: b_Middle ? 50 : 100
            minimumValue: b_Middle ? -50 : 0
            stepSize: 1.0
            value: i_Value
            visible: !b_Type
            onValueChanged:
            {
                updataImage(value)
                m_tslider.text = value
                iValueChanged(value)
            }
            style: SliderStyle
            {
                //增加一段留白
                handle: Rectangle
                {
                    width: 20;
                    height: 16;
                    color:"#00FFFFFF"
                    Rectangle
                    {
                        anchors.centerIn: parent;
                        color: control.pressed ? "#4296FA":"#FFFFFF";
                        x:5
                        y:2
                        width: 10;
                        height: 16;
                    }
                }
                groove:Rectangle
                {
                    color: control.pressed ? "#7FB8FC" : "#E0E3EE"
                    radius:8
                    implicitWidth: 255
                    implicitHeight: 20
                }
            }
        }
        TextBlack{
            id: m_tslider
            anchors.centerIn: m_slider
            visible: !b_Type
            text: "0"
        }
        Image {
            id: m_image
            x: 36
            y: 5
            width: 52
            height: 52
            source: !b_RestoreDefault ? icon_Full : ""
        }
        TextBlack{
            id: m_text
            x: 35
            y: 60
            width: 55
            height: 15
            text: t_Text
        }
    }
}
