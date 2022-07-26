import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.1
import QtQuick.Controls 1.2 as Controls12
import QtQuick.Controls.Styles 1.2 as Styles12
import QtQuick.Controls 1.4 as Controls14
import QtQuick.Controls.Styles 1.4 as Styles14
import OpenGLUnderQML 1.0
import QtMultimedia 5.4
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import "content"

Window {
    id:m_window
    title: "FU Live Demo PC"
    visible: true
    x: bFullScreen ? 0 : 200
    y: bFullScreen ? 10 : 100
    width: bFullScreen ? Screen.desktopAvailableWidth : 1360
    height: bFullScreen ? Screen.desktopAvailableHeight : 830
    flags: bFullScreen ? Qt.FramelessWindowHint | Qt.Window : Qt.Window
    //渲染画面全屏
    property var bFullScreen: false
    property var xScale: 1.0
    property var yScale: 1.0
    //是否选中AR功能
    property var b_ARFunction: true
    property var i_avatarType: -1
    //记住上一次选中了avatar半身还是全身
    property var i_avatarTypeShow: 1
    //绿幕是否已经选择摄像头或者视频
    property var b_setGreenScreen: false
    //底部选中第几个道具项
    property var i_selectCategory: -1;
    //AR弹出道具选中第几个底部项,第几个弹出项目
    property var i_arSelectCategoryPoint: Qt.point(-1,-1);
    //绿幕背景视频选中第几个底部项
    property var i_gsSelectVideo: 0
    //绿幕安全区域选中第几个底部项
    property var i_gsSelectSaveArea: -1
    //选中精品贴纸第几个项目
    property var i_selectSticker: 0
    //渲染美体
    property var b_arBody: false
    //道具选项下标
    property var i_category_avatar: 0;
    property var i_category_jingpin: 3;
    property var i_category_backgroundSegmentation: 7;
    property var i_category_makeup: 10;
    property var i_category_lightMakeup: 11;
    property var i_category_greenScreen: 14;
    property var i_category_safeArea: 15;
    //窗体大小改变
    onWidthChanged: {
        xScale = width * 1.0 / 1360.0
    }
    onHeightChanged: {
        yScale = height * 1.0  / 830.0
    }
    Component.onCompleted: {
        updataProp()
        updataBeautySkin()
        updataBeautyFace()
        updataBeautyBody()
        updataFilter()
        updataGreenScreen()
        loadCustomMakeupTitle()
        UIBridge.getCameraList();
        UIBridge.tipChanged.connect(tipTimerStart);
        UIBridge.arBodyFlagChanged.connect(arBodyFlagChanged);
        UIBridge.callQmlRefeshImg.connect(callQmlRefeshImg)
        UIBridge.selectColorChanged.connect(selectColorChanged)
        UIBridge.finishDownload.connect(finishDownload)
        UIBridge.switchARChanged.connect(switchARFunction)
    }
    Connections {
        target: UIBridge
        onUpdataGreenScreenParam:{
            updataGreenScreen()
        }
        onUpdataBsgPic: {
            //选中第二个背景图片
            i_arSelectCategoryPoint.y = 1
            //更新自定义背景图片
            updataPropOption(i_category_backgroundSegmentation)
        }
        onUpdataGSSafePic: {
            //选中第二个绿幕安全图片
            i_gsSelectSaveArea = 1
            //更新自定义绿幕安全区域图片
            updataPropOption(i_category_safeArea - i_category_greenScreen)
        }
        onUpdataCameraIndex:{
            m_cCamera.currentIndex = index
        }
        onUpdataCameraSet:{
            m_cCameraSet.currentIndex = index
        }
        onUpdataBodyTrackType:{
            i_avatarType = type
        }
        onUpdataSelectCategory:{
            if(x < i_category_greenScreen){
                i_arSelectCategoryPoint = Qt.point(x,y);
                i_selectCategory = x
                if(x == 0 && y == 0){
                    showAvator(true);
                }else if(x == i_category_jingpin){
                    i_selectSticker = z
                    m_lStickerTips.currentIndex = z
                }else{
                    updataPropOption(x);
                }
            }else{
                b_ARFunction = false
            }
        }
        onUpdataGSSelectIndex:{
            b_setGreenScreen = true
            UIBridge.arFunction = false
            switchARFunction(false)
            i_gsSelectVideo = x
            updataPropOption(0)
            i_gsSelectSaveArea = y
            if(y >= 0){
                updataPropOption(1)
            }
        }
    }
    Timer{
        id: timerLoadTime
        interval: 2000
        repeat: false
        onTriggered:{
            m_rTipText.visible = false
        }
    }
    //刷新绿幕,avator显示图片
    function callQmlRefeshImg(){
        if(b_ARFunction){
            m_showImage.source = ""
            m_showImage.source = "image://ShowImage/" + Math.random()
        }else{
            m_showImageCamera.source = ""
            m_showImageCamera.source = "image://ShowImage/" + Math.random()
        }
    }
    //美体滑块滑动时ar道具列表不能使用,遮挡
    function arBodyFlagChanged(arBody){
        b_arBody = arBody
        m_rPropShadow.visible = arBody
        if(i_arSelectCategoryPoint.x == i_category_jingpin){
            m_rBoutiqueSticker.visible = !arBody
        }else if(i_arSelectCategoryPoint.x > 0){
            m_rpropOption.visible = !arBody
        }
    }
    //显示提示文字计时器开始
    function tipTimerStart(){
        m_rTipText.visible = true
        timerLoadTime.start()
    }
    //AR功能/绿幕切换
    function switchARFunction(arfun){
        b_ARFunction = arfun
        updataProp()
        m_rpropOption.visible = false
        m_rPropShadow.visible = false
        m_rBoutiqueSticker.visible = false
        showGreenScreenList(false)
        if(!arfun){
            m_rARbackgroud.x = 137
            showAvator(false);
            m_lProp.x = 380
            m_lProp.contentX = 0
            //第一次没选择摄像头或视频时遮挡
            m_rGreenScreen.visible = !b_setGreenScreen
            m_rPropShadow.visible = !b_setGreenScreen
            m_maCameraDisplay.enabled = true
            //绿幕默认选中第一个
            if(i_gsSelectSaveArea >= 0){
                i_selectCategory = i_category_safeArea - i_category_greenScreen
                updataPropOption(i_category_safeArea - i_category_greenScreen)
            }else{
                i_selectCategory = 0
                updataPropOption(0)
            }
            //如果切绿幕在美体,切成美肤
            if(m_rBeautyBody.b_Selected == true){
                updataTapContent(0)
            }
            m_rSelectGreenScreen.visible = true
        }else{
            m_rARbackgroud.x = 2
            m_lProp.x = 0
            m_lProp.contentX = 0
            m_rGreenScreen.visible = false
            m_greenScreenSelectCamera.visible = false
            m_maCameraDisplay.enabled = false
            //切换回之前状态
            if(i_arSelectCategoryPoint != Qt.point(-1,-1)){
                m_rpropOption.visible = true
                m_lmProp.get(i_arSelectCategoryPoint.x).selected = true
                i_selectCategory = i_arSelectCategoryPoint.x
                UIBridge.selectCategory = i_arSelectCategoryPoint.x
                updataPropOption(i_arSelectCategoryPoint.x)
                if(i_arSelectCategoryPoint.x == 0){
                    showAvator(true)
                }else if(i_arSelectCategoryPoint.x == i_category_jingpin){
                    showBoutiqueSticker(true)
                    UIBridge.downloadSticker(i_arSelectCategoryPoint.y)
                }
            }
            m_rSelectGreenScreen.visible = false
            //如果选择美体
            if(b_arBody){
                m_rPropShadow.visible = b_arBody
                if(i_arSelectCategoryPoint.x == i_category_jingpin){
                    m_rBoutiqueSticker.visible = !b_arBody
                    m_rpropOption.visible = false
                }else{
                    m_rpropOption.visible = !b_arBody
                }
            }
        }
    }
    //精品贴纸里道具下载完成,下载图标不显示
    function finishDownload(index){
        if(m_lmStickerBundles.count >= index)
            m_lmStickerBundles.get(index).download = true;
    }
    //显示精品贴纸
    function showBoutiqueSticker(flag){
        if(flag){
            m_rBoutiqueSticker.visible = true
            m_lStickerBundles.contentX = 0
            if(m_lmStickerTips.count == 0){
                m_lmStickerTips.clear()
                for(var i = 0; i < UIBridge.stickerTip.length; i++)
                {
                    if(i === 0){
                        m_lmStickerTips.append({"tipText": UIBridge.stickerTip[i], "selected": true})
                    }else{
                        m_lmStickerTips.append({"tipText": UIBridge.stickerTip[i], "selected": false})
                    }
                }
            }
            updataBoutiqueSticker(i_selectSticker)
        }else{
            m_rBoutiqueSticker.visible = false
        }
    }
    //精品贴纸选择中级道具|高级道具|效果验证|游戏道具
    function updataBoutiqueSticker(index){
        UIBridge.setStickerIndex(index)
        m_lmStickerBundles.clear()
        for(var i = 0; i < UIBridge.getStickerLength(); i++){
            if(3 === i_arSelectCategoryPoint.x && i === i_arSelectCategoryPoint.y && i_selectSticker === index){
                m_lmStickerBundles.append({"icon": UIBridge.getStickerValue(i,0),"download": UIBridge.getStickerValue(i,1),
                                              "ondownload": UIBridge.getStickerValue(i,2),"selected": true})
            }else{
                m_lmStickerBundles.append({"icon": UIBridge.getStickerValue(i,0),"download": UIBridge.getStickerValue(i,1),
                                              "ondownload": UIBridge.getStickerValue(i,2), "selected": false})
            }
        }
    }
    //显示全身|半身
    function showAvator(select){
        if(select){
            UIBridge.bodyTrackType = i_avatarTypeShow
            i_avatarType = i_avatarTypeShow
        }else{
            i_avatarType = -1
            UIBridge.bodyTrackType = -1
        }
    }
    //true显示右侧绿幕选项,false显示常规右侧参数选项
    function showGreenScreenList(flag){
        m_rSideGreenScreenWindow.visible = flag
        m_rSideWindow.visible = !flag
    }
    //更新道具列表
    function updataProp(){
        m_lmProp.clear()
        if(b_ARFunction){
            m_lmProp.append( { icon: "avatar", text:"Avatar", selected: false })
            m_lmProp.append( { icon: "annimoji", text:"Animoji", selected: false })
            m_lmProp.append( { icon: "Propmap", text:"道具贴图" , selected: false})
            m_lmProp.append( { icon: "Jinpin", text:"精品贴纸" , selected: false})
            m_lmProp.append( { icon: "AR", text:"AR面具", selected: false })
            m_lmProp.append( { icon: "Expressionrecognition", text:"表情识别", selected: false })
            m_lmProp.append( { icon: "Musicfilter", text:"音乐滤镜", selected: false })
            m_lmProp.append( { icon: "Bgsegmentation", text:"人像分割", selected: false })
            m_lmProp.append( { icon: "gesturerecognition", text:"手势识别", selected: false })
            m_lmProp.append( { icon: "Hahamirror", text:"哈哈镜", selected: false })
            m_lmProp.append( { icon: "makeup", text:"美妆", selected: false })
            m_lmProp.append( { icon: "lightmakeup", text:"轻美妆", selected: false })
            m_lmProp.append( { icon: "hairdressing", text:"美发", selected: false })
            m_lmProp.append( { icon: "photo_sticker", text:"搞笑大头", selected: false })
        }else{
            m_lmProp.append( { icon: "green_screen_cutout", text:"绿幕", selected: false })
            m_lmProp.append( { icon: "safe_area", text:"安全区域", selected: false })
        }
    }
    //更新底部弹出框选项
    function updataPropOption(index){
        m_propOptionListView.contentX = 0
        m_lmPropOption.clear()
        if(b_ARFunction){
            for(var i = 0; i< UIBridge.categoryBundles[index].length; i++)
            {
                var iconValue = UIBridge.categoryBundles[index][i];
                var pathType = true;
                //如果是人像分割
                if(index === i_category_backgroundSegmentation && iconValue === "bg_seg_shot"){
                    pathType = false;
                }
                if(index === i_arSelectCategoryPoint.x && i === i_arSelectCategoryPoint.y){
                    m_lmPropOption.append({"icon": iconValue, "selected": true, "pathType": pathType})
                }else{
                    m_lmPropOption.append({"icon": iconValue, "selected": false, "pathType": pathType})
                }
            }
        }else{
            for(var j = 0; j< UIBridge.greenScreenIcon[index].length; j++)
            {
                var pathType1 = true;
                var iconValue1 = UIBridge.greenScreenIcon[index][j];
                if(index === (i_category_safeArea - i_category_greenScreen) && iconValue1 === "gs_savearea_shot"){
                    pathType1 = false;
                }
                if((index === 0 && j === i_gsSelectVideo) || (index === 1 && j === i_gsSelectSaveArea)){
                    m_lmPropOption.append({"icon": iconValue1, "selected": true, "pathType": pathType1})
                }else{
                    m_lmPropOption.append({"icon": iconValue1, "selected": false, "pathType": pathType1})
                }
            }
        }
    }
    //读取美肤
    function updataBeautySkin(){
        m_lmBeautySkin.clear()
        for(var i = 0; i< UIBridge.beautySkin[0].length; i++)
        {
            m_lmBeautySkin.append({"text":UIBridge.beautySkin[0][i], "icon":UIBridge.beautySkin[1][i], "value":UIBridge.beautySkin[3][i]})
        }
        m_lmBeautySkin.append({"text": "恢复默认", "icon": "", "value": ""})
    }
    //读取美型
    function updataBeautyFace(){
        m_lmBeautyFace.clear()
        for(var i = 0; i< UIBridge.beautyFace[0].length; i++)
        {
            m_lmBeautyFace.append({"text":UIBridge.beautyFace[0][i], "icon":UIBridge.beautyFace[1][i],
                                      "value":UIBridge.beautyFace[3][i], "middle":UIBridge.beautyFace[4][i]})
        }
        m_lmBeautyFace.append({"text": "恢复默认", "icon": "", "value": "", "middle": ""})
    }
    //读取滤镜
    function updataFilter(){
        m_lmFilter.clear()
        for(var i = 0; i< UIBridge.filter[0].length; i++)
        {
            m_lmFilter.append({"icon":UIBridge.filter[0][i], "value":UIBridge.filter[1][i]})
        }
    }
    //读取美体
    function updataBeautyBody(){
        m_lmBeautyBody.clear()
        for(var i = 0; i< UIBridge.beautyBody[0].length; i++)
        {
            m_lmBeautyBody.append({"text":UIBridge.beautyBody[0][i], "icon":UIBridge.beautyBody[1][i],
                                      "value":UIBridge.beautyBody[3][i], "middle":UIBridge.beautyBody[4][i]})
        }
        m_lmBeautyBody.append({"text": "恢复默认", "icon": "", "value": "", "middle": ""})
    }
    //读取绿幕参数
    function updataGreenScreen(){
        m_lmGreenScreen.clear()
        for(var i = 0; i< UIBridge.greenScreen[0].length; i++)
        {
            m_lmGreenScreen.append({"text":UIBridge.greenScreen[0][i], "icon":UIBridge.greenScreen[1][i],
                                       "value":UIBridge.greenScreen[3][i], "middle":UIBridge.greenScreen[4][i]})
        }
        m_lmGreenScreen.append({"text": "恢复默认", "icon": "", "value": "", "middle": ""})
    }
    //读取美妆参数
    function loadCustomMakeupTitle(){
        m_lmCustomMakeupTitle.clear()
        for(var i = 0; i< 10; i++)
        {
            m_lmCustomMakeupTitle.append({"text":UIBridge.getCustomMakeup(i)[0][0], "value":UIBridge.getCustomMakeup(i)[0][3]})
        }
    }
    //选中自定义美妆标题按钮 腮红,阴影等展示苹果肌,扇形
    function selectCustomMakeupTitel(index){
        m_sliderMakeup.visible = true
        //修改滑块值
        m_sliderMakeup.value = UIBridge.getCustomMakeup(index)[0][3]
        m_lmCustomMakeup.clear()
        for(var i = 1; i < UIBridge.getCustomMakeup(index).length; i++){
            if(UIBridge.getCustomMakeup(index)[i][2] !== ""){
                m_lmCustomMakeup.append({"text":UIBridge.getCustomMakeup(index)[i][0], "icon":UIBridge.getCustomMakeup(index)[i][2]})
            }
        }
        //修改选中项
        var selectMakeup = Number(UIBridge.getCustomMakeup(index)[0][4])
        var selectColor = Number(UIBridge.getCustomMakeup(index)[0][5])
        m_lCustomMakeup.contentX = 0
        m_lCustomMakeup.currentIndex = selectMakeup
        m_lCustomMakeup.visible = true
        //粉底列外,直接显示颜色
        if(m_lmCustomMakeup.count == 0){
            selectCustomMakeup(0)
            //粉底需要加载bundle
            UIBridge.setCustomMakeupIndex(7, 0)
            m_lCustomMakeupColor.visible = true
        }else{
            if(selectColor !== -1){
                selectCustomMakeup(selectMakeup)
                m_lCustomMakeupColor.visible = true
            }else{
                m_lCustomMakeupColor.visible = false
            }
        }
        m_lCustomMakeupColor.currentIndex = -1
        m_lCustomMakeupColor.currentIndex = selectColor
    }
    //选中自定义美妆选项下表苹果肌,扇形等展示颜色
    function selectCustomMakeup(index){
        m_lCustomMakeupColor.visible = true
        m_lmCustomMakeupColor.clear()
        //根据颜色字符串每个#截取分开
        for(var i = 4; i < UIBridge.getCustomMakeup(m_rCustomMakeupWindow.n_select)[index + 1].length; i++){
            var color0 = "", color1 = "", color2 = ""
            color0 = UIBridge.getCustomMakeup(m_rCustomMakeupWindow.n_select)[index + 1][i]
            if(color0.indexOf("#",1) > 0){
                var firstIndex = color0.indexOf("#",1)
                color1 = color0.substring(firstIndex,color0.length)
                color0 = color0.substring(0, firstIndex)
                if(color1.indexOf("#",1) > 0){
                    var secondIndex = color1.indexOf("#",1)
                    color2 = color1.substring(secondIndex,color1.length)
                    color1 = color1.substring(0,secondIndex)
                }
            }
            m_lmCustomMakeupColor.append({"color0":color0, "color1":color1, "color2":color2})
        }
    }
    //右侧美肤,美型,滤镜,美体 选中切换
    function updataTapContent(id){
        m_rBeautySkin.b_Selected = false
        m_rBeautyFace.b_Selected = false
        m_rFilter.b_Selected = false
        m_rBeautyBody.b_Selected = false
        m_lBeautySkin.visible = false
        m_lBeautyFace.visible = false
        m_iFilter.visible = false
        m_lBeautyBody.visible = false
        switch(id){
        case 0:
            m_rBeautySkin.b_Selected = true
            m_lBeautySkin.visible = true
            break
        case 1:
            m_rBeautyFace.b_Selected = true
            m_lBeautyFace.visible = true
            break
        case 2:
            m_rFilter.b_Selected = true
            m_iFilter.visible = true
            break
        case 3:
            m_rBeautyBody.b_Selected = true
            m_lBeautyBody.visible = true
            break
        default:
            break
        }
    }
    //完成取色
    function completeSelectColor(){
        m_bSelectColor.enabled = true
        m_rARGSShadow.visible = false
        m_rPropShadow.visible = false
        m_maSelectColor.enabled = false
        m_rpropOption.visible = true
        UIBridge.gsUnSelectColor();
    }
    //取色获得点击颜色
    function selectColorChanged(getcolor){
        m_rColor.color = getcolor
    }
    //一键卸妆
    function resetMakeup(){
        m_sliderMakeup.visible = false
        m_lCustomMakeup.currentIndex = -1
        m_lCustomMakeup.contentX = 0
        m_lCustomMakeup.visible = false
        m_lmCustomMakeup.clear()
        m_lCustomMakeupColor.visible = false
        UIBridge.resetCustomMakeup()
    }
    //总窗体
    Rectangle{
        anchors.fill: parent
        transform: Scale{
            origin.x: 0
            origin.y: 0
            xScale: m_window.xScale
            yScale: m_window.yScale
        }
        color: "#EAEBF2"

        //主窗体
        Rectangle{
            x: bFullScreen ? 0 : 10
            y: bFullScreen ? 0 : 160
            width: bFullScreen ? 1360 : 910
            height: bFullScreen ? 830 : 660
            radius: 8
            color: "#FFFFFF"
            //显示摄像头图像区域
            CameraDisplay{
                id:m_CameraDisplay
                x: bFullScreen ? 0 :10
                y: bFullScreen ? 0 : 10
                width: bFullScreen ? 1360 : 864
                height: bFullScreen ? 830 : 486
                onWidthChanged: {
                    UIBridge.cameraWidth = width
                }
                onHeightChanged: {
                    UIBridge.cameraHeight = height
                }
                //精品贴纸点击触发效果
                MouseArea{
                    id: m_maitemJingpin
                    anchors.fill: parent
                    enabled: i_selectCategory == 3
                    onClicked: {
                        UIBridge.itemJingpinClick();
                    }
                }
                //绿幕左键点击拖动,滚轮放大缩小
                MouseArea{
                    id: m_maCameraDisplay
                    property point clickPoint: "0,0"
                    property var b_Pressed: false
                    property var changeX: 0.0
                    property var changeY: 0.0
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    enabled: false
                    onPressed: {
                        b_Pressed = true
                        clickPoint = Qt.point(mouse.x, mouse.y)
                    }
                    onPositionChanged: {
                        var offset = Qt.point(mouse.x - clickPoint.x, mouse.y - clickPoint.y)
                        changeX = offset.x / m_maCameraDisplay.width
                        changeY = offset.y / m_maCameraDisplay.height
                        UIBridge.gsTranslocation(changeX, changeY);
                    }
                    onWheel:{
                        if(wheel.modifiers & Qt.ControlModifier){
                            var datl = wheel.angleDelta.y/120;
                            if(datl>0){
                                UIBridge.gsZoom(true);
                            }else{
                                UIBridge.gsZoom(false);
                            }
                        }
                    }
                    onReleased: {
                        b_Pressed = false
                        UIBridge.gsUpdatalocation(changeX, changeY);
                    }
                }
            }
            //点击绿幕选择颜色
            MouseArea{
                id: m_maSelectColor
                enabled: false
                anchors.fill: m_CameraDisplay
                onClicked: {
                    UIBridge.selectColor(mouseX, mouseY)
                    completeSelectColor()
                }
            }
            //avatar时显示右下角摄像头图像
            Image{
                id: m_showImage
                anchors.bottom: m_CameraDisplay.bottom
                anchors.right: m_CameraDisplay.right
                anchors.rightMargin: 15
                anchors.bottomMargin: 15
                width: 216
                height: 121
                visible: m_window.i_avatarType != -1
            }

            //渲染区域全屏按钮,默认不展示
            Image {
                visible: false
                anchors.bottom: m_CameraDisplay.bottom
                anchors.right: m_CameraDisplay.right
                anchors.rightMargin: 20
                anchors.bottomMargin: 20
                width: 30
                height: 30
                source: !bFullScreen ? "qrc:/res/btn_video_zoom_nor.png" : "qrc:/res/btn_video_zoom_out_nor.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        bFullScreen = !bFullScreen
                    }
                }
            }
            Item{
                visible: !bFullScreen
                //avator切换全身驱动,半身驱动
                Rectangle{
                    x: 20
                    y: 380
                    width: 100
                    height: 28
                    radius: 90
                    visible: m_window.i_avatarType != -1
                    color: m_window.i_avatarType == 1 ? "#7787E9" : "#FFFFFF"
                    Rectangle{
                        x: m_window.i_avatarType == 1 ? 74 : 2
                        y: 2
                        width: 24
                        height: 24
                        radius: 90
                        color: m_window.i_avatarType == 1 ? "#FFFFFF" : "#7787E9"
                    }
                    TextBlack{
                        x: m_window.i_avatarType == 1 ? 0 : 25
                        y: 0
                        width: 74
                        height: 28
                        text: m_window.i_avatarType == 1 ? "全身驱动" : "半身驱动"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(m_window.i_avatarType == 0){
                                UIBridge.bodyTrackType = 1
                                m_window.i_avatarType = 1
                                m_window.i_avatarTypeShow = 1
                            }else{
                                UIBridge.bodyTrackType = 0
                                m_window.i_avatarType = 0
                                m_window.i_avatarTypeShow = 0
                            }
                        }
                    }
                }

                //自定义背景分割选择
                Rectangle{
                    id: m_rBGSegment
                    x: 10
                    y: 10
                    width: 864
                    height: 486
                    color: "#C0C0C0"
                    visible: false
                    FileDialog {
                        id: m_fileDialogBGS
                        title: qsTr("选择视频/图片")
                        nameFilters: ["选择视频 (*.mp4 *.avi *.wmv)", "选择图片 (*.jpg *.png *.gif *.bmp *.ico)", "*.*"]
                        onAccepted: {
                            UIBridge.bgsSelectVideo(m_fileDialogBGS.fileUrl)
                            m_rBGSegment.visible = false
                            m_rPropShadow.visible = false
                            m_rARGSShadow.visible = false
                        }
                        onRejected: {
                            m_rBGSegment.visible = false
                            m_rPropShadow.visible = false
                            m_rARGSShadow.visible = false
                        }
                    }
                    Rectangle{
                        anchors.centerIn: parent
                        width: 400
                        height: 300
                        radius: 8
                        Image {
                            x: 176
                            y: 0
                            width: 52
                            height: 52
                            source: "qrc:/res/icon_prompt.png"
                        }
                        TextBlack{
                            x: 110
                            y: 60
                            text:"请选择自定义背景视频/图片"
                        }
                    }
                }

                //绿幕选择窗口
                Rectangle{
                    id: m_rGreenScreen
                    x: 10
                    y: 10
                    width: 864
                    height: 486
                    color: "#C0C0C0"
                    visible: false
                    Rectangle{
                        anchors.centerIn: parent
                        width: 400
                        height: 300
                        radius: 8
                        Image {
                            x: 176
                            y: 0
                            width: 52
                            height: 52
                            source: "qrc:/res/icon_prompt.png"
                        }
                        TextBlack{
                            x: 60
                            y: 60
                            text:"请使用纯色背景，推荐绿色幕布效果效果最佳"
                        }
                        RectangleButtonGreenScreen{
                            x: 140
                            y: 150
                            t_Text: "选择摄像头"
                            function onButtonClick(){
                                m_greenScreenSelectCamera.visible = true
                                m_cCamera.visible = true
                                m_cCamera.x = 270
                                m_cCamera.y = 195
                                UIBridge.gsCameraImage = true
                            }
                        }
                        FileDialog {
                            id: m_fileDialog
                            title: qsTr("选择视频/图片")
                            nameFilters: ["选择视频 (*.mp4 *.avi *.wmv)", "选择图片 (*.jpg *.png *.gif *.bmp *.ico)", "*.*"]
                            onAccepted: {
                                UIBridge.gsSelectVideo(m_fileDialog.fileUrl)
                                b_setGreenScreen = true
                                m_rGreenScreen.visible = false
                                m_rPropShadow.visible = false
                            }
                            onRejected: {
                                if(b_setGreenScreen == true){
                                    m_rGreenScreen.visible = false
                                    m_rPropShadow.visible = false
                                }
                            }
                        }
                        RectangleButtonGreenScreen{
                            x: 140
                            y: 200
                            t_Text: "选择视频/图片"
                            function onButtonClick(){
                                m_fileDialog.open()
                            }
                        }
                        RectangleButtonGreenScreen{
                            x: 140
                            y: 250
                            t_Text: "取消"
                            visible: b_setGreenScreen
                            function onButtonClick(){
                                m_rGreenScreen.visible = false
                            }
                        }
                    }
                }

                Rectangle{
                    id: m_bPerformance
                    x: 20
                    y: 20
                    width: 120
                    height: 40
                    radius: 8
                    color: "#525870"
                    opacity: 0.78
                    TextBlack{
                        anchors.fill: parent
                        text: "显示性能参数"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            m_bPerformance.visible = false
                            m_rParameter.visible = true
                        }
                    }
                }

                Rectangle{
                    id: m_rSelectGreenScreen
                    x: 730
                    y: 20
                    width: 120
                    height: 40
                    radius: 8
                    color: "#525870"
                    opacity: 0.78
                    visible: false
                    TextBlack{
                        anchors.fill: parent
                        text: "选择绿幕输入"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            m_rGreenScreen.visible = true
                            UIBridge.stopMediaPlayer();
                        }
                    }
                }
                //显示性能参数值
                Rectangle{
                    id: m_rParameter
                    visible: false
                    x: 20
                    y: 20
                    width: 160
                    height: 100
                    color: "#525870"
                    radius: 8
                    opacity: 0.78
                    TextBlack{
                        x: 8
                        y: 8
                        color: "#FFFFFF"
                        text: "FPS:"
                    }
                    TextBlack{
                        id:m_tFPS
                        x: 40
                        y: 8
                        color: "#FFFFFF"
                        text: UIBridge.fps
                    }
                    TextBlack{
                        x: 8
                        y: 30
                        color: "#FFFFFF"
                        text: "Resolution:"
                    }
                    TextBlack{
                        id:m_tResolution
                        x: 85
                        y: 30
                        color: "#FFFFFF"
                        text: UIBridge.frameSize
                    }
                    TextBlack{
                        x: 8
                        y: 52
                        color: "#FFFFFF"
                        text: "RenderTime:"
                    }
                    TextBlack{
                        id:m_tRenderTime
                        x: 95
                        y: 52
                        color: "#FFFFFF"
                        text: UIBridge.renderTime + "ms"
                    }
                    Rectangle{
                        id: m_bParameterClose
                        x: 105
                        y: 80
                        width: 40
                        height: 20
                        color: "#61677A"
                        opacity: 0.78
                        TextBlack{
                            anchors.fill: parent
                            text: "关闭"
                        }
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                m_bParameterClose.color = "#98A5F5"
                            }
                            onExited: {
                                m_bParameterClose.color = "#61677A"
                            }
                            onClicked: {
                                m_bPerformance.visible = true
                                m_rParameter.visible = false
                            }
                        }
                    }
                }//---显示性能参数值
                //下方道具列表
                ListView {
                    id: m_lProp
                    x: 0
                    y: 500
                    width: 910
                    height: 140
                    orientation: ListView.Horizontal
                    clip: true
                    interactive:false
                    model: ListModel
                    {
                        id: m_lmProp
                    }
                    delegate: ListViewProp{
                        icon_Name: icon
                        t_Text: text
                        b_Selected: selected
                        //重新选择道具列表选中
                        onSelectedChange: {
                            showBoutiqueSticker(false)
                            var rowCount = m_lmProp.count;
                            //遍历道具列表
                            for( var i = 0;i < rowCount;i++ ) {
                                var model = m_lmProp.get(i);
                                if(model.icon === selectName){
                                    model.selected = !model.selected;
                                    m_rpropOption.visible = false
                                    if(i === i_category_jingpin && b_ARFunction){
                                        //更新精品贴纸
                                        i_selectCategory = i_category_jingpin
                                        UIBridge.selectCategory = i_category_jingpin
                                        showBoutiqueSticker(selected)
                                    }else if(b_ARFunction){
                                        m_rpropOption.visible = model.selected
                                        updataPropOption(i)
                                        UIBridge.selectCategory = i
                                        i_selectCategory = i
                                        showGreenScreenList(false)
                                    }else{
                                        m_rpropOption.visible = model.selected
                                        if(model.selected){
                                            updataPropOption(i)
                                            UIBridge.selectCategory = i + i_category_greenScreen
                                            i_selectCategory = i
                                        }
                                        //如果在绿幕状态显示绿幕参数设置
                                        showGreenScreenList(model.selected)
                                    }
                                }else{
                                    model.selected = false;

                                }
                            }
                        }
                    }
                }//---下方道具列表
                //用于遮挡弹出道具选择列表
                Rectangle{
                    id: m_rPropShadow
                    x: 0
                    y: 500
                    width: 910
                    height: 140
                    color: "#C8FFFFFF"
                    visible: false
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                //道具列表自定义滑块
                ScrollBar  {
                    id: hbar
                    hoverEnabled: true
                    active:  hovered || pressed
                    orientation: Qt.Horizontal
                    x:0
                    y:645
                    width: 910
                    height: 13
                    size: m_lProp.width / m_lProp.contentWidth
                    policy: ScrollBar.AsNeeded
                    //定义样式
                    contentItem: Rectangle {
                        radius: 8
                        color: hbar.pressed ? "#828282" : "#707EED"
                    }
                    onPositionChanged: {
                        m_lProp.contentX = position * (m_lProp.contentWidth)
                    }
                }
                Connections{
                    target: m_lProp
                    onContentXChanged: {
                        hbar.position = m_lProp.contentX / m_lProp.contentWidth
                    }
                }
                //弹出道具选择列表
                Rectangle{
                    id: m_rpropOption
                    x: 10
                    y: 420
                    width: 864
                    height: 80
                    color: "#77FFFFFF"
                    radius: 8
                    visible: false
                    ListView {
                        id: m_propOptionListView
                        anchors.fill: parent
                        orientation: ListView.Horizontal
                        clip: true
                        model: ListModel
                        {
                            id: m_lmPropOption
                        }
                        delegate: ListViewPropOption{
                            icon_Name: icon
                            b_Selected: selected
                            b_path_type: pathType
                            //重新选择道具列表选中
                            onSelectedChange: {
                                var rowCount = m_lmPropOption.count;
                                for( var i = 0; i < rowCount; i++ ) {
                                    var model = m_lmPropOption.get(i);
                                    if(model.icon === selectName){
                                        model.selected = !model.selected;
                                        if(model.selected){
                                            if(b_ARFunction){
                                                i_arSelectCategoryPoint = Qt.point(i_selectCategory, i)
                                            }else{
                                                if(i_selectCategory == 0){
                                                    i_gsSelectVideo = i
                                                }else if(i_selectCategory == i_category_safeArea - i_category_greenScreen){
                                                    if(i > 0){
                                                        i_gsSelectSaveArea = i
                                                    }
                                                }
                                            }
                                            if(m_rCustomMakeupWindow.visible){
                                                resetMakeup()
                                            }
                                            if(UIBridge.selectCategory === i_category_backgroundSegmentation && i === 0){
                                                UIBridge.useProps(i)
                                                //显示自定义背景分割
                                                m_rPropShadow.visible = true
                                                m_rBGSegment.visible = true
                                                m_rARGSShadow.visible = true
                                                m_fileDialogBGS.open()
                                            }else if(UIBridge.selectCategory === i_category_avatar && i === 0){
                                                showAvator(true)
                                            }else if(UIBridge.selectCategory === i_category_safeArea && i === 0){
                                                //打开自定义绿幕安全区域选择
                                                m_rPropShadow.visible = true
                                                m_rARGSShadow.visible = true
                                                m_fileDialogGSSafe.open()
                                            }else{
                                                showAvator(false)
                                                UIBridge.useProps(i)
                                            }
                                        }else{
                                            if(UIBridge.selectCategory === 0){
                                                showAvator(false)
                                            }else{
                                                UIBridge.nonuseProps()
                                            }
                                            i_arSelectCategoryPoint = Qt.point(-1, -1)
                                        }
                                    }else{
                                        model.selected = false;
                                    }
                                }
                            }
                        }
                    }
                }//---弹出道具选择列表
                //下方道具滑块
                ScrollBar  {
                    id: hbarpropOption
                    hoverEnabled: true
                    active:  hovered || pressed
                    orientation: Qt.Horizontal
                    x: 10
                    y: 497
                    width: 864
                    height: 13
                    size: m_propOptionListView.width / m_propOptionListView.contentWidth
                    visible: m_propOptionListView.count > 11 ? m_propOptionListView.visible : false
                    //定义样式
                    contentItem: Rectangle {
                        radius: 8
                        color: hbarpropOption.pressed ? "#828282" : "#707EED"
                    }
                    onPositionChanged: {
                        m_propOptionListView.contentX = position * (m_propOptionListView.contentWidth)
                    }
                }
                Connections{
                    target: m_propOptionListView
                    onContentXChanged: {
                        hbarpropOption.position = m_propOptionListView.contentX / m_propOptionListView.contentWidth
                    }
                }//---下方道具滑块
                //精品贴纸
                Rectangle{
                    id: m_rBoutiqueSticker
                    x: 10
                    y: 370
                    width: 864
                    height: 130
                    color: "#77FFFFFF"
                    radius: 8
                    visible: false
                    //中级道具|高级道具|效果验证|游戏道具
                    ListView {
                        id: m_lStickerTips
                        x: 10
                        y: 0
                        width: 854
                        height: 30
                        orientation: ListView.Horizontal
                        clip: true
                        model: ListModel
                        {
                            id: m_lmStickerTips
                        }
                        delegate:Rectangle{
                            property var b_hover: false
                            x: 0
                            y: 0
                            width: 150
                            height: 30
                            TextBlack{
                                anchors.fill: parent
                                text: tipText
                                color: m_lStickerTips.currentIndex != index  ? "#FFFFFF" : "#000000"
                            }
                            color: b_hover ? "#5DA0EE" : (m_lStickerTips.currentIndex == index ? "#FFFFFF" : "#32305C")
                            MouseArea{
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    b_hover = true
                                }
                                onExited: {
                                    b_hover = false
                                }
                                onClicked: {
                                    m_lStickerTips.currentIndex = index
                                    updataBoutiqueSticker(index)
                                }
                            }
                        }
                    }//---中级道具|高级道具
                    //精品贴纸道具选项
                    ListView {
                        id: m_lStickerBundles
                        x: 0
                        y: 30
                        width: 864
                        height: 80
                        orientation: ListView.Horizontal
                        clip: true
                        cacheBuffer: 1200
                        model: ListModel
                        {
                            id: m_lmStickerBundles
                        }
                        delegate:Item{
                            x: 0
                            y: 0
                            width: 80
                            height: 80
                            Rectangle{
                                id: m_StickerBackground
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
                                visible: selected
                                color: "#00ffffff"
                                border.color: "#7787E9"
                                border.width: 3
                            }
                            Item{
                                x: 10
                                y: 10
                                width: 60
                                height: 60
                                Image {
                                    id: m_StickerImage
                                    anchors.centerIn: parent
                                    source: "file:./" + icon
                                    smooth: true
                                    visible: false
                                }
                                Rectangle {
                                    id: mask
                                    width: parent.width
                                    height: parent.height
                                    radius: 90
                                    visible: false
                                }
                                OpacityMask {
                                    anchors.fill: parent
                                    source: m_StickerImage
                                    maskSource: mask
                                }
                                MouseArea{
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: {
                                        m_StickerBackground.visible = true
                                    }
                                    onExited: {
                                        m_StickerBackground.visible = false
                                    }
                                    onClicked: {
                                        var rowCount = m_lmStickerBundles.count;
                                        var selectName = icon
                                        for( var i = 0;i < rowCount; i++ ) {
                                            var model = m_lmStickerBundles.get(i);
                                            if(model.icon === selectName){
                                                model.selected = !model.selected;
                                                if(model.selected){
                                                    i_arSelectCategoryPoint = Qt.point(i_selectCategory, i)
                                                    model.ondownload = true
                                                    i_selectSticker = m_lStickerTips.currentIndex
                                                    //点击下载
                                                    UIBridge.downloadSticker(index)
                                                }else{
                                                    i_arSelectCategoryPoint = Qt.point(-1, -1)
                                                    UIBridge.nonuseProps()
                                                }
                                            }else{
                                                model.selected = false;
                                            }
                                        }
                                    }
                                }
                            }
                            Image{
                                x: 55
                                y: 55
                                width: 12
                                height: 12
                                visible: !download
                                source: ondownload ? "qrc:/res/btnLoad.png" : "qrc:/res/boutique_stickers_icon_download.png"
                            }
                        }
                    }//---精品贴纸道具选项
                }//---精品贴纸
                //精品贴纸自定义滑块
                ScrollBar  {
                    id: hbarSticker
                    hoverEnabled: true
                    active: hovered || pressed
                    orientation: Qt.Horizontal
                    x:20
                    y:480
                    width: 844
                    height: 13
                    visible: m_rBoutiqueSticker.visible & m_lStickerBundles.count > 11 ? true : false
                    size: m_lStickerBundles.width / m_lStickerBundles.contentWidth
                    //定义样式
                    contentItem: Rectangle {
                        radius: 8
                        color: hbarSticker.pressed ? "#828282" : "#707EED"
                    }
                    onPositionChanged: {
                        m_lStickerBundles.contentX = position * (m_lStickerBundles.contentWidth)
                    }
                }
                Connections{
                    target: m_lStickerBundles
                    onContentXChanged: {
                        hbarSticker.position = m_lStickerBundles.contentX / m_lStickerBundles.contentWidth
                    }
                }
                //AR功能/绿幕抠像
                Rectangle{
                    x: 340
                    y: 605
                    width: 270
                    height: 24
                    radius: 8
                    color: "#E1E4EE"
                    Rectangle{
                        id: m_rARbackgroud
                        x: 2
                        y: 2
                        radius: 8
                        width: 131
                        height: 20
                        color: "#3D85E0"
                    }
                    TextBlack{
                        x: 45
                        y: 2
                        text: "AR功能"
                    }
                    TextBlack{
                        x: 175
                        y: 2
                        text: "绿幕抠像"
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(UIBridge.arFunction && mouseX >= 135){
                                //绿幕选择视频,避免切换显示绿幕图像一帧,截断切换
                                if(!UIBridge.gsSelectCamera){
                                    UIBridge.setRenderNewFrame()
                                    UIBridge.arFunction = false
                                }else{
                                    UIBridge.arFunction = false
                                    switchARFunction(false)
                                }
                            }else if(!UIBridge.arFunction && mouseX < 135){
                                if(!UIBridge.gsSelectCamera){
                                    UIBridge.setRenderNewFrame()
                                    UIBridge.arFunction = true
                                }else{
                                    UIBridge.arFunction = true
                                    switchARFunction(true)
                                }
                            }
                        }
                    }
                }//---AR功能/绿幕抠像
                //用于遮挡AR功能/绿幕抠像
                Rectangle{
                    id: m_rARGSShadow
                    x: 340
                    y: 605
                    width: 270
                    height: 24
                    color: "#C8FFFFFF"
                    visible: false
                    MouseArea{
                        anchors.fill: parent
                    }
                }
            }
        }//---主窗体
        Item{
            visible: !bFullScreen
            //标题
            Rectangle{
                x: 0
                y: 0
                width: 1360
                height: 60
                radius: 8
                Image {
                    x: 10
                    y: 10
                    width: 1340
                    height: 40
                    source: "qrc:/res/title.png"
                }
            }
            //显示提示文字
            Rectangle{
                id: m_rTipText
                x: 432 - m_tipText.contentWidth / 2
                y: 490
                width: m_tipText.contentWidth + 20
                radius: 8
                height: 30
                color: "#C8FFFFFF"
                visible: false
                TextBlack{
                    id:m_tipText
                    text: UIBridge.tip
                    anchors.fill: parent
                }
            }
            //侧边功能窗体 美肤 美型 滤镜 美体
            Rectangle{
                id: m_rSideWindow
                x: 935
                y: 70
                width: 415
                height: 750
                radius: 8
                Row {
                    x: 8
                    y: 8
                    width: 400
                    height: 30
                    spacing: 2
                    RectangleTabContent{
                        id: m_rBeautySkin
                        b_Selected: true
                        i_Tab_ID: 0
                        t_Text: "美肤"
                        b_GreenScreen: !b_ARFunction
                        onSelectedChange: {
                            updataTapContent(i_Tab_ID)
                        }
                    }
                    RectangleTabContent{
                        id: m_rBeautyFace
                        i_Tab_ID: 1
                        t_Text: "美型"
                        b_GreenScreen: !b_ARFunction
                        onSelectedChange: {
                            updataTapContent(i_Tab_ID)
                        }
                    }
                    RectangleTabContent{
                        id: m_rFilter
                        i_Tab_ID: 2
                        t_Text: "滤镜"
                        b_GreenScreen: !b_ARFunction
                        onSelectedChange: {
                            updataTapContent(i_Tab_ID)
                        }
                    }
                    RectangleTabContent{
                        id: m_rBeautyBody
                        i_Tab_ID: 3
                        t_Text: "美体"
                        visible: b_ARFunction
                        onSelectedChange: {
                            updataTapContent(i_Tab_ID)
                        }
                    }
                }
                //美肤窗体
                ListView {
                    id: m_lBeautySkin
                    x: 0
                    y: 50
                    width: 415
                    height: 680
                    clip: true
                    model: ListModel
                    {
                        id: m_lmBeautySkin
                    }
                    delegate: ListViewSide{
                        icon_Name: icon
                        t_Text: text
                        i_Value: value
                        onIValueChanged: {
                            UIBridge.updataItemParam(0, index, value)
                        }
                        //恢复默认
                        onClickRestoreDefault: {
                            UIBridge.resetItemParam(0)
                            var list = [1, 3, 70, 30, 30, 20, 0, 0, 0, 0]
                            for(var i = 0; i < m_lmBeautySkin.count - 1; i++){
                                m_lBeautySkin.itemAtIndex(i).resetValue(list[i])
                            }
                        }
                    }
                    ScrollBar  {
                        id: m_sbBeautySkin
                        hoverEnabled: true
                        active: hovered || pressed
                        orientation: Qt.Vertical
                        x:400
                        y:0
                        width: 13
                        height: 680
                        size: m_lBeautySkin.height / m_lBeautySkin.contentHeight
                        policy: ScrollBar.AlwaysOn
                        //定义样式
                        contentItem: Rectangle {
                            radius: 8
                            color: m_sbBeautySkin.pressed ? "#707EED" : "#E0E3EE"
                        }
                        onPositionChanged: {
                            m_lBeautySkin.contentY = position * (m_lBeautySkin.contentHeight)
                        }
                    }
                    Connections{
                        target: m_lBeautySkin
                        onContentYChanged: {
                            m_sbBeautySkin.position = m_lBeautySkin.contentY / m_lBeautySkin.contentHeight
                        }
                    }
                }//---美肤窗体
                //美型窗体
                ListView {
                    id: m_lBeautyFace
                    x: 0
                    y: 50
                    width: 415
                    height: 680
                    clip: true
                    visible: false
                    //此属性确定委托是否保留在视图的可见区域之外,解决下滑后恢复默认失效问题
                    cacheBuffer: 1000
                    model: ListModel
                    {
                        id: m_lmBeautyFace
                    }
                    delegate: ListViewSide{
                        icon_Name: icon
                        t_Text: text
                        i_Value: value
                        b_Middle: middle
                        onIValueChanged: {
                            UIBridge.updataItemParam(1, index, value)
                        }
                        onClickRestoreDefault: {
                            UIBridge.resetItemParam(1)
                            var list = [0, 40, 0, -20, -20, 50, -10, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
                            for(var i = 0; i < m_lmBeautyFace.count - 1; i++){
                                m_lBeautyFace.itemAtIndex(i).resetValue(list[i])
                            }
                        }
                    }
                    ScrollBar  {
                        id: m_sbBeautyFace
                        hoverEnabled: true
                        active: hovered || pressed
                        orientation: Qt.Vertical
                        x:400
                        y:0
                        width: 13
                        height: 680
                        size: m_lBeautyFace.height / m_lBeautyFace.contentHeight
                        policy: ScrollBar.AlwaysOn
                        //定义样式
                        contentItem: Rectangle {
                            radius: 8
                            color: m_sbBeautyFace.pressed ? "#707EED" : "#E0E3EE"
                        }
                        onPositionChanged: {
                            m_lBeautyFace.contentY = position * (m_lBeautyFace.contentHeight)
                        }
                    }
                    Connections{
                        target: m_lBeautyFace
                        onContentYChanged: {
                            m_sbBeautyFace.position = m_lBeautyFace.contentY / m_lBeautyFace.contentHeight
                        }
                    }
                }//---美型窗体
                //滤镜窗体
                Item{
                    id: m_iFilter
                    x: 15
                    y: 50
                    width: 400
                    height: 680
                    visible: false
                    GridView{
                        id: m_gFilter
                        anchors.fill: parent
                        clip: true;
                        model: ListModel{
                            id: m_lmFilter
                        }
                        cellWidth: 133;
                        cellHeight: 133;
                        delegate:Rectangle{
                            id: m_crFilter
                            property var b_inMouse: false
                            property var b_Selected: index == 0 ? true : false
                            width: 110
                            height: 110
                            border.color: b_Selected ? "#7088ED" :(b_inMouse ? "#98A5F5" : "#FFFFFF")
                            border.width: 4
                            function updataSelect(select){
                                if(select === index){
                                    b_Selected = true
                                }else{
                                    b_Selected = false
                                }
                            }
                            Image{
                                x: 4
                                y: 4
                                width: 102
                                height: 102
                                source: "qrc:/res/list_image_" + icon + ".png"
                            }
                            MouseArea{
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    m_crFilter.b_inMouse = true
                                }
                                onExited: {
                                    m_crFilter.b_inMouse = false
                                }
                                onClicked: {
                                    m_gFilter.currentIndex = index
                                    UIBridge.resetFilterIndex(index)
                                    //更新底部滑块
                                    if(index == 0){
                                        m_slider.visible = false
                                    }else{
                                        m_slider.visible = true
                                    }
                                    m_slider.value = UIBridge.filter[1][index]
                                    //更新选中颜色
                                    for(var i = 0; i < m_gFilter.count; i++){
                                        m_gFilter.itemAtIndex(i).updataSelect(index)
                                    }
                                }
                            }
                        }
                    }
                    //底部滑块
                    CustomSlider
                    {
                        id: m_slider
                        x: 70
                        y: 510
                        width: 255
                        height: 20
                        visible: false
                        value: 0
                        onValueChanged:
                        {
                            if(m_gFilter.currentIndex != -1){
                                UIBridge.setFilter(value)
                                m_tslider.text = value
                            }
                        }
                    }
                    TextBlack{
                        id: m_tslider
                        visible: m_slider.visible
                        anchors.centerIn: m_slider
                        text: "0"
                    }
                    //轻美妆提示
                    Rectangle{
                        anchors.fill: parent
                        color: "#82FFFFFF"
                        visible: i_arSelectCategoryPoint.x == i_category_lightMakeup && b_ARFunction && !b_arBody
                        MouseArea{
                            anchors.fill: parent
                        }
                        Rectangle{
                            x: 34
                            y: 346
                            width: 348
                            height: 88
                            radius: 8
                            border.width: 1
                            border.color: "#E0E3EE"
                            Image {
                                x:32
                                y:34
                                width: 20
                                height: 20
                                source: "qrc:/res/icon_tips.png"
                            }
                            TextBlack{
                                x:58
                                y:32
                                text: "轻美妆与滤镜无法共用，如需编辑请先取消"
                            }
                        }
                    }
                }//---滤镜窗体

                //美体窗体
                ListView {
                    id: m_lBeautyBody
                    x: 0
                    y: 50
                    width: 415
                    height: 680
                    clip: true
                    visible: false
                    model: ListModel
                    {
                        id: m_lmBeautyBody
                    }
                    delegate: ListViewSide{
                        icon_Name: icon
                        t_Text: text
                        i_Value: value
                        b_Middle: middle
                        onIValueChanged: {
                            UIBridge.updataItemParam(2, index, value)
                        }
                        onClickRestoreDefault: {
                            var list = [0, 0, 0, 0, 0, 0, 0]
                            for(var i = 0; i < m_lmBeautyBody.count - 1; i++){
                                m_lBeautyBody.itemAtIndex(i).resetValue(list[i])
                            }
                            UIBridge.resetItemParam(2)
                            m_rPropShadow.visible = false
                        }
                    }
                }//---美体窗体
            }
            //绿幕窗体
            Rectangle{
                id: m_rSideGreenScreenWindow
                x: 935
                y: 70
                width: 415
                height: 750
                radius: 8
                visible: false
                FileDialog {
                    id: m_fileDialogGSSafe
                    title: qsTr("选择图片")
                    nameFilters: ["选择图片 (*.jpg *.png *.gif *.bmp *.ico)", "*.*"]
                    onAccepted: {
                        m_rPropShadow.visible = false
                        m_rARGSShadow.visible = false
                        UIBridge.gsSafeAreaSelect(m_fileDialogGSSafe.fileUrl)
                    }
                    onRejected: {
                        m_rPropShadow.visible = false
                        m_rARGSShadow.visible = false
                    }
                }
                Item{
                    x: 0
                    y: 20
                    width: 415
                    height: 85
                    Rectangle {
                        id: m_rColor
                        x: 36
                        y: 5
                        width: 52
                        height: 52
                        radius: 90
                        color: "#00FF00"
                        border.color: "#E1E1E1"
                        border.width: 3
                        onColorChanged: {
                            UIBridge.gsColorChange(color)
                        }
                    }
                    RectangleButtonGreenScreen{
                        id: m_bGetColor
                        x: 120
                        y: 25
                        width: 80
                        height: 30
                        t_Text: "取色"
                        function onButtonClick(){
                            //第一次点击取色
                            if(m_bSelectColor.enabled){
                                m_bSelectColor.enabled = false
                                //遮挡下方ar,绿幕切换
                                m_rARGSShadow.visible = true
                                m_rPropShadow.visible = true
                                //取色区域可用
                                m_maSelectColor.enabled = true
                                m_rpropOption.visible = false
                                //设置绿幕图像位置最大化
                                UIBridge.gsSelectColor();
                            }else{
                                completeSelectColor()
                            }
                        }
                    }
                    RectangleButtonGreenScreen{
                        id: m_bSelectColor
                        x: 240
                        y: 25
                        width: 80
                        height: 30
                        t_Text: "选择颜色"
                        function onButtonClick(){
                            if(m_bGetColor.enabled){
                                m_bGetColor.enabled = false
                                m_rSelectColor.visible = true
                            }else{
                                m_bGetColor.enabled = true
                                m_rSelectColor.visible = false
                            }
                        }
                    }
                }
                ListView {
                    id: m_lGreenScreen
                    x: 0
                    y: 85
                    width: 415
                    height: 500
                    clip: true
                    model: ListModel
                    {
                        id: m_lmGreenScreen
                    }
                    delegate: ListViewSide{
                        icon_Name: icon
                        t_Text: text
                        i_Value: value
                        b_Middle: middle
                        onIValueChanged: {
                            if(index >= 0){
                                UIBridge.updataItemParam(3, index, value)
                            }
                        }
                        //绿幕点击恢复默认
                        onClickRestoreDefault: {
                            UIBridge.resetItemParam(3)
                            m_rColor.color = "#00FF00"
                            var list = [50, 30, 66]
                            for(var i = 0; i < m_lmGreenScreen.count - 1; i++){
                                m_lGreenScreen.itemAtIndex(i).resetValue(list[i])
                            }
                            //如果是绿幕区域状态
                            if(i_gsSelectSaveArea >= 0){
                                i_gsSelectSaveArea = -1
                                if(UIBridge.selectCategory === i_category_safeArea){
                                    updataPropOption(1)
                                }else{
                                    UIBridge.selectCategory = i_category_safeArea
                                }
                                UIBridge.nonuseProps()
                            }
                        }
                    }
                }
                //选择颜色窗口
                Rectangle{
                    id: m_rSelectColor
                    property var n_selectColor: 0
                    border.color: "#969DB5"
                    radius: 8
                    x: 70
                    y: 100
                    width: 200
                    height: 120
                    visible: false
                    TextBlack{
                        x: 10
                        y: 10
                        text: "可选颜色"
                    }
                    Rectangle{
                        id: m_rGreen
                        x: 10
                        y: 40
                        width: 30
                        height: 30
                        radius: 90
                        color: "#00FF00"
                        border.color: m_rSelectColor.n_selectColor == 1 ? "#8291EB" : "#FFFFFF"
                        border.width: 3
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                m_rColor.color = "#00FF00"
                                m_rSelectColor.n_selectColor = 1
                            }
                        }
                    }
                    Rectangle{
                        id: m_rBlue
                        x: 50
                        y: 40
                        width: 30
                        height: 30
                        radius: 90
                        color: "#0000FF"
                        border.color: m_rSelectColor.n_selectColor == 2 ? "#8291EB" : "#FFFFFF"
                        border.width: 3
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                m_rColor.color = "#0000FF"
                                m_rSelectColor.n_selectColor = 2
                            }
                        }
                    }
                    Rectangle{
                        id: m_rWhite
                        x: 90
                        y: 40
                        width: 30
                        height: 30
                        radius: 90
                        color: "#FFFFFF"
                        border.color: m_rSelectColor.n_selectColor == 3 ? "#8291EB" : "#E1E1E1"
                        border.width: 3
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                m_rColor.color = "#FFFFFF"
                                m_rSelectColor.n_selectColor = 3
                            }
                        }
                    }
                    RectangleButtonGreenScreen{
                        x: 60
                        y: 80
                        width: 80
                        height: 30
                        t_Text: "关闭"
                        function onButtonClick(){
                            m_bGetColor.enabled = true
                            m_rSelectColor.visible = false
                            m_rSelectColor.n_selectColor = 0
                        }
                    }
                }
            }//---绿幕窗体
            //绿幕选择摄像头
            Rectangle{
                id: m_greenScreenSelectCamera
                x: 180
                y: 240
                width: 510
                height: 390
                radius: 8
                visible:false
                TextBlack{
                    x: 5
                    y: 25
                    text:"选择摄像头:"
                }
                //绿幕摄像头图像
                Image{
                    id: m_showImageCamera
                    x: 12
                    y: 55
                    width: 480
                    height: 270
                }
                //---绿幕摄像头图像
                RectangleButtonGreenScreen{
                    x: 390
                    y: 347
                    width: 40
                    height: 22
                    border.width: 2
                    t_Text: "确定"
                    function onButtonClick(){
                        m_cCamera.x = 90
                        m_cCamera.y = 25
                        UIBridge.gsCameraImage = false
                        UIBridge.gsCameraConfirm()
                        b_setGreenScreen = true
                        m_greenScreenSelectCamera.visible = false
                        m_rGreenScreen.visible = false
                        m_rPropShadow.visible = false
                    }
                }
                RectangleButtonGreenScreen{
                    x: 440
                    y: 347
                    width: 40
                    height: 22
                    border.width: 2
                    t_Text: "关闭"
                    function onButtonClick(){
                        m_cCamera.x = 90
                        m_cCamera.y = 25
                        m_greenScreenSelectCamera.visible = false
                    }
                }
            }
            //---绿幕选择摄像头
            //抬头
            Rectangle{
                x: 10
                y: 70
                width: 910
                height: 80
                radius: 8
                TextBlack{
                    id: m_textSelectCamera
                    x: 5
                    y: 25
                    text:"选择摄像头:"
                    //双击打开网络摄像头
                    MouseArea{
                        property var webcam:false
                        anchors.fill: parent
                        onClicked: {
                            //屏蔽usb摄像头
                            if(!webcam){
                                webcam = true
                                m_textSelectCamera.text = "选择网络摄像头:"
                                m_webCamera.visible = true
                                m_usbCamera.visible = false
                                UIBridge.changeCameraType(true)
                            }else{
                                webcam = false
                                m_textSelectCamera.text = "选择摄像头:"
                                m_webCamera.visible = false
                                m_usbCamera.visible = true
                                UIBridge.changeCameraType(false)
                            }
                        }
                    }
                }
                Item {
                    id: m_webCamera
                    visible: false
                    Rectangle{
                        x: 110
                        y: 25
                        width: 490
                        height: 25
                        border.color: "#000000"
                        TextInput{
                            id:m_tiCameraURL
                            x: 5
                            y: 0
                            width: 485
                            height: 25
                            color: "#000000"
                            font {family: "微软雅黑"; pixelSize: 14;}
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                        }
                    }
                    FileDialog {
                        id: m_fileDialogImport
                        title: qsTr("选择视频/图片")
                        nameFilters: ["选择视频 (*.mp4 *.avi *.wmv)", "选择图片 (*.jpg *.png *.gif *.bmp *.ico)", "*.*"]
                        onAccepted: {
                            m_tiCameraURL.text = m_fileDialogImport.fileUrl
                        }
                    }
                    Rectangle{
                        id:m_rWebCamImport
                        x: 40
                        y: 45
                        width: 40
                        height: 25
                        color: "#E1E4EE"
                        TextBlack{
                            anchors.fill: parent
                            text: "导入"
                        }
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                m_rWebCamImport.color = "#98A5F5"
                            }
                            onExited: {
                                m_rWebCamImport.color = "#E1E4EE"
                            }
                            onClicked: {
                                m_fileDialogImport.open()
                            }
                        }
                    }
                    Rectangle{
                        id:m_rWebCamConfirm
                        x: 610
                        y: 10
                        width: 40
                        height: 25
                        color: "#E1E4EE"
                        TextBlack{
                            anchors.fill: parent
                            text: "确定"
                        }
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                m_rWebCamConfirm.color = "#98A5F5"
                            }
                            onExited: {
                                m_rWebCamConfirm.color = "#E1E4EE"
                            }
                            onClicked: {
                                UIBridge.openWebCamera(m_tiCameraURL.text)
                            }
                        }
                    }
                    Rectangle{
                        id:m_rWebCamStop
                        x: 610
                        y: 45
                        width: 40
                        height: 25
                        color: "#E1E4EE"
                        property var mState: false
                        TextBlack{
                            id: m_tWebCamStop
                            anchors.fill: parent
                            text: m_rWebCamStop.mState ? "开始" : "暂停"
                        }
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                m_rWebCamStop.color = "#98A5F5"
                            }
                            onExited: {
                                m_rWebCamStop.color = "#E1E4EE"
                            }
                            onClicked: {
                                m_rWebCamStop.mState = !m_rWebCamStop.mState
                                UIBridge.stopStartWebCamera(m_rWebCamStop.mState)
                            }
                        }
                    }
                }
                Item {
                    id: m_usbCamera
                    //摄像头下拉框,与绿幕选择摄像头共用
                    ComboBox {
                        id: m_cCamera
                        x: 90
                        y: 25
                        width: 245
                        height: 25
                        editable: false
                        font.pointSize: 16
                        font.family: "微软雅黑"
                        model: UIBridge.cameraList
                        //文本框背景
                        background: Rectangle {
                            color: b_ARFunction ? "#8A9CB7" : "#DAE0EA"
                        }
                        onCurrentIndexChanged: {
                            //切换摄像头
                            UIBridge.changeCamera(currentIndex)
                        }
                        contentItem: TextBlack {
                            text: m_cCamera.displayText
                            color: "#000000"
                        }
                        //每个item
                        delegate: ItemDelegate {
                            id:delegate
                            width: m_cCamera.width
                            height: m_cCamera.height
                            contentItem: Rectangle
                            {
                                anchors.centerIn: parent
                                color:"transparent"
                                TextBlack {
                                    width: parent.width
                                    height: parent.height
                                    text: modelData
                                }
                            }
                            highlighted: m_cCamera.highlightedIndex == index //鼠标移动时变色
                            //选择的样式
                            background: Rectangle{
                                color: m_cCamera.highlightedIndex == index?"#67ABFB":"#FFFFFF"
                            }
                        }
                        //三角符号指示器
                        indicator: Rectangle
                        {
                            x: parent.width - 26
                            y: 0
                            width: 26
                            height: 25
                            Canvas{
                                anchors.fill: parent
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.fillStyle = "#000000"
                                    ctx.beginPath()
                                    ctx.moveTo(5,5)
                                    ctx.lineTo(21,5)
                                    ctx.lineTo(13,17)
                                    ctx.closePath();
                                    ctx.fill();
                                }
                            }
                        }
                    }//---摄像头下拉框
                    //绿幕相机框不可选,显示"请从下方选则绿幕输入"
                    Rectangle{
                        x: 90
                        y: 25
                        width: 219
                        height: 25
                        visible: !b_ARFunction
                        color: "#DAE0EA"
                        TextBlack {
                            anchors.fill: parent
                            text: "请从下方选则绿幕输入"
                            color: "#B1B1B1"
                        }
                    }
                    //摄像头参数
                    TextBlack{
                        x: 350
                        y: 25
                        text:"摄像头参数:"
                    }
                    //摄像头参数下拉框
                    ComboBox {
                        id: m_cCameraSet
                        x: 430
                        y: 25
                        width: 200
                        height: 25
                        editable: false
                        font.pointSize: 16
                        font.family: "微软雅黑"
                        currentIndex: UIBridge.selectCameraSet
                        enabled: b_ARFunction ? true : UIBridge.gsSelectCamera
                        model: UIBridge.cameraSetList
                        //文本框背景
                        background: Rectangle {
                            color: b_ARFunction ? "#8A9CB7" : "#DAE0EA"
                        }
                        onModelChanged: {
                            currentIndex = UIBridge.selectCameraSet
                        }
                        onCurrentIndexChanged: {
                            //切换摄像头
                            UIBridge.changeCameraSet(currentIndex)
                        }
                        contentItem: TextBlack {
                            text: m_cCameraSet.displayText
                            color:b_ARFunction ? "#000000" : (UIBridge.gsSelectCamera ? "#000000" : "#B1B1B1")
                        }
                        //每个item
                        delegate: ItemDelegate {
                            width: m_cCameraSet.width
                            height: m_cCameraSet.height
                            contentItem: Rectangle
                            {
                                anchors.centerIn: parent
                                color:"transparent"
                                TextBlack {
                                    width: parent.width
                                    height: parent.height
                                    text: modelData
                                }
                            }
                            highlighted: m_cCameraSet.highlightedIndex == index //鼠标移动时变色
                            //选择的样式
                            background: Rectangle{
                                color: m_cCameraSet.highlightedIndex == index?"#67ABFB":"#FFFFFF"
                            }
                        }
                        //三角符号指示器
                        indicator: Rectangle
                        {
                            x: parent.width - 26
                            y: 0
                            width: 26
                            height: 25
                            Canvas{
                                anchors.fill: parent
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.fillStyle = "#000000"
                                    ctx.beginPath()
                                    ctx.moveTo(5,5)
                                    ctx.lineTo(21,5)
                                    ctx.lineTo(13,17)
                                    ctx.closePath();
                                    ctx.fill();
                                }
                            }
                        }
                    }//---摄像头参数下拉框
                }
                //虚拟摄像头复选框
                Controls12.CheckBox{
                    x:660
                    y:25
                    width: 100
                    height: 25
                    style: checkStyle
                    onCheckedChanged:{
                        //打开关闭虚拟摄像头
                        UIBridge.virturalCamera = checked
                    }
                }
                //虚拟摄像头复选框样式
                Component {
                    id: checkStyle;
                    Styles12.CheckBoxStyle{
                        indicator: Rectangle{
                            implicitWidth: 25
                            implicitHeight: 25
                            color: "#8A9CB7"
                            Canvas{
                                anchors.fill: parent
                                visible: control.checked
                                onPaint: {
                                    var ctx = getContext("2d")
                                    ctx.strokeStyle  = "#4296F9"
                                    ctx.lineWidth = 3
                                    ctx.beginPath()
                                    ctx.moveTo(5,12)
                                    ctx.lineTo(10,17)
                                    ctx.lineTo(20,8)
                                    ctx.stroke();
                                }
                            }
                        }
                        label: TextBlack{
                            text: "虚拟摄像头"
                        }
                    }
                }

                TextBlack {
                    x: 760
                    y: 25
                    width: 25
                    height: 25
                    text: "(?)"
                    color: "#B1B1B1"
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            m_rTip.visible = true
                        }
                        onExited: {
                            m_rTip.visible = false
                        }
                    }
                }
                Rectangle{
                    id:m_rTip
                    x: 580
                    y: 50
                    width: 370
                    height: 30
                    visible: false
                    radius: 8
                    TextBlack{
                        anchors.fill: parent
                        text: "勾选后会开启虚拟摄像头功能，详细可参见帮助文档里内容"
                    }
                }
                Rectangle{
                    id:m_rHelpDocument
                    x: 800
                    y: 10
                    width: 95
                    height: 25
                    color: "#E1E4EE"
                    TextBlack{
                        anchors.fill: parent
                        text: "打开帮助文档"
                    }
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            m_rHelpDocument.color = "#98A5F5"
                        }
                        onExited: {
                            m_rHelpDocument.color = "#E1E4EE"
                        }
                        onClicked: {
                            UIBridge.openHelpText()
                        }
                    }
                }

                Rectangle{
                    id:m_rSaveUserConfig
                    x: 800
                    y: 45
                    width: 95
                    height: 25
                    color: "#E1E4EE"
                    TextBlack{
                        anchors.fill: parent
                        text: "保存配置"
                    }
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            m_rSaveUserConfig.color = "#98A5F5"
                        }
                        onExited: {
                            m_rSaveUserConfig.color = "#E1E4EE"
                        }
                        onClicked: {
                            UIBridge.saveUserConfig()
                        }
                    }
                }
            }//---抬头

            //自定义美妆窗体
            Rectangle{
                id: m_rCustomMakeupWindow
                //选中一级下标 腮红 阴影 眉毛
                property var n_select: -1
                x: 935
                y: 60
                width: 415
                height: 750
                radius: 8
                visible: i_arSelectCategoryPoint.x == i_category_makeup && i_arSelectCategoryPoint.y == 0 && b_ARFunction
                //自定义美妆标题 腮红 阴影 眉毛
                GridView{
                    id: m_gCustomMakeupTitle
                    x: 40
                    y: 40
                    width: 385
                    height: 120
                    clip: true;
                    model: ListModel{
                        id: m_lmCustomMakeupTitle
                    }
                    cellWidth: 70;
                    cellHeight: 60;
                    delegate:RectangleButtonGreenScreen{
                        x: 10
                        y: 20
                        width: 50
                        height: 30
                        t_Text: text
                        border.width: 2
                        function updataSelect(select){
                            if(select === index){
                                b_Selected = true
                            }else{
                                b_Selected = false
                            }
                        }
                        function onButtonClick(){
                            for(var i = 0; i < m_gCustomMakeupTitle.count; i++){
                                m_gCustomMakeupTitle.itemAtIndex(i).updataSelect(index)
                            }
                            m_rCustomMakeupWindow.n_select = index
                            selectCustomMakeupTitel(index)
                        }
                    }
                }//---自定义美妆标题
                //自定义美妆二级选项 苹果肌 扇形
                ListView{
                    id: m_lCustomMakeup
                    x: 30
                    y: 140
                    width: 350
                    height: 100
                    orientation: ListView.Horizontal
                    clip: true
                    model: ListModel{
                        id: m_lmCustomMakeup
                    }
                    onCurrentIndexChanged:{
                        for(var i = 0; i < m_lCustomMakeup.count; i++){
                            m_lCustomMakeup.itemAtIndex(i).updataSelect(currentIndex)
                        }
                    }
                    delegate:Rectangle{
                        property var b_Selected: false
                        property var icon_Full: ""
                        Component.onCompleted: {
                            icon_Full =  "qrc:/res/" + icon
                            m_textCustomMakeup.text = text
                        }
                        width: 70
                        height: 100
                        function updataSelect(select){
                            if(select === index){
                                b_Selected = true
                            }else{
                                b_Selected = false
                            }
                        }
                        Rectangle{
                            x: 5
                            y: 5
                            width: 60
                            height: 60
                            border.color: b_Selected ? "#97A1D5" : "#969DB5"
                            border.width: b_Selected ? 4 : 2
                            Image {
                                x: 4
                                y: 4
                                width: 52
                                height: 52
                                source: icon_Full
                            }
                        }
                        TextBlack{
                            id: m_textCustomMakeup
                            x: 0
                            y: 65
                            width: 70
                            height: 30
                            text: text
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                m_lCustomMakeup.currentIndex = index
                                var selectIndex = m_lCustomMakeupColor.currentIndex
                                selectCustomMakeup(index)
                                if(selectIndex !== -1){
                                    m_lCustomMakeupColor.currentIndex = -1
                                    m_lCustomMakeupColor.currentIndex = selectIndex
                                }
                                UIBridge.setCustomMakeupIndex(m_rCustomMakeupWindow.n_select, index)
                                //切换重新设置颜色
                                if(m_lCustomMakeupColor.currentIndex == -1){
                                    m_lCustomMakeupColor.currentIndex = 0
                                }
                                UIBridge.setCustomMakeupColor(m_rCustomMakeupWindow.n_select,m_lCustomMakeupColor.currentIndex)
                            }
                        }
                    }
                }//---自定义美妆二级选项
                //美妆滑块
                ScrollBar  {
                    id: hbarCustomMakeup
                    hoverEnabled: true
                    active:  hovered || pressed
                    orientation: Qt.Horizontal
                    x: 30
                    y: 240
                    width: 350
                    height: 13
                    size: m_lCustomMakeup.width / m_lCustomMakeup.contentWidth
                    visible: m_lCustomMakeup.count > 5 ? true : false
                    //定义样式
                    contentItem: Rectangle {
                        radius: 8
                        color: hbarCustomMakeup.pressed ? "#828282" : "#707EED"
                    }
                    onPositionChanged: {
                        m_lCustomMakeup.contentX = position * (m_lCustomMakeup.contentWidth)
                    }
                }
                Connections{
                    target: m_lCustomMakeup
                    onContentXChanged: {
                        hbarCustomMakeup.position = m_lCustomMakeup.contentX / m_lCustomMakeup.contentWidth
                    }
                }//---美妆滑块
                //自定义美妆三级选项颜色
                ListView{
                    id: m_lCustomMakeupColor
                    x: 30
                    y: 255
                    width: 350
                    height: 100
                    orientation: ListView.Horizontal
                    clip: true
                    model: ListModel{
                        id: m_lmCustomMakeupColor
                    }
                    onCurrentIndexChanged:{
                        for(var i = 0; i < m_lCustomMakeupColor.count; i++){
                            m_lCustomMakeupColor.itemAtIndex(i).updataSelect(currentIndex)
                        }
                    }
                    delegate:Rectangle{
                        id: m_rCustomMakeupColor
                        width: 70
                        height: 70
                        property var b_Selected: false
                        function updataSelect(select){
                            if(select === index){
                                b_Selected = true
                            }else{
                                b_Selected = false
                            }
                        }
                        Rectangle{
                            x: 5
                            y: 5
                            width: 60
                            height: 60
                            border.color: "#97A1D5"
                            radius: 8
                            border.width: m_rCustomMakeupColor.b_Selected ? 4 : 0
                            Rectangle{
                                id: m_rShowColor
                                x: 4
                                y: 4
                                width: 52
                                height: 52
                                radius: 90
                                //根据颜色分层,如果color2有颜色分三层,如果color1有颜色分两层
                                gradient: Gradient {
                                    GradientStop { position: 0.0;  color: color0 }
                                    GradientStop { position: (color2 !== "" ? 0.33 : (color1 !== "" ? 0.5 : 2)); color: color0 }
                                    GradientStop { position: (color2 !== "" ? 0.34 : (color1 !== "" ? 0.51 : 2)); color: color1 }
                                    GradientStop { position: (color2 !== "" ? 0.66 : 2); color: color1 }
                                    GradientStop { position: (color2 !== "" ? 0.67 : 2); color: color2 }
                                    GradientStop { position: 1.0; color: (color2 !== "" ? color2 : (color1 !== "" ? color1 : color0)) }
                                }
                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: {
                                        m_lCustomMakeupColor.currentIndex = index
                                        UIBridge.setCustomMakeupColor(m_rCustomMakeupWindow.n_select,index)
                                    }
                                }
                            }
                        }
                    }
                }//---自定义美妆三级选项颜色
                //美妆颜色滑块
                ScrollBar  {
                    id: hbarCustomMakeupColor
                    hoverEnabled: true
                    active:  hovered || pressed
                    orientation: Qt.Horizontal
                    x: 30
                    y: 340
                    width: 350
                    height: 13
                    size: m_lCustomMakeupColor.width / m_lCustomMakeupColor.contentWidth
                    visible: m_lCustomMakeupColor.count > 5 ? true : false
                    //定义样式
                    contentItem: Rectangle {
                        radius: 8
                        color: hbarCustomMakeup.pressed ? "#828282" : "#707EED"
                    }
                    onPositionChanged: {
                        m_lCustomMakeupColor.contentX = position * (m_lCustomMakeupColor.contentWidth)
                    }
                }
                Connections{
                    target: m_lCustomMakeupColor
                    onContentXChanged: {
                        hbarCustomMakeup.position = m_lCustomMakeupColor.contentX / m_lCustomMakeupColor.contentWidth
                    }
                }//---美妆颜色滑块
                //底部滑块
                CustomSlider
                {
                    id: m_sliderMakeup
                    x: 80
                    y: 350
                    width: 255
                    height: 20
                    value: 100
                    visible: false
                    onValueChanged:
                    {
                        if(m_rCustomMakeupWindow.n_select != -1){
                            UIBridge.setCustomMakeupValue(m_rCustomMakeupWindow.n_select,value)
                        }
                        m_tsliderMakeup.text = value
                    }
                }
                TextBlack{
                    id: m_tsliderMakeup
                    visible: m_sliderMakeup.visible
                    anchors.centerIn: m_sliderMakeup
                    text: "100"
                }
                //一键卸妆
                RectangleButtonGreenScreen{
                    x: 142
                    y: 415
                    width: 130
                    height: 40
                    t_Text: "一键卸妆"
                    function onButtonClick(){
                        resetMakeup()
                    }
                }
            }//---自定义美妆窗体
            //选择avator后右侧提示
            Rectangle{
                x: 935
                y: 70
                width: 415
                height: 750
                radius: 8
                visible: i_arSelectCategoryPoint.x == i_category_avatar && i_arSelectCategoryPoint.y == 0
                Image {
                    x:10
                    y:300
                    width: 20
                    height: 20
                    source: "qrc:/res/icon_tips.png"
                }
                TextBlack{
                    x:50
                    y:300
                    text: "全身Avatar开启后美颜模块无法使用 如需编辑请先取消"
                }
            }//---选择avator后右侧提示
        }
    }
    //---总窗体
}
