import QtQuick 2.5
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
    property bool b_TextEdit: false //避免循环修改文本滑块
    property alias m_lButton: m_lButton
    signal iValueChanged(var value)
    x: 0
    y: 0
    width: 420
    height: 85
    function resetValue(ivalue) {
        i_Value = ivalue
        m_lButton.currentIndex = i_Button_Num - i_Value
    }
    function defaultClick(){
    }
    //如果是精准美肤,选中关闭按钮时图标变灰,其他都是蓝
    function updateImage(ivalue){
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
        updateImage(i_Value)
    }
    //显示恢复默认
    RectangleButtonText{
        visible: b_RestoreDefault
        id: m_rDefault
        x: 145
        y: 40
        t_Text: "恢复默认"
        function onButtonClick() {
            defaultClick()
        }
    }
    //显示按钮组或者滑块
    Item{
        visible: !b_RestoreDefault
        ListView{
            id: m_lButton
            x: 115
            y: 15
            width: 290
            height: 35
            visible: b_Type
            spacing: 8
            orientation: ListView.Horizontal
            model: ListModel
            {
                id: m_lmButton
            }
            delegate: Rectangle{
                //鼠标是否处在此范围内
                property var b_hover: false
                id: m_rButton
                width: m_lmButton.count == 2 ? 100 : (290 - (m_lmButton.count - 1) * 8) / m_lmButton.count
                height: 35
                radius: 5
                color: m_lButton.currentIndex == index ? "#7787E9" : "#FFFFFF"
                border.color: m_lButton.currentIndex == index ? "#FFFFFF" : b_hover ?"#7787E9" : "#959CB4"
                border.width: 1
                TextBlack {
                    id: m_tButton
                    anchors.fill: parent
                    text: tText
                    color:  m_lButton.currentIndex == index ? "#FFFFFF" : b_hover ?"#7787E9" : "#959CB4"
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        b_hover = true
                    }
                    onExited: {
                        b_hover = false
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
            wheelEnabled :false
            x: 105
            y: 17
            width: 250
            height: 24
            maximumValue: b_Middle ? 50 : 100
            minimumValue: b_Middle ? -50 : 0
            stepSize: 1.0
            value: i_Value
            visible: !b_Type
            onValueChanged:
            {
                updateImage(value)
                if(!b_TextEdit){
                    m_tslider.text = value
                }
                iValueChanged(value)
            }
            style: SliderStyle
            {
                handle:Rectangle
                {
                    id: m_ssh
                    anchors.centerIn: parent
                    color: "#FFFFFF"//control.pressed ? "#4296FA":"#FFFFFF"
                    border.color: "#46437133"
                    border.width: 1
                    radius: 90
                    width: 24
                    height: 24
                }

                groove:Rectangle
                {
                    implicitWidth: 250
                    implicitHeight: 8
                    color: "#FFFFFF"
                    Rectangle{
                        x: 10
                        y: 0
                        width: 230
                        height: 8
                        color: "#E0E3EE" //control.pressed ? "#7FB8FC" : "#E0E3EE"
                        radius: 5
                        Rectangle{
                            x: b_Middle ? (m_slider.value > 0) ? 115 : 115 + 2.3 * m_slider.value : 0
                            y: 0
                            width: b_Middle ? 2.3 * Math.abs(m_slider.value) : 2.3 * m_slider.value
                            height: 8
                            color: "#7787E9"
                            radius: 5
                        }
                    }
                }
            }
        }

        Rectangle{
            id: m_editR
            x: 365
            y: 17
            width: 32
            height: 28
            border.color: "#E0E3EE"
            border.width: 1
            radius: 5
            visible: !b_Type
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    m_editR.border.color = "#7787E9"
                }
                onExited: {
                    if(!m_tslider.activeFocus){
                        m_editR.border.color = "#E0E3EE"
                    }
                }
            }
            TextInput{
                id: m_tslider
                text: "0"
                anchors.fill: parent
                color: "#5C6071"
                font {family: "微软雅黑"; pixelSize: 14;}
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                validator: IntValidator{bottom: b_Middle ? -50 : 0; top: b_Middle ? 50 : 100;}
                onEditingFinished:{
                    m_tslider.focus = false
                    b_TextEdit = true
                    i_Value = text
                    b_TextEdit = false
                    m_editR.border.color = "#E0E3EE"
                }
            }
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
