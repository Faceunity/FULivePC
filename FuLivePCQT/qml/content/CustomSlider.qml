import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Slider{
    id: m_slider
    maximumValue: 100
    minimumValue: 0
    stepSize: 1.0
    onValueChanged:
    {
    }
    style: SliderStyle
    {
        handle:Rectangle
        {
            id: m_ssh
            anchors.centerIn: parent
            color: "#FFFFFF"
            border.color: "#46437133"
            border.width: 1
            radius: 90
            width: 24
            height: 24
        }

        groove:Rectangle
        {
            implicitWidth: m_slider.width
            implicitHeight: 8
            color: "#FFFFFF"
            Rectangle{
                id: m_rs
                x: 10
                y: 0
                width: m_slider.width - 20
                height: 8
                color: "#E0E3EE"
                radius: 5
                Rectangle{
                    x: 0
                    y: 0
                    width: (m_slider.width - 20) / 100 * m_slider.value
                    height: 8
                    color: "#7787E9"
                    radius: 5
                }
            }
        }
    }
    Rectangle{
        id: m_editR
        x: m_slider.width + 10
        y: 0
        width: 32
        height: 28
        border.color: "#E0E3EE"
        border.width: 1
        radius: 5
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
            text: m_slider.value
            anchors.fill: parent
            color: "#5C6071"
            font {family: "微软雅黑"; pixelSize: 14;}
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            validator: IntValidator{bottom: 0; top: 100;}
            onEditingFinished:{
                m_tslider.focus = false
                m_slider.value = text
                m_editR.border.color = "#E0E3EE"
            }
        }
    }
}
