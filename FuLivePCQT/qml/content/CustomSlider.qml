import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Slider{
    id: m_slider
    x: 70
    y: 500
    width: 275
    height: 20
    maximumValue: 100
    minimumValue: 0
    stepSize: 1.0
    value: 40
    onValueChanged:
    {
    }
    style: SliderStyle
    {
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
