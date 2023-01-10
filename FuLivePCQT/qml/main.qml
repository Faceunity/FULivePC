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
    y: bFullScreen ? 0 : 100
    width: bFullScreen ? Screen.desktopAvailableWidth : 1360
    height: bFullScreen ? Screen.desktopAvailableHeight : 870
    flags: Qt.FramelessWindowHint | Qt.Window
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
    //绿幕播放显示重播
    property var b_replayShow: false
    //底部列表第几页
    property var i_selectCategoryPage: 0
    //底部选中第几个道具项
    property var i_selectCategory: -1
    //弹出道具列表第几页
    property var i_propPage: 0
    property var i_propPageLast: 0
    property var i_propJingpinPage: 0
    //AR弹出道具选中第几个底部项,第几个弹出项目
    property var i_arSelectCategoryPoint: Qt.point(-1,-1)
    //绿幕背景视频选中第几个底部项
    property var i_gsSelectVideo: 0
    //绿幕安全区域选中第几个底部项
    property var i_gsSelectSaveArea: -1
    //选中精品贴纸第几个项目
    property var i_selectSticker: 0
    //渲染美体
    property var b_arBody: false
    //道具选项下标
    property var i_category_avatar: 9
    property var i_category_jingpin: 0
    property var i_category_backgroundSegmentation: 5
    property var i_category_makeup: 3
    property var i_category_lightMakeup: 4
    property var i_category_greenScreen: 15
    property var i_category_safeArea: 16
    //窗体大小改变
    onWidthChanged: {
        xScale = width * 1.0 / 1360.0
    }
    onHeightChanged: {
        yScale = height * 1.0  / 870.0
    }
    Component.onCompleted: {
        updateProp()
        updateBeautySkin()
        updateBeautyFace()
        updateBeautyBody()
        updateFilter()
        updateGSKeyColor()
        updateGreenScreen()
        loadCustomMakeupTitle()
        UIBridge.getCameraList()
        UIBridge.arBodyFlagChanged.connect(arBodyFlagChanged)
        UIBridge.callQmlRefeshImg.connect(callQmlRefeshImg)
        UIBridge.selectColorChanged.connect(selectColorChanged)
        UIBridge.finishDownload.connect(finishDownload)
        UIBridge.switchARChanged.connect(switchARFunction)
        updateBoutiqueSticker(0)
    }
    Connections {
        target: UIBridge
        //显示提示文字计时器开始
        onTipChanged:{
            m_rTipText.visible = true
            timerLoadTime.start()
        }
        onTipExtraChanged:{
            if(time == 0){
                m_rTipTextExtra.visible = false
                timerLoadTimeExtra.stop()
            }else{
                m_rTipTextExtra.visible = true
                timerLoadTimeExtra.interval = time * 1000
                timerLoadTimeExtra.start()
            }
        }
        onUpdateCheckModule:{
            updateCheckModule()
        }
        onUpdateReplayButton:{
            b_replayShow = flag
        }
        onUpdateGSPreviewRect:{
            m_itGSForegroundArea.x = m_CameraDisplay.width * startx
            m_itGSForegroundArea.y = m_CameraDisplay.height * starty
            m_itGSForegroundArea.width = m_CameraDisplay.width * (endx - startx)
            m_itGSForegroundArea.height = m_CameraDisplay.height * (endy - starty)
        }
        onUpdateGreenScreenParam:{
            updateGreenScreen()
        }
        onUpdateBeautySkinParam:{
            updateBeautySkin()
        }
        onUpdateBeautyFaceParam:{
            updateBeautyFace()
        }
        onUpdateBsgPic: {
            //选中第二个背景图片
            i_arSelectCategoryPoint.y = 1
            //更新自定义背景图片
            updatePropOption(i_category_backgroundSegmentation)
        }
        onUpdateGSSafePic: {
            //选中第二个绿幕安全图片
            i_gsSelectSaveArea = 1
            //更新自定义绿幕安全区域图片
            updatePropOption(i_category_safeArea - i_category_greenScreen)
        }
        onUpdateCameraIndex:{
            m_cCamera.currentIndex = index
        }
        onUpdateCameraSet:{
            m_cCameraSet.currentIndex = index
        }
        onUpdateBodyTrackType:{
            i_avatarType = type
        }
        onUpdateSelectCategory:{
            if(x < i_category_greenScreen){
                i_arSelectCategoryPoint = Qt.point(x,y)
                i_selectCategory = x
                if(x == 0 && y == 0){
                    showAvator(true)
                }else if(x == i_category_jingpin){
                    i_selectSticker = z
                    m_lStickerTips.currentIndex = z
                }else{
                    updatePropOption(x)
                }
            }else{
                b_ARFunction = false
            }
        }
        onUpdateGSSelectIndex:{
            b_setGreenScreen = true
            switchARFunction(false)
            UIBridge.arFunction = false
            i_gsSelectVideo = x
            updatePropOption(0)
            i_gsSelectSaveArea = y
            if(y >= 0){
                updatePropOption(1)
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
    Timer{
        id: timerLoadTimeExtra
        interval: 2000
        repeat: false
        onTriggered:{
            m_rTipTextExtra.visible = false
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
        if(m_rBoutiqueSticker.visible){
            m_rBoutiqueSticker.enabled = !arBody
        }else if(m_rpropOption.visible){
            m_rpropOption.enabled = !arBody
        }
    }
    //AR功能/绿幕切换
    function switchARFunction(arfun){
        b_ARFunction = arfun
        i_selectCategoryPage = 0
        updateProp()
        updateCheckModule()
        m_rpropOption.visible = false
        m_rPropShadow.visible = false
        m_rPropOptionBackgroundSeg.visible = false
        m_rBoutiqueSticker.visible = false
        showGreenScreenList(false)
        b_replayShow = false
        if(!arfun){
            m_rpropOption.enabled = true
            i_propPageLast = i_propPage
            i_propPage = 0
            m_propOptionListView.contentX = 0
            showAvator(false)
            m_lProp.x = 380
            m_lProp.contentX = 0
            //第一次没选择摄像头或视频时遮挡
            m_rGreenScreen.visible = !b_setGreenScreen
            m_rPropShadow.visible = !b_setGreenScreen
            m_maCameraDisplay.enabled = true
            //绿幕默认选中第一个
            if(i_gsSelectSaveArea >= 0){
                i_selectCategory = i_category_safeArea - i_category_greenScreen
                updatePropOption(i_category_safeArea - i_category_greenScreen)
            }else{
                i_selectCategory = 0
                updatePropOption(0)
            }
            //如果切绿幕在美体,切成美肤
            if(m_rBeautyBody.b_Selected == true){
                updateTapContent(0)
            }
        }else{
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
                updatePropOption(i_arSelectCategoryPoint.x)
                if(i_arSelectCategoryPoint.x == i_category_avatar){
                    showAvator(true)
                }else if(i_arSelectCategoryPoint.x == i_category_jingpin){
                    showBoutiqueSticker(true)
                }
                i_propPage =  i_propPageLast
                m_propOptionListView.contentX += 760 * i_propPage
            }
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
        }else{
            m_rBoutiqueSticker.visible = false
        }
    }
    //精品贴纸选择中级道具|高级道具|效果验证|游戏道具
    function updateBoutiqueSticker(index){
        m_lStickerTips.currentIndex = index
        UIBridge.setStickerIndex(index)
        i_propJingpinPage = 0
        m_lStickerBundles.contentX = 0
        m_lmStickerBundles.clear()
        for(var i = 0; i < UIBridge.getStickerLength(); i++){
            if(i_category_jingpin === i_arSelectCategoryPoint.x && i === i_arSelectCategoryPoint.y && i_selectSticker === index){
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
    function updateProp(){
        m_lmProp.clear()
        if(b_ARFunction){
            m_lmProp.append( { icon: "Jinpin", text:"精品贴纸" , selected: false, selecteditem:false, enabledItem: false})
            m_lmProp.append( { icon: "Propmap", text:"道具贴图" , selected: false, selecteditem:false, enabledItem: false})
            m_lmProp.append( { icon: "style_recommendation", text:"风格推荐", selected: false, selecteditem:false, enabledItem: false})
            m_lmProp.append( { icon: "makeup", text:"美妆", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "lightmakeup", text:"轻美妆", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "Bgsegmentation", text:"人像分割", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "gesturerecognition", text:"手势识别", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "Expressionrecognition", text:"表情识别", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "annimoji", text:"Animoji", selected: false, selecteditem:false, enabledItem: false})
            m_lmProp.append( { icon: "avatar", text:"Avatar", selected: false, selecteditem:false, enabledItem: false})
            m_lmProp.append( { icon: "hairdressing", text:"美发", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "AR", text:"AR面具", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "Musicfilter", text:"音乐滤镜", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "Hahamirror", text:"哈哈镜", selected: false, selecteditem:false , enabledItem: false})
            m_lmProp.append( { icon: "photo_sticker", text:"搞笑大头", selected: false, selecteditem:false , enabledItem: false})
        }else{
            m_lmProp.append( { icon: "green_screen_cutout", text:"绿幕", selected: false, selecteditem:false, enabledItem: false})
            m_lmProp.append( { icon: "safe_area", text:"安全区域", selected: false, selecteditem:false, enabledItem: false})
        }
    }
    //鉴权后更新图标
    function updateCheckModule(){
        for(var i = 0; i< m_lmProp.count; i++)
        {
            var checkModule;
            if(b_ARFunction){
                checkModule = UIBridge.checkModule[i]
            }else{
                checkModule = UIBridge.checkModule[i + i_category_greenScreen]
            }
            if(checkModule === "true"){
                m_lmProp.setProperty(i, "enabledItem", true)
            }else{
                m_lmProp.setProperty(i, "enabledItem", false)
            }
        }
        if(UIBridge.checkModule[i_category_greenScreen] === "true"){
            m_rARGSShadow.visible = false
        }else{
            m_rARGSShadow.visible = true
        }
    }
    //更新底部弹出框选项
    function updatePropOption(index){
        m_propOptionListView.contentX = 0
        m_lmPropOption.clear()
        if(b_ARFunction){
            for(var i = 0; i< UIBridge.categoryBundles[index].length; i++)
            {
                var iconValue = UIBridge.categoryBundles[index][i];
                var pathType = true
                //如果是人像分割
                if(index === i_category_backgroundSegmentation && iconValue === "bg_seg_shot"){
                    pathType = false
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
                var pathType1 = true
                var iconValue1 = UIBridge.greenScreenIcon[index][j];
                if(index === (i_category_safeArea - i_category_greenScreen) && iconValue1 === "gs_savearea_shot"){
                    pathType1 = false
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
    function updateBeautySkin(){
        m_lmBeautySkin.clear()
        for(var i = 0; i< UIBridge.beautySkin[0].length; i++)
        {
            m_lmBeautySkin.append({"text":UIBridge.beautySkin[0][i], "icon":UIBridge.beautySkin[1][i], "value":UIBridge.beautySkin[3][i]})
        }
        m_lmBeautySkin.append({"text": "恢复默认", "icon": "", "value": ""})
    }
    //读取美型
    function updateBeautyFace(){
        m_lmBeautyFace.clear()
        for(var i = 0; i< UIBridge.beautyFace[0].length; i++)
        {
            m_lmBeautyFace.append({"text":UIBridge.beautyFace[0][i], "icon":UIBridge.beautyFace[1][i],
                                      "value":UIBridge.beautyFace[3][i], "middle":UIBridge.beautyFace[4][i]})
        }
        m_lmBeautyFace.append({"text": "恢复默认", "icon": "", "value": "", "middle": ""})
    }
    //读取滤镜
    function updateFilter(){
        m_lmFilter.clear()
        for(var i = 0; i< UIBridge.filter[0].length; i++)
        {
            m_lmFilter.append({"textFilter":UIBridge.filter[0][i],"icon":UIBridge.filter[1][i], "value":UIBridge.filter[2][i]})
        }
    }
    //读取美体
    function updateBeautyBody(){
        m_lmBeautyBody.clear()
        for(var i = 0; i< UIBridge.beautyBody[0].length; i++)
        {
            m_lmBeautyBody.append({"text":UIBridge.beautyBody[0][i], "icon":UIBridge.beautyBody[1][i],
                                      "value":UIBridge.beautyBody[3][i], "middle":UIBridge.beautyBody[4][i]})
        }
        m_lmBeautyBody.append({"text": "恢复默认", "icon": "", "value": "", "middle": ""})
    }
    //读取绿幕参数
    function updateGreenScreen(){
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
        for(var i = 0; i < 10; i++)
        {
            m_lmCustomMakeupTitle.append({"text":UIBridge.getCustomMakeup(i)[0][0], "value":UIBridge.getCustomMakeup(i)[0][3]})
        }
    }
    //选中自定义美妆标题按钮 腮红,阴影等展示苹果肌,扇形
    function selectCustomMakeupTitel(index){
        m_lmCustomMakeup.clear()
        for(var i = 1; i < UIBridge.getCustomMakeup(index).length; i++){
            if(UIBridge.getCustomMakeup(index)[i][2] !== ""){
                m_lmCustomMakeup.append({"text":UIBridge.getCustomMakeup(index)[i][0], "icon":UIBridge.getCustomMakeup(index)[i][2]})
            }
        }
        //修改选中项
        var selectMakeup = Number(UIBridge.getCustomMakeup(index)[0][4])
        var selectColor = Number(UIBridge.getCustomMakeup(index)[0][5])
        if(selectMakeup === -1){
            m_rCustomMakeupWindow.b_fSliderMakeup = false
            m_sliderMakeup.value = 0
            m_rCustomMakeupWindow.b_fSliderMakeup = true
        }
        m_lCustomMakeup.contentX = 0
        m_lCustomMakeup.currentIndex = -1
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
        //修改滑块值
        if(m_lmCustomMakeup.count != 0){
            m_sliderMakeup.value = UIBridge.getCustomMakeup(m_rCustomMakeupWindow.n_select)[0][3]
        }
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
    function updateTapContent(id){
        m_rBeautySkin.b_Selected = false
        m_rBeautyFace.b_Selected = false
        m_rFilter.b_Selected = false
        m_rBeautyBody.b_Selected = false
        m_lBeautySkin.visible = false
        m_lBeautyFace.visible = false
        m_itFilter.visible = false
        m_lBeautyBody.visible = false
        UIBridge.updateTapContent(id)
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
            m_itFilter.visible = true
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
        m_bGetColor.b_Selected = false
        m_rARGSShadow.visible = false
        m_rPropShadow.visible = false
        m_maSelectColor.enabled = false
        m_rpropOption.visible = true
        UIBridge.gsUnSelectColor()
    }
    //取色获得点击颜色
    function selectColorChanged(getcolor){
        m_rColor.color = getcolor
        if(getcolor === "#FFFFFF"){
            m_rColor.border.width = 2
        }else{
            m_rColor.border.width = 0
        }
        //更新列表
        if(getcolor !== "#FFFFFF" && getcolor !== "#0000FF" &&getcolor !== "#00FF00"){
            updateGSKeyColor()
        }
        if(getcolor !== ""){
            selectColorListChanged(getcolor)
        }
    }
    //更新选中
    function selectColorListChanged(selectColor){
        for(var i = 0; i < m_lvSelectColor.count; i++){
            m_lvSelectColor.itemAtIndex(i).updateSelect(selectColor)
        }
        for(var j = 0; j < m_lvSelectColorRencent.count; j++){
            m_lvSelectColorRencent.itemAtIndex(j).updateSelect(selectColor)
        }
    }
    //一键卸妆
    function resetMakeup(){
        m_rCustomMakeupWindow.b_fSliderMakeup = false
        m_sliderMakeup.value = 0
        m_rCustomMakeupWindow.b_fSliderMakeup = true
        m_lCustomMakeup.currentIndex = -1
        if(m_lmCustomMakeup.count != 0){
            m_lCustomMakeupColor.visible = false
        }else{
            m_lCustomMakeupColor.currentIndex = -1
        }
        UIBridge.resetCustomMakeup()
    }
    //更新最近颜色
    function updateGSKeyColor(){
        var colorlength = UIBridge.gsKeyColor.length
        while(m_lmSelectColorRencent.count < colorlength){
            m_lmSelectColorRencent.append({"vColor": ""})
        }
        for(var i = 0; i< UIBridge.gsKeyColor.length; i++)
        {
            m_lmSelectColorRencent.setProperty(i, "vColor",UIBridge.gsKeyColor[i])
        }
        //强制刷新
        m_lvSelectColorRencent.forceLayout()
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
            height: bFullScreen ? 870 : 700
            radius: 8
            color: "#FFFFFF"
            //显示摄像头图像区域
            CameraDisplay{
                id: m_CameraDisplay
                x: bFullScreen ? 0 :10
                y: bFullScreen ? 0 : 10
                width: bFullScreen ? 1360 : 864
                height: bFullScreen ? 870 : 486
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
                    enabled: i_selectCategory == i_category_jingpin
                    onClicked: {
                        UIBridge.itemJingpinClick()
                    }
                }
                //绿幕左键点击拖动,滚轮放大缩小
                MouseArea{
                    id: m_maCameraDisplay
                    property point clickPoint: "0,0"
                    property var b_Pressed: false
                    property var changeX: 0.0
                    property var changeY: 0.0
                    propagateComposedEvents: true
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
                        UIBridge.gsTranslocation(changeX, changeY)
                    }
                    onWheel:{
                        if(wheel.modifiers & Qt.ControlModifier){
                            var datl = wheel.angleDelta.y/120
                            if(datl>0){
                                UIBridge.gsZoom(true)
                            }else{
                                UIBridge.gsZoom(false)
                            }
                        }
                    }
                    onReleased: {
                        b_Pressed = false
                        UIBridge.gsUpdatelocation(changeX, changeY)
                    }
                }
                //绿幕分割实际图像区域
                Item{
                    id: m_itGSForegroundArea
                    Image{
                        anchors.centerIn: parent
                        visible: !b_ARFunction && b_replayShow
                        width: 56
                        height: 56
                        source: "qrc:/res/btn_paly.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                b_replayShow = false
                                UIBridge.gsVideoMediaReplay()
                            }
                        }
                    }
                }
            }
            //点击绿幕选择颜色
            MouseArea{
                id: m_maSelectColor
                enabled: false
                anchors.fill: m_CameraDisplay
                cursorShape: m_maSelectColor.enabled ? Qt.BlankCursor : Qt.ArrowCursor
                hoverEnabled: true
                onClicked: {
                    UIBridge.selectColor(mouseX, mouseY)
                    completeSelectColor()
                }
                Image{
                    x: m_maSelectColor.mouseX
                    y: m_maSelectColor.mouseY - 20
                    width: 20
                    height: 20
                    visible: m_maSelectColor.enabled && m_maSelectColor.containsMouse
                    source: "qrc:/res/icon_take_color.png"
                }
            }
            //avatar时显示右下角摄像头图像
            Image{
                id: m_showImage
                anchors.bottom: m_CameraDisplay.bottom
                anchors.right: m_CameraDisplay.right
                anchors.rightMargin: 15
                anchors.bottomMargin: m_rpropOption.visible? 95 : 15
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
                    color: "#3D3F53"
                    visible: false
                    Rectangle{
                        anchors.centerIn: parent
                        width: 360
                        height: 315
                        radius: 5
                        Image {
                            x: 140
                            y: 20
                            width: 78
                            height: 78
                            source: "qrc:/res/icon_prompt.png"
                        }
                        TextBlack{
                            x: 35
                            y: 110
                            text:"请使用纯色背景，推荐绿色幕布效果效果最佳"
                        }
                        RectangleButtonSolid{
                            x: 80
                            y: 170
                            width: 200
                            height: 40
                            t_Text: "选择摄像头"
                            function onButtonClick(){
                                m_greenScreenSelectCamera.visible = true
                                b_replayShow = false
                                m_cCamera.visible = true
                                UIBridge.gsCameraImage = true
                                UIBridge.gsCameraPlay = true
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
                                    UIBridge.startMediaPlayer()
                                }
                            }

                        }
                        RectangleButtonSolid{
                            x: 80
                            y: 220
                            width: 200
                            height: 40
                            t_Text: "选择视频/图片"
                            function onButtonClick(){
                                m_fileDialog.open()
                            }
                        }
                        TextBlack{
                            id: m_tGSCancel
                            x: 300
                            y: 10
                            width: 60
                            height: 25
                            text: "取  消"
                            color: "#2F3658"
                            font.underline: true
                            visible: b_setGreenScreen
                            MouseArea{
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    m_tGSCancel.color = "#7787E9"
                                }
                                onExited: {
                                    m_tGSCancel.color = "#2F3658"
                                }
                                onClicked: {
                                    m_rGreenScreen.visible = false
                                    UIBridge.startMediaPlayer()
                                }

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
                    radius: 20
                    color: "#596078"
                    opacity: 0.78
                    visible: !m_rGreenScreen.visible
                    TextBlack{
                        color:"#FFFFFF"
                        anchors.fill: parent
                        text: "显示性能参数"
                    }
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            m_bPerformance.color = "#CC596078"
                        }
                        onExited: {
                            m_bPerformance.color = "#596078"
                        }
                        onClicked: {
                            m_bPerformance.visible = false
                            m_rParameter.visible = true
                        }
                    }
                }

                RectangleButtonImage{
                    v_Source_nor: "qrc:/res/icon_reselect.png"
                    v_Source_sel: "qrc:/res/icon_reselect.png"
                    t_Text: "重新输入"
                    function onButtonClick(){
                        m_rGreenScreen.visible = true
                        UIBridge.stopMediaPlayer()
                    }
                    id: m_rSelectGreenScreen
                    x: 730
                    y: 20
                    width: 120
                    height: 40
                    radius: 20
                    opacity: 0.78
                    visible: !m_rGreenScreen.visible && !b_ARFunction && b_setGreenScreen
                }
                //显示性能参数值
                Rectangle{
                    id: m_rParameter
                    visible: false
                    x: 20
                    y: 20
                    width: 160
                    height: 100
                    color: "#CC596078"
                    radius: 8
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
                    TextBlack{
                        id: m_tParameterClose
                        x: 120
                        y: 75
                        width: 40
                        height: 20
                        color: "#FFFFFF"
                        text: "关闭"
                        font.underline: true
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                m_tParameterClose.color = "#CCFFFFFF"
                            }
                            onExited: {
                                m_tParameterClose.color = "#FFFFFF"
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
                    height: 120
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
                        b_SelectedItem: selecteditem
                        b_Enabled: enabledItem
                        //重新选择道具列表选中
                        onSelectedChange: {
                            var rowCount = m_lmProp.count
                            showBoutiqueSticker(false)
                            //遍历道具列表
                            for( var i = 0;i < rowCount;i++ ) {
                                var model = m_lmProp.get(i)
                                if(model.icon === selectName){
                                    model.selected = !model.selected
                                    m_rpropOption.visible = false
                                    m_rPropOptionBackgroundSeg.visible = false
                                    if(i === i_category_jingpin && b_ARFunction){
                                        //更新精品贴纸
                                        i_selectCategory = i_category_jingpin
                                        UIBridge.selectCategory = i_category_jingpin
                                        showBoutiqueSticker(selected)
                                    }else if(b_ARFunction){
                                        if(i === i_category_backgroundSegmentation && b_ARFunction){
                                            //更新人像分割会议选项
                                            m_rPropOptionBackgroundSeg.visible = model.selected
                                        }
                                        m_rpropOption.visible = model.selected
                                        i_propPage = 0
                                        //切换选中状态,展示道具列表
                                        if(model.selected && i_selectCategory !== i){
                                            updatePropOption(i)
                                            UIBridge.selectCategory = i
                                            i_selectCategory = i
                                            showGreenScreenList(false)
                                        }
                                    }else{
                                        m_rpropOption.visible = model.selected
                                        if(model.selected){
                                            updatePropOption(i)
                                            UIBridge.selectCategory = i + i_category_greenScreen
                                            i_selectCategory = i
                                            i_propPage = 0
                                        }
                                        //如果在绿幕状态显示绿幕参数设置
                                        showGreenScreenList(model.selected)
                                    }
                                }else{
                                    model.selected = false
                                }
                            }
                            //有道具选中状态提示
                            for( var j = 0;j < rowCount;j++ ) {
                                var model1 = m_lmProp.get(j)
                                model1.selecteditem = false
                                if(i_arSelectCategoryPoint.x === j && i_arSelectCategoryPoint.y >= 0){
                                    if(!m_rpropOption.visible && !m_rBoutiqueSticker.visible && !m_rPropOptionBackgroundSeg.visible){
                                        model1.selecteditem = true
                                    }
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
                    height: 120
                    color: "#C8FFFFFF"
                    visible: false
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                Image{
                    x: 430
                    y: 620
                    width: 24
                    height: 24
                    source: i_selectCategoryPage == 0 ? "qrc:/res/icon_lbutton_nor.png" : "qrc:/res/icon_lbutton.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(i_selectCategoryPage != 0){
                                i_selectCategoryPage--
                                m_lProp.contentX = 0
                            }
                        }
                    }
                }
                Image{
                    x: 480
                    y: 620
                    width: 24
                    height: 24
                    source: i_selectCategoryPage == 1 ?  "qrc:/res/icon_rbutton_nor.png" : "qrc:/res/icon_rbutton.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            if(i_selectCategoryPage != 1){
                                i_selectCategoryPage++
                                m_lProp.contentX = 900
                            }
                        }
                    }
                }
                //道具列表自定义滑块
                //                ScrollBar  {
                //                    id: hbar
                //                    hoverEnabled: true
                //                    active:  hovered || pressed
                //                    orientation: Qt.Horizontal
                //                    x:0
                //                    y:645
                //                    width: 910
                //                    height: 13
                //                    size: m_lProp.width / m_lProp.contentWidth
                //                    policy: ScrollBar.AsNeeded
                //                    //定义样式
                //                    contentItem: Rectangle {
                //                        radius: 8
                //                        color: hbar.pressed ? "#828282" : "#707EED"
                //                    }
                //                    onPositionChanged: {
                //                        m_lProp.contentX = position * (m_lProp.contentWidth)
                //                    }
                //                }
                //                Connections{
                //                    target: m_lProp
                //                    onContentXChanged: {
                //                        hbar.position = m_lProp.contentX / m_lProp.contentWidth
                //                    }
                //                }
                //弹出道具选择列表
                Rectangle{
                    id: m_rpropOption
                    x: 10
                    y: 416
                    width: 864
                    height: 80
                    opacity: enabled ? 1 : 0.7
                    color: "#E0E0E3EE"
                    visible: false
                    Image{
                        x: 10
                        y: 30
                        width: 24
                        height: 24
                        visible: i_propPage != 0
                        source: "qrc:/res/icon_lbutton.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                if(i_propPage != 0){
                                    i_propPage--
                                    m_propOptionListView.contentX -= 764
                                }
                            }
                        }
                    }
                    Image{
                        x: 830
                        y: 30
                        width: 24
                        height: 24
                        visible: m_lmPropOption.count > 10 && i_propPage != Math.ceil(m_lmPropOption.count / 9) - 1
                        source: "qrc:/res/icon_rbutton.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                if(i_propPage != Math.ceil(m_lmPropOption.count / 9) - 1){
                                    i_propPage++
                                    m_propOptionListView.contentX += 764
                                }
                            }
                        }
                    }
                    ListView {
                        id: m_propOptionListView
                        x: m_lmPropOption.count == 10 ? 0 : m_lmPropOption.count < 10 ? 7.5 + (10 - m_lmPropOption.count) * 42.5 : 50
                        y: 0
                        width: m_lmPropOption.count <= 10 ? 864 : 764
                        height: 80
                        orientation: ListView.Horizontal
                        spacing: 5
                        interactive:false
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
                                    var model = m_lmPropOption.get(i)
                                    if(model.icon === selectName){
                                        model.selected = !model.selected
                                        if(model.selected){
                                            if(b_ARFunction){
                                                i_arSelectCategoryPoint = Qt.point(i_selectCategory, i)
                                                updateBoutiqueSticker(0)
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
                                            if(UIBridge.selectCategory === i_category_avatar){
                                                showAvator(false)
                                            }else{
                                                UIBridge.nonuseProps()
                                            }
                                            i_arSelectCategoryPoint = Qt.point(-1, -1)
                                        }
                                    }else{
                                        model.selected = false
                                    }
                                }
                            }
                        }
                    }
                }//---弹出道具选择列表
                //用于人像分割选择列表
                Rectangle{
                    id: m_rPropOptionBackgroundSeg
                    x: 10
                    y: 416
                    width: 864
                    height: 80
                    color: "#E0E3EE"
                    visible: false
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                    Rectangle{
                        property var b_hover: false
                        x: 250
                        y: 20
                        width: 180
                        height: 40
                        color: b_hover ? "#7888E6" : "#FFFFFF"
                        border.color: "#E1E1E1"
                        radius: 90
                        TextBlack{
                            text: "通用分割版"
                            font {family: "微软雅黑"; pixelSize: 18;}
                            anchors.fill: parent
                        }
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                UIBridge.setBackgroundSegType(1)
                                m_rPropOptionBackgroundSeg.visible = false
                            }
                            onEntered: {
                                parent.b_hover = true
                            }
                            onExited: {
                                parent.b_hover = false
                            }
                        }
                    }
                    Rectangle{
                        property var b_hover: false
                        x: 480
                        y: 20
                        width: 180
                        height: 40
                        color: b_hover ? "#7888E6" :"#FFFFFF"
                        border.color: "#E1E1E1"
                        radius: 90
                        TextBlack{
                            text: "视频会议版"
                            font {family: "微软雅黑"; pixelSize: 18;}
                            anchors.fill: parent
                        }
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                UIBridge.setBackgroundSegType(0)
                                m_rPropOptionBackgroundSeg.visible = false
                            }
                            onEntered: {
                                parent.b_hover = true
                            }
                            onExited: {
                                parent.b_hover = false
                            }
                        }
                    }
                }
                //精品贴纸
                Rectangle{
                    id: m_rBoutiqueSticker
                    x: 10
                    y: 366
                    width: 864
                    height: 130
                    color: "#E0E0E3EE"
                    visible: false
                    opacity: enabled ? 1 : 0.7
                    //中级道具|高级道具|效果验证|游戏道具
                    Rectangle{
                        x: 300
                        y: 0
                        width: 564
                        height: 30
                        color: "#66FFFFFF"
                    }
                    ListView {
                        id: m_lStickerTips
                        x: 0
                        y: 0
                        width: 864
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
                            width: 100
                            height: 30
                            TextBlack{
                                anchors.fill: parent
                                text: tipText
                                color: m_lStickerTips.currentIndex != index  ? "#992F3658" : "#2F3658"
                            }
                            color: m_lStickerTips.currentIndex == index ? "#00FFFFFF" : "#66FFFFFF"
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
                                    updateBoutiqueSticker(index)
                                }
                            }
                        }
                    }//---中级道具|高级道具
                    Image{
                        x: 10
                        y: 60
                        width: 24
                        height: 24
                        visible: i_propJingpinPage != 0
                        source: "qrc:/res/icon_lbutton.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                if(i_propJingpinPage != 0){
                                    i_propJingpinPage--
                                    m_lStickerBundles.contentX -= 760
                                    if(m_lStickerBundles.contentX < 0){
                                        m_lStickerBundles.contentX = 0
                                    }
                                }
                            }
                        }
                    }
                    Image{
                        x: 830
                        y: 60
                        width: 24
                        height: 24
                        visible: i_propJingpinPage != Math.ceil(m_lmStickerBundles.count / 9) - 1
                        source: "qrc:/res/icon_rbutton.png"
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                if(i_propJingpinPage != Math.ceil(m_lmStickerBundles.count / 9) - 1){
                                    i_propJingpinPage++
                                    m_lStickerBundles.contentX += 760
                                }
                            }
                        }
                    }
                    //精品贴纸道具选项
                    ListView {
                        id: m_lStickerBundles
                        x: m_lmStickerBundles.count == 10 ? 0 : m_lmStickerBundles.count < 10 ? 7.5 + (10 - m_lmStickerBundles.count) * 42.5 : 50
                        y: 30
                        width: m_lmStickerBundles.count <= 10 ? 864 : 764
                        height: 80
                        orientation: ListView.Horizontal
                        spacing: 5
                        clip: true
                        cacheBuffer: 10000
                        interactive:false
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
                                        var rowCount = m_lmStickerBundles.count
                                        var selectName = icon
                                        for( var i = 0;i < rowCount; i++ ) {
                                            var model = m_lmStickerBundles.get(i)
                                            if(model.icon === selectName){
                                                model.selected = !model.selected
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
                                                model.selected = false
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
                //AR功能/绿幕抠像
                Rectangle{
                    x: 340
                    y: 655
                    width: 270
                    height: 32
                    radius: 16
                    color: "#FFFFFF"
                    border.color: "#848496CD"
                    Rectangle{
                        id: m_rARbackgroud
                        x: b_ARFunction ? 0 : 135
                        y: 0
                        radius: 16
                        width: 135
                        height: 32
                        color: "#7787E9"
                    }
                    TextBlack{
                        id: m_tAR
                        x: 45
                        y: 6
                        text: "AR功能"
                        color: b_ARFunction ? "#FFFFFF" : "#5C6071"
                    }
                    TextBlack{
                        id: m_tGS
                        x: 175
                        y: 6
                        text: "绿幕抠像"
                        color: b_ARFunction ? "#5C6071" : "#FFFFFF"
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
                                    switchARFunction(false)
                                    UIBridge.arFunction = false
                                }
                            }else if(!UIBridge.arFunction && mouseX < 135){
                                if(!UIBridge.gsSelectCamera){
                                    UIBridge.setRenderNewFrame()
                                    UIBridge.selectCategory = i_arSelectCategoryPoint.x
                                    UIBridge.arFunction = true
                                }else{
                                    switchARFunction(true)
                                    UIBridge.arFunction = true
                                }
                            }
                        }
                    }
                }//---AR功能/绿幕抠像
                //用于遮挡AR功能/绿幕抠像
                Rectangle{
                    id: m_rARGSShadow
                    x: 340
                    y: 653
                    width: 270
                    height: 34
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
            Image {
                x: 0
                y: 0
                width: 1360
                height: 60
                source: "qrc:/res/title.png"
                MouseArea{
                    enabled: !m_window.bFullScreen
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton //只处理鼠标左键
                    property point clickPos: "0,0"
                    onPressed: { //接收鼠标按下事件
                        clickPos  = Qt.point(mouse.x,mouse.y)
                    }
                    onPositionChanged: { //鼠标按下后改变位置
                        //鼠标偏移量
                        var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                        m_window.setX(m_window.x+delta.x)
                        m_window.setY(m_window.y+delta.y)
                    }
                }
                Image{
                    property var hoverflag: false
                    x: 6
                    y: 6
                    width: 12
                    height: 12
                    source: hoverflag ? "qrc:/res/window_Controls_1_hover.png" : "qrc:/res/window_Controls_1_nor.png"
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            parent.hoverflag = true
                        }
                        onExited: {
                            parent.hoverflag = false
                        }
                        onClicked: {
                            m_window.close()
                        }
                    }
                }
                Image{
                    property var hoverflag: false
                    x: 23
                    y: 6
                    width: 12
                    height: 12
                    source: hoverflag ? "qrc:/res/window_Controls_2_hover.png" : "qrc:/res/window_Controls_2_nor.png"
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            parent.hoverflag = true
                        }
                        onExited: {
                            parent.hoverflag = false
                        }
                        onClicked: {
                            m_window.showMinimized()
                        }
                    }
                }
                Image{
                    property var hoverflag: false
                    x: 40
                    y: 6
                    width: 12
                    height: 12
                    source: hoverflag ? "qrc:/res/window_Controls_3_hover.png" : "qrc:/res/window_Controls_3_nor.png"
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: {
                            parent.hoverflag = true
                        }
                        onExited: {
                            parent.hoverflag = false
                        }
                        onClicked: {
                            if(m_window.visibility == Window.Maximized){
                                m_window.showNormal()
                            }else{
                                m_window.showMaximized()
                            }
                        }
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
                Rectangle{
                    id: m_rTipTextExtra
                    x: 432 - m_tipTextExtra.contentWidth / 2
                    y: 440
                    width: m_tipTextExtra.contentWidth + 20
                    radius: 8
                    height: 30
                    color: "#C8FFFFFF"
                    visible: false
                    TextBlack{
                        id:m_tipTextExtra
                        text: UIBridge.tipExtra
                        anchors.fill: parent
                    }
                }
                //侧边功能窗体 美肤 美型 滤镜 美体
                Rectangle{
                    id: m_rSideWindow
                    x: 930
                    y: 70
                    width: 420
                    height: 790
                    radius: 5
                    Row {
                        x: 0
                        y: 0
                        width: 420
                        height: 30
                        spacing: 0
                        RectangleTabContent{
                            id: m_rBeautySkin
                            b_Selected: true
                            i_Tab_ID: 0
                            t_Text: "美肤"
                            b_GreenScreen: !b_ARFunction
                            onSelectedChange: {
                                updateTapContent(i_Tab_ID)
                            }
                        }
                        RectangleTabContent{
                            id: m_rBeautyFace
                            i_Tab_ID: 1
                            t_Text: "美型"
                            b_GreenScreen: !b_ARFunction
                            onSelectedChange: {
                                updateTapContent(i_Tab_ID)
                            }
                        }
                        RectangleTabContent{
                            id: m_rFilter
                            i_Tab_ID: 2
                            t_Text: "滤镜"
                            b_GreenScreen: !b_ARFunction
                            onSelectedChange: {
                                updateTapContent(i_Tab_ID)
                            }
                        }
                        RectangleTabContent{
                            id: m_rBeautyBody
                            i_Tab_ID: 3
                            t_Text: "美体"
                            visible: b_ARFunction
                            onSelectedChange: {
                                updateTapContent(i_Tab_ID)
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
                                UIBridge.updateItemParam(0, index, value)
                            }
                            //恢复默认
                            function defaultClick() {
                                UIBridge.resetItemParam(0)
                                //                                var list = [1, 3, 70, 30, 30, 20, 0, 0, 0, 0, 0]
                                //                                for(var i = 0; i < m_lmBeautySkin.count - 1; i++){
                                //                                    m_lBeautySkin.itemAtIndex(i).resetValue(list[i])
                                //                                }
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
                        cacheBuffer: 10000
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
                                UIBridge.updateItemParam(1, index, value)
                            }
                            function defaultClick() {
                                UIBridge.resetItemParam(1)
                                //                                var list = [0, 0, 0, -20, -20, 50, -10, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
                                //                                for(var i = 0; i < m_lmBeautyFace.count - 1; i++){
                                //                                    m_lBeautyFace.itemAtIndex(i).resetValue(list[i])
                                //                                }
                            }
                        }
                    }//---美型窗体
                    //滤镜窗体
                    Item{
                        id: m_itFilter
                        x: 15
                        y: 50
                        width: 400
                        height: 680
                        visible: false
                        GridView{
                            id: m_gFilter
                            anchors.fill: parent
                            clip: true
                            model: ListModel{
                                id: m_lmFilter
                            }
                            cellWidth: 133
                            cellHeight: 133
                            delegate:Rectangle{
                                id: m_crFilter
                                property var b_inMouse: false
                                property var b_Selected: index == 0 ? true : false
                                width: 110
                                height: 110
                                function updateSelect(select){
                                    if(select === index){
                                        b_Selected = true
                                    }else{
                                        b_Selected = false
                                    }
                                }
                                Image{
                                    x: 2
                                    y: 2
                                    width: 106
                                    height: 106
                                    source: "qrc:/res/list_image_" + icon + ".png"
                                }
                                Image{
                                    x: 2
                                    y: 2
                                    width: 106
                                    height: 106
                                    source: b_Selected ? "qrc:/res/list_image_filter_selected.png" :
                                                         (m_crFilter.b_inMouse ? "qrc:/res/list_image_filter_hover.png" :
                                                                                 "qrc:/res/list_image_filter_nor.png" )
                                }
                                TextBlack{
                                    x: 2
                                    y: b_Selected ? 2 : 82
                                    width: 106
                                    height: b_Selected ? 106 : 26
                                    color: "#FFFFFF"
                                    text: textFilter
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
                                        m_slider.value = UIBridge.filter[2][index]
                                        //更新选中颜色
                                        for(var i = 0; i < m_gFilter.count; i++){
                                            m_gFilter.itemAtIndex(i).updateSelect(index)
                                        }
                                    }
                                }
                            }
                        }
                        //底部滑块
                        CustomSlider
                        {
                            id: m_slider
                            x: 0
                            y: 580
                            width: 340
                            height: 24
                            visible: m_gFilter.currentIndex != 0
                            value: 0
                            onValueChanged:
                            {
                                if(m_gFilter.currentIndex != -1){
                                    UIBridge.setFilter(value)
                                }
                            }
                        }
                        RectangleButtonText{
                            id: m_rbFilter
                            x: 130
                            y: 635
                            width: 130
                            height: 40
                            t_Text: "恢复默认"
                            visible: m_gFilter.currentIndex != 0
                            enabled: m_slider.value != 40
                            function onButtonClick() {
                                if(m_gFilter.currentIndex > 0){
                                    var list = [0, 40, 40, 40, 40, 40]
                                    m_slider.value = (list[m_gFilter.currentIndex])
                                }
                            }
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
                                UIBridge.updateItemParam(2, index, value)
                            }
                            function defaultClick(){
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
                    x: 930
                    y: 70
                    width: 420
                    height: 770
                    radius: 8
                    visible: false
                    Rectangle{
                        x: 0
                        y: 0
                        width: 420
                        height: 35
                        color: "#2D2956"
                        TextBlack{
                            anchors.centerIn: parent
                            color: "#FFFFFF"
                            text: "抠像参数"
                        }
                    }
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
                        y: 55
                        width: 420
                        height: 85
                        Rectangle {
                            id: m_rColor
                            x: 36
                            y: 5
                            width: 52
                            height: 52
                            radius: 90
                            color: "#00FF00"
                            border.color: "#E9EAF2"
                            onColorChanged: {
                                UIBridge.gsColorChange(color)
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    if(!m_bGetColor.b_Selected){
                                        m_rSelectColor.visible = ! m_rSelectColor.visible
                                    }
                                }
                            }
                            Image{
                                x: 37
                                y: 37
                                width: 14
                                height: 14
                                source: "qrc:/res/iconn_colour_picker.png"
                            }
                        }
                        TextBlack{
                            x: 36
                            y: 65
                            width: 52
                            height: 20
                            text:"关键色"
                        }
                        RectangleButtonImage{
                            id: m_bGetColor
                            x: 120
                            y: 25
                            width: 80
                            height: 30
                            radius: 5
                            v_Source_nor: "qrc:/res/icon_straw_nor.png"
                            v_Source_sel: "qrc:/res/icon_straw_sel.png"
                            t_Text: "取色"
                            function onButtonClick(){
                                //第一次点击取色
                                if(!m_bGetColor.b_Selected){
                                    m_rSelectColor.visible = false
                                    m_bGetColor.b_Selected = true
                                    //遮挡下方ar,绿幕切换
                                    m_rARGSShadow.visible = true
                                    m_rPropShadow.visible = true
                                    //取色区域可用
                                    m_maSelectColor.enabled = true
                                    m_rpropOption.visible = false
                                    //设置绿幕图像位置最大化
                                    UIBridge.gsSelectColor()
                                }else{
                                    completeSelectColor()
                                }
                            }
                        }
                    }
                    ListView {
                        id: m_lGreenScreen
                        x: 0
                        y: 145
                        width: 420
                        height: 660
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
                                    UIBridge.updateItemParam(3, index, value)
                                }
                            }
                            //绿幕点击恢复默认
                            function defaultClick() {
                                UIBridge.resetItemParam(3)
                                m_rColor.color = "#00FF00"
                                var list = [50, 30, 66]
                                for(var i = 0; i < m_lmGreenScreen.count - 1; i++){
                                    m_lGreenScreen.itemAtIndex(i).resetValue(list[i])
                                }
                                //如果是绿幕区域状态
                                //                                if(i_gsSelectSaveArea >= 0){
                                //                                    i_gsSelectSaveArea = -1
                                //                                    if(UIBridge.selectCategory === i_category_safeArea){
                                //                                        updatePropOption(1)
                                //                                    }else{
                                //                                        UIBridge.selectCategory = i_category_safeArea
                                //                                    }
                                //                                    UIBridge.nonuseProps()
                                //                                }
                            }
                        }
                    }
                    //选择颜色窗口
                    Rectangle{
                        id: m_rSelectColor
                        property var n_selectColor: 0
                        border.color: "#E9EAF2"
                        border.width: 1
                        radius: 5
                        x: 36
                        y: 100
                        width: 250
                        height: 180
                        visible: false
                        TextBlack{
                            x: 30
                            y: 30
                            text: "推荐颜色"
                        }
                        ListView{
                            id: m_lvSelectColor
                            x: 30
                            y: 60
                            width: 200
                            height: 30
                            clip: true
                            model: ListModel{
                                id: m_lmSelectColor
                                ListElement { vColor: "#00FF00" }
                                ListElement { vColor: "#0000FF" }
                                ListElement { vColor: "#FFFFFF" }
                            }
                            spacing: 8
                            orientation: ListView.Horizontal
                            delegate: ColorCircle{
                                v_Color: vColor
                                function updateSelect(selectColor){
                                    if(selectColor === v_Color){
                                        b_Selected = true
                                    }else{
                                        b_Selected = false
                                    }
                                }
                                function onButtonClick(){
                                    selectColorChanged(v_Color)
                                }
                            }
                        }
                        TextBlack{
                            x: 30
                            y: 90
                            text: "最近使用"
                        }
                        ListView{
                            id: m_lvSelectColorRencent
                            x: 30
                            y: 120
                            width: 200
                            height: 30
                            clip: true
                            model: ListModel{
                                id: m_lmSelectColorRencent
                            }
                            spacing: 8
                            orientation: ListView.Horizontal
                            delegate: ColorCircle{
                                v_Color: vColor
                                function updateSelect(selectColor){
                                    if(selectColor === v_Color){
                                        b_Selected = true
                                    }else{
                                        b_Selected = false
                                    }
                                }
                                function onButtonClick(){
                                    selectColorChanged(v_Color)
                                }
                            }
                        }
                    }
                }//---绿幕窗体
                //绿幕选择摄像头
                Rectangle{
                    id: m_greenScreenSelectCamera
                    x: 210
                    y: 220
                    width: 480
                    height: 400
                    radius: 5
                    visible: false
                    Rectangle{
                        x: 0
                        y: 0
                        width: parent.width
                        radius: 5
                        height: 35
                        color: "#F7F8FA"
                        TextBlack{
                            anchors.centerIn: parent
                            text:"摄像头设置"
                        }
                        TextBlack{
                            x: parent.width - 35
                            y: 0
                            width: 35
                            height: 35
                            text:"X"
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    UIBridge.gsCameraImage = false
                                    UIBridge.gsCameraPlay = false
                                    m_greenScreenSelectCamera.visible = false
                                }
                            }
                        }
                    }
                    //绿幕摄像头图像
                    Image{
                        id: m_showImageCamera
                        x: 16
                        y: 55
                        width: 448
                        height: 252
                    }
                    TextBlack{
                        x: 16
                        y: 320
                        text:"选择设备:"
                    }
                    //---绿幕摄像头图像
                    RectangleButtonSolid{
                        x: 400
                        y: 365
                        width: 60
                        height: 24
                        t_Text: "确定"
                        radius: 5
                        function onButtonClick(){
                            UIBridge.gsCameraImage = false
                            UIBridge.gsCameraPlay = true
                            UIBridge.gsCameraConfirm()
                            b_setGreenScreen = true
                            m_greenScreenSelectCamera.visible = false
                            m_rGreenScreen.visible = false
                            m_rPropShadow.visible = false
                        }
                    }
                    RectangleButtonText{
                        x: 330
                        y: 365
                        width: 60
                        height: 24
                        t_Text: "取消"
                        function onButtonClick(){
                            UIBridge.gsCameraImage = false
                            UIBridge.gsCameraPlay = false
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
                        font.pixelSize: 16
                        color:"#2F3658"
                        text:"选择摄像头"
                        //双击打开网络摄像头
                        MouseArea{
                            property var webcam:false
                            anchors.fill: parent
                            onClicked: {
                                //屏蔽usb摄像头
                                if(!webcam){
                                    webcam = true
                                    m_textSelectCamera.text = "选择网络摄像头"
                                    m_webCamera.visible = true
                                    m_usbCamera.visible = false
                                    UIBridge.changeCameraType(true)
                                }else{
                                    webcam = false
                                    m_textSelectCamera.text = "选择摄像头"
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
                            border.color: "#5C6071"
                            TextInput{
                                id:m_tiCameraURL
                                x: 5
                                y: 0
                                width: 485
                                height: 25
                                color: "#5C6071"
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
                            x: m_greenScreenSelectCamera.visible ? 280 : 90
                            y: m_greenScreenSelectCamera.visible ? 470 : 25
                            width: m_greenScreenSelectCamera.visible ? 385 : 245
                            height: 25
                            editable: false
                            font.pointSize: 16
                            font.family: "微软雅黑"
                            model: UIBridge.cameraList
                            //文本框背景
                            background: Rectangle {
                                radius: 2
                                border.color: m_cCamera.hovered ? "#7787E9" : "#959CB4"
                                border.width: 1
                                color: m_cCamera.hovered ? "#F0F4FF" : "#FFFFFF"
                            }
                            onCurrentIndexChanged: {
                                //切换摄像头
                                UIBridge.changeCamera(currentIndex)
                            }
                            contentItem: TextBlack {
                                text: m_cCamera.displayText
                                color: "#5C6071"
                            }
                            //每个item
                            delegate: ItemDelegate {
                                id: delegate
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
                                    color: m_cCamera.highlightedIndex == index? "#F0F4FF" : "#FFFFFF"
                                }
                            }
                            //三角符号指示器
                            indicator: Rectangle
                            {
                                x: parent.width - 26
                                y: 0
                                width: 25
                                height: 25
                                color: m_cCamera.hovered ? "#7787E9" : "#959CB4"
                                Canvas{
                                    anchors.fill: parent
                                    onPaint: {
                                        var ctx = getContext("2d")
                                        ctx.fillStyle = "#FFFFFF"
                                        ctx.beginPath()
                                        ctx.moveTo(8,9)
                                        ctx.lineTo(19,9)
                                        ctx.lineTo(13,15)
                                        ctx.closePath()
                                        ctx.fill()
                                    }
                                }
                            }
                        }//---摄像头下拉框
                        //绿幕相机框不可选,显示"请从下方选则绿幕输入"
                        Rectangle{
                            x: 90
                            y: 25
                            width: 245
                            height: 25
                            visible: !b_ARFunction
                            color: "#FFFFFF"
                            border.color: "#DEE0E8"
                            border.width: 1
                            radius: 2
                            TextBlack {
                                anchors.fill: parent
                                text: "请从下方选择绿幕输入"
                                color: "#DEE0E8"
                            }
                        }
                        //摄像头参数
                        TextBlack{
                            x: 350
                            y: 25
                            text:"摄像头参数"
                            font.pixelSize: 16
                            color:"#2F3658"
                        }
                        //摄像头参数下拉框
                        ComboBox {
                            id: m_cCameraSet
                            x: 440
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
                                radius: 2
                                border.color: m_cCameraSet.hovered ? "#7787E9" : "#959CB4"
                                border.width: 1
                                color: m_cCameraSet.hovered ? "#F0F4FF" : "#FFFFFF"
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
                                color: "#5C6071"
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
                                    color:  m_cCameraSet.highlightedIndex == index? "#F0F4FF" : "#FFFFFF"
                                }
                            }
                            //三角符号指示器
                            indicator: Rectangle
                            {
                                x: parent.width - 26
                                y: 0
                                width: 25
                                height: 25
                                color: m_cCameraSet.hovered ? "#7787E9" : "#959CB4"
                                Canvas{
                                    anchors.fill: parent
                                    onPaint: {
                                        var ctx = getContext("2d")
                                        ctx.fillStyle = "#FFFFFF"
                                        ctx.beginPath()
                                        ctx.moveTo(8,9)
                                        ctx.lineTo(19,9)
                                        ctx.lineTo(13,15)
                                        ctx.closePath()
                                        ctx.fill()
                                    }
                                }
                            }
                        }//---摄像头参数下拉框
                    }
                    //虚拟摄像头复选框
                    Controls12.CheckBox{
                        x:655
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
                        id: checkStyle
                        Styles12.CheckBoxStyle{
                            indicator: Rectangle{
                                implicitWidth: 20
                                implicitHeight: 20
                                radius: 2
                                color: control.checked ? "#7787E9" : "#FFFFFF"
                                border.color: control.checked ? "#FFFFFF" : "#959CB4"
                                Canvas{
                                    anchors.fill: parent
                                    visible: control.checked
                                    onPaint: {
                                        var ctx = getContext("2d")
                                        ctx.strokeStyle  = "#FFFFFF"
                                        ctx.lineWidth = 2
                                        ctx.beginPath()
                                        ctx.moveTo(5,9)
                                        ctx.lineTo(9,13)
                                        ctx.lineTo(15,6)
                                        ctx.stroke()
                                    }
                                }
                            }
                            label: TextBlack{
                                font.pixelSize: 16
                                color:"#2F3658"
                                text: "  虚拟摄像头"
                            }
                        }
                    }

                    Image {
                        x: 770
                        y: 28
                        width: 20
                        height: 20
                        source: m_rTip.visible ? "qrc:/res/icon_notes_hover.png" :"qrc:/res/icon_notes_nor.png"
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

                    TextBlack{
                        id: m_tHelpDocument
                        x: 800
                        y: 10
                        width: 95
                        height: 25
                        text: "打开帮助文档"
                        color: "#2F3658"
                        font.underline: true
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                m_tHelpDocument.color = "#7787E9"
                            }
                            onExited: {
                                m_tHelpDocument.color = "#2F3658"
                            }
                            onClicked: {
                                UIBridge.openHelpText()
                            }

                        }
                    }

                    TextBlack{
                        id: m_tSaveUserConfig
                        x: 800
                        y: 45
                        width: 95
                        height: 25
                        text: "保存配置"
                        color: "#2F3658"
                        font.underline: true
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                m_tSaveUserConfig.color = "#7787E9"
                            }
                            onExited: {
                                m_tSaveUserConfig.color = "#2F3658"
                            }
                            onClicked: {
                                UIBridge.saveUserConfig(true)
                            }

                        }
                    }
                }//---抬头

                //自定义美妆窗体
                Rectangle{
                    id: m_rCustomMakeupWindow
                    //选中一级下标 腮红 阴影 眉毛
                    property var n_select: -1
                    //滑块切换显示0
                    property var b_fSliderMakeup: true
                    x: 930
                    y: 70
                    width: 420
                    height: 750
                    radius: 8
                    visible: i_arSelectCategoryPoint.x == i_category_makeup && i_arSelectCategoryPoint.y == 0 && b_ARFunction
                    onVisibleChanged: {
                        //默认选中第一个
                        if(visible){
                            for(var i = 0; i < m_lvCustomMakeupTitle.count; i++){
                                m_lvCustomMakeupTitle.itemAtIndex(i).updateSelect(m_lvCustomMakeupTitle.currentIndex)
                            }
                            m_rCustomMakeupWindow.n_select = m_lvCustomMakeupTitle.currentIndex
                            selectCustomMakeupTitel(m_lvCustomMakeupTitle.currentIndex)
                        }
                    }
                    Rectangle{
                        x: 0
                        y: 0
                        width: 420
                        height: 35
                        color: "#2D2956"
                        TextBlack{
                            anchors.centerIn: parent
                            color: "#FFFFFF"
                            text: "自定义参数"
                        }
                    }
                    //自定义美妆标题 腮红 阴影 眉毛
                    ListView{
                        id: m_lvCustomMakeupTitle
                        x: 10
                        y: 60
                        width: 385
                        height: 120
                        clip: true
                        model: ListModel{
                            id: m_lmCustomMakeupTitle
                        }
                        spacing: 8
                        orientation: ListView.Horizontal
                        delegate:RectangleButtonText{
                            width: 54
                            height: 30
                            t_Text: text
                            function updateSelect(select){
                                if(select === index){
                                    b_Select = true
                                }else{
                                    b_Select = false
                                }
                            }
                            function onButtonClick(){
                                for(var i = 0; i < m_lvCustomMakeupTitle.count; i++){
                                    m_lvCustomMakeupTitle.itemAtIndex(i).updateSelect(index)
                                }
                                m_rCustomMakeupWindow.n_select = index
                                selectCustomMakeupTitel(index)
                            }
                        }
                    }//---自定义美妆标题
                    //自定义美妆二级选项 苹果肌 扇形
                    GridView{
                        id: m_lCustomMakeup
                        x: 20
                        y: 120
                        width: 420
                        height: 280
                        flow: GridView.FlowLeftToRight
                        clip: true
                        model: ListModel{
                            id: m_lmCustomMakeup
                        }
                        onCurrentIndexChanged:{
                            for(var i = 0; i < m_lCustomMakeup.count; i++){
                                m_lCustomMakeup.itemAtIndex(i).updateSelect(currentIndex)
                            }
                        }
                        cellWidth: 98
                        cellHeight: 140
                        delegate:Rectangle{
                            property var b_Selected: false
                            property var icon_Full: ""
                            Component.onCompleted: {
                                icon_Full =  "qrc:/res/" + icon
                                m_textCustomMakeup.text = text
                            }
                            width: 98
                            height: 120
                            function updateSelect(select){
                                if(select === index){
                                    b_Selected = true
                                }else{
                                    b_Selected = false
                                }
                            }
                            Rectangle{
                                x: 0
                                y: 0
                                width: 86
                                height: 86
                                radius: 4
                                border.color: "#7787E9"
                                border.width: b_Selected ? 3 : 0
                                Image {
                                    x: 3
                                    y: 3
                                    width: 80
                                    height: 80
                                    source: icon_Full
                                }
                            }
                            TextBlack{
                                id: m_textCustomMakeup
                                x: 3
                                y: 95
                                width: 80
                                height: 20
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
                    //自定义美妆三级选项颜色
                    ListView{
                        id: m_lCustomMakeupColor
                        x: 30
                        y: m_lCustomMakeup.count == 0 ? 120 : m_lCustomMakeup.count > 4 ? 395 : 255
                        width: 350
                        height: 100
                        orientation: ListView.Horizontal
                        clip: true
                        model: ListModel{
                            id: m_lmCustomMakeupColor
                        }
                        onCurrentIndexChanged:{
                            for(var i = 0; i < m_lCustomMakeupColor.count; i++){
                                m_lCustomMakeupColor.itemAtIndex(i).updateSelect(currentIndex)
                            }
                            //粉底一键卸妆后更新滑块值
                            if(m_sliderMakeup.value == 0 && m_lmCustomMakeup.count == 0 && currentIndex != -1){
                                UIBridge.setCustomMakeupIndex(7, 0)
                                m_sliderMakeup.value = UIBridge.getCustomMakeup(m_rCustomMakeupWindow.n_select)[0][3]
                            }
                        }
                        delegate:Rectangle{
                            id: m_rCustomMakeupColor
                            width: 70
                            height: 70
                            property var b_Selected: false
                            function updateSelect(select){
                                if(select === index){
                                    b_Selected = true
                                }else{
                                    b_Selected = false
                                }
                            }
                            Rectangle{
                                x: 5
                                y: 5
                                width: 48
                                height: 48
                                border.color: "#7787E9"
                                radius: 90
                                border.width: m_rCustomMakeupColor.b_Selected ? 3 : 0
                                Rectangle{
                                    id: m_rShowColor
                                    x: 4
                                    y: 4
                                    width: 40
                                    height: 40
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
                    //底部滑块
                    CustomSlider
                    {
                        id: m_sliderMakeup
                        x: 15
                        y: 630
                        width: 340
                        height: 24
                        value: 0
                        enabled: m_lCustomMakeupColor.currentIndex >= 0
                        onValueChanged:
                        {
                            if(m_rCustomMakeupWindow.n_select != -1 && m_rCustomMakeupWindow.b_fSliderMakeup){
                                UIBridge.setCustomMakeupValue(m_rCustomMakeupWindow.n_select,value)
                            }
                        }
                    }
                    //一键卸妆
                    RectangleButtonText{
                        x: 140
                        y: 685
                        enabled: m_sliderMakeup.value != 0 && (m_lCustomMakeup.currentIndex != -1 || m_lmCustomMakeup.count == 0)
                        t_Text: "一键卸妆"
                        function onButtonClick(){
                            resetMakeup()
                        }
                    }
                }//---自定义美妆窗体
                //选择avator后右侧提示
                Rectangle{
                    x: 930
                    y: 70
                    width: 420
                    height: 790
                    color: "#C8FFFFFF"
                    visible: i_arSelectCategoryPoint.x == i_category_avatar && i_arSelectCategoryPoint.y == 0
                    MouseArea{
                        anchors.fill: parent
                    }
                    Rectangle{
                        x: 35
                        y: 360
                        width: 350
                        height: 90
                        radius: 8
                        border.color: "#E1E1E1"
                        border.width: 1
                        Image {
                            x: 30
                            y: 15
                            width: 20
                            height: 20
                            source: "qrc:/res/icon_tips.png"
                        }
                        TextBlack{
                            x: 0
                            y: 15
                            width: 350
                            text: "全身Avatar开启后美颜模块无法使用"
                        }
                        TextBlack{
                            x: 0
                            y: 55
                            width: 350
                            text: "如需编辑请先取消"
                        }
                    }
                }//---选择avator后右侧提示
            }
        }
        //---总窗体
    }
}

