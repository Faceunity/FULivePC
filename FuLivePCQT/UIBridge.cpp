#include "UIBridge.h"
#include <QDir>
#include <QDebug>
#include <QStringList>
#include "MainClass.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include "Nama.h"
#include "Camera.h"
#include <QMediaPlaylist>
#include <future>
#include <CNamaSDK.h>
#include <QBitmap>
#include <QPainter>
#include <QThread>

#pragma execution_character_set("utf-8")

UIBridge::UIBridge()
{
    connect(this, SIGNAL(updateConfig()), this, SLOT(updateUserConfig()));
    initBodyTrackConfig();
    //fps定时器刷新
    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(getFPS()));
    m_timer->start(1000);
    //avator小窗口
    m_ImageProvider = new ImageProvider();
    //美肤,美型,美体,绿幕相同
    //第一个QStringList是qml中显示列表文字,括号中分成按钮显示
    //第二个QStringList是qml中显示图标中间名
    //第三个QStringList是nama中fuItemSetParamd设置道具名
    //第四个QStringList是nama中fuItemSetParamd设置值,也是qml滑块值和按钮选中项
    //第五个QStringList是qml中滑块显示是否在中间,true中间[-50,50],false[0,100],""转换qml为false,"1"转换为true
    //美肤
    m_beautySkin.append(QStringList{"精准美肤 开启|关闭", "美肤模式 均匀磨皮|精细磨皮|清晰磨皮|朦胧磨皮", "磨皮", "祛斑痘",
                                    "美白", "红润", "清晰", "锐化", "五官立体",
                                    "亮眼","美牙", "去黑眼圈", "去法令纹"});
    m_beautySkin.append(QStringList{ "skinbeauty", "BeautyMode", "Grindingskin", "acne",
                                     "Skinwhitening", "Ruddy", "clearness","sharpen", "stereoscopic",
                                     "Brighteye", "Beautifulteeth", "dark_circles", "wrinkle"});
    m_beautySkin.append(QStringList{ "skin_detect", "blur_type", "blur_level", "delspot_level",
                                     "color_level_mode2", "red_level", "clarity", "sharpen", "face_threed",
                                     "eye_bright", "tooth_whiten", "remove_pouch_strength_mode2", "remove_nasolabial_folds_strength_mode2"});
    m_defaultBeautySkin = QStringList{ "1", "3", "70", "0",
            "30", "30", "0", "20", "0",
            "0", "0", "0", "0"};
    m_beautySkin.append(m_defaultBeautySkin);
    m_beautySkin.append(QStringList{ "","","","","","","","","","","","",""});
    //美型
    m_beautyFace.append(QStringList{ "瘦脸", "大眼", "圆眼", "下巴", "额头",
                                     "瘦鼻", "嘴型", "嘴唇厚度", "V脸", "窄脸",
                                     "短脸", "小脸", "瘦颧骨", "瘦下颌骨", "眼睛位置",
                                     "开眼角", "眼睑下至", "眼距", "眼睛角度", "长鼻",
                                     "缩人中", "微笑嘴角", "眉毛上下", "眉间距", "眉毛粗细"});
    m_beautyFace.append(QStringList{ "Thinface", "Bigeye", "round_eye", "chin", "forehead",+
                                     "Thinnose", "Mouthtype", "mouth_thickness", "v", "narrow_face",
                                     "short_face", "little_face", "cheekbones", "lower_jaw", "eye_position",
                                     "open_eyes", "eyelid_down", "eye_distance", "eye_angle", "proboscis",
                                     "shrinking", "smile_mouth", "eyebrow_position", "eyebrow_spacing", "eyebrow_thickness"});
    m_beautyFace.append(QStringList{ "cheek_thinning_mode2", "eye_enlarging_mode3", "intensity_eye_circle", "intensity_chin_mode2", "intensity_forehead_mode2",
                                     "intensity_nose_mode2", "intensity_mouth_mode3", "intensity_lip_thick", "cheek_v", "cheek_narrow_mode2",
                                     "cheek_short", "cheek_small_mode2", "intensity_cheekbones","intensity_lower_jaw", "intensity_eye_height",
                                     "intensity_canthus", "intensity_eye_lid", "intensity_eye_space", "intensity_eye_rotate","intensity_long_nose",
                                     "intensity_philtrum", "intensity_smile", "intensity_brow_height", "intensity_brow_space", "intensity_brow_thick"});
    m_defaultBeautyFace = QStringList{ "0", "0", "0", "-20", "-20",
            "50", "-10", "0", "50", "0",
            "0", "0", "0", "0", "0",
            "0", "0", "0", "0", "0",
            "0", "0", "0", "0", "0"};
    m_beautyFace.append(m_defaultBeautyFace);
    m_beautyFace.append(QStringList{ "","","","1","1",
                                     "","1","1","","",
                                     "","","","","1",
                                     "","","1","1","1",
                                     "1","","1","1","1"});
    //美体
    m_beautyBody.append(QStringList{ "瘦身", "长腿", "瘦腰", "美肩", "美臀", "小头", "瘦腿"});
    m_beautyBody.append(QStringList{ "slimming", "stovepipe", "thin_waist", "shoulder", "hip", "little_head", "thin_leg"});
    m_beautyBody.append(QStringList{ "BodySlimStrength","LegSlimStrength" ,"WaistSlimStrength" , "ShoulderSlimStrength",
                                     "HipSlimStrength" , "HeadSlim", "LegSlim"});
    m_defaultBeautyBody = QStringList{ "0", "0", "0", "0", "0", "0", "0"};
    m_beautyBody.append(m_defaultBeautyBody);
    m_beautyBody.append(QStringList{ "","","","1","","",""});
    //滤镜
    //第一个QStringList是qml中显示列表文字,括号中分成按钮显示
    //第二个QStringList是qml中显示图标中间名,也是nama中fuItemSetParamd设置道具名
    //第三个QStringList是qml中显示滑块值,也是设置nama中filter_level的参数（需要/100.0)
    m_filter.append(QStringList{ "原图", "白亮", "粉嫩", "小清新", "冷色调", "暖色调"});
    m_filter.append(QStringList{ "origin", "bailiang1", "fennen1", "xiaoqingxin1", "lengsediao1", "nuansediao1"});
    m_filter.append(QStringList{ "0", "40", "40", "40", "40", "40"});
    //绿幕
    m_greenScreen.append(QStringList{"相似度", "平滑", "祛色度"});
    m_greenScreen.append(QStringList{"tolerance", "smooth", "transparency"});
    m_greenScreen.append(QStringList{"chroma_thres", "chroma_thres_T", "alpha_L"});
    m_defaultGreenScreen = QStringList{ "50", "30", "66"};
    m_greenScreen.append(m_defaultGreenScreen);
    m_greenScreen.append(QStringList{ "","",""});

    //绿幕道具、图片名
    m_greenScreenIcon.append(QStringList{"green_screen_bg_atechnology", "green_screen_bg_beach", "green_screen_bg_classroom",
                                         "green_screen_bg_ink painting", "green_screen_bg_jforest"});
    m_greenScreenIcon.append(QStringList{"gs_savearea", "safe_area_high", "safe_area_low"});

    //自定义美妆
    //一为显示中文名,二为道具名,三为对应强度名称,四为强度系数,五为选中下标,六选中颜色下标
    m_tempCustomMakeup.append(QStringList{"腮红", "makeup_blusher_color", "makeup_intensity_blusher", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"阴影", "makeup_shadow_color", "makeup_intensity_shadow", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"眉毛", "makeup_eyeBrow_color", "makeup_intensity_eyeBrow", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"睫毛", "makeup_eyelash_color", "makeup_intensity_eyelash", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"眼线", "makeup_eyeLiner_color", "makeup_intensity_eyeLiner", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"美瞳", "makeup_pupil_color", "makeup_intensity_pupil", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"眼影", "makeup_eye_color", "makeup_intensity_eye", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"粉底", "makeup_foundation_color", "makeup_intensity_foundation", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"高亮", "makeup_highlight_color", "makeup_intensity_highlight", "100", "-1", "-1"});
    m_tempCustomMakeup.append(QStringList{"口红", "makeup_lip_color", "makeup_intensity_lip", "100", "-1", "-1"});
    readCustomMakeup();
    //保存一份默认,一键卸妆用
    m_defaultCustomMakeup = m_customMakeup;
    //读取道具
    readCategoryBundle();
    //读取道具提示
    readBundleTip();
    //读取json配置
    readUserConfig();
    //读取风格推荐参数
    readStyleRecommendation();
    m_styleRecommendationName = QStringList{"QiZhi", "XueJie", "ZhiYa", "DanYan", "ZhiGan", "HanGuoXueMei", "AiLing", "YuanSheng", "JingDian", "NvShen", "NanShen"};
    m_styleRecommendationIndex = m_styleRecommendationParam.mNameList.indexOf("BeautyParam");
    QStringList strList;
    m_gsKeyColor = strList;
    emit updateGSPreviewRect(float(m_gsStart.x()), float(m_gsStart.y()), float(m_gsStart.x() + m_gsSize.x()), float(m_gsStart.y() + m_gsSize.y()));
}

UIBridge::~UIBridge()
{
}

void UIBridge::readBundleTip()
{
    QFile file(":/res/config.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<tr("config.json路径错误");
        return;
    }
    QString value = file.readAll();
    file.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug()<<tr("解析config.json文件错误！");
        return;
    }
    QJsonObject jsonObject = document.object();
    for(auto it = jsonObject.begin(); it != jsonObject.end(); it++)
    {
        m_tipMap.insert(it.key().split(".").first(),it.value().toString());
    }
}

void UIBridge::readCategoryBundle()
{
    //这里根据Bundle路径找到所有bundle并在res找到相应名称的图标显示
    for(int i = 0; i < BundleCategory::Count; i++){
        QStringList tempList;
        if(i == BundleCategory::GreenScreen){
            return;
        }else if(i == BundleCategory::ItemJingpin){
            m_categoryBundles.append(QStringList());
            continue;
        }else if(i == BundleCategory::BackgroundSegmentation){
            tempList.append("bg_segment");
        }else if(i == BundleCategory::Avatar){
            QStringList tempList;
            tempList<<"fakeman";
            m_categoryBundles.append(tempList);
            continue;
        }else if(i == BundleCategory::LightMakeup){
            //轻美妆加入桃花,西柚,清透,男友
            QStringList tempList;
            tempList<<"light_makeup_peachblossom"<<"light_makeup_grapefruit"<<"light_makeup_clear"<<"light_makeup_boyfriend";
            m_categoryBundles.append(tempList);
            continue;
        }
        QDir dir(gBundlePath[i]);
        if(!dir.exists())
        {
            qDebug()<<gBundlePath[i]<<"目录路径错误,请在Config.h中修改g_assetDir路径"<<endl;
            return ;
        }
        //文件过滤器
        QStringList filter;
        filter<<"*.bundle";
        dir.setNameFilters(filter);//设置文件名的过滤
        QFileInfoList list = dir.entryInfoList();
        if(list.length()!=0){
            for (int j = 0; j < list.size(); j++)
            {
                tempList.append(list.at(j).fileName().split(".").at(0));
            }
            m_categoryBundles.append(tempList);
        }
        else
        {
            qDebug()<<gBundlePath[i]<<"no bundle file";
        }
    }
}

void UIBridge::updateCategory(QList<QVariant> &lv, int place, int index, QString value)
{
    QStringList tempList = lv[place].toStringList();
    tempList.replace(index, value);
    lv.replace(place, tempList);
}

void UIBridge::readCustomMakeup()
{
    //从配置文件读取彩妆
    //读取完m_customMakeup结构为
    //第一组 0 {"腮红", "makeup_blusher_color", "makeup_intensity_blusher", "100", "-1", "-1"}
    //      1 {"苹果肌", "-1", "demo_style_blush_01.png","mu_style_blush_01","#F4706B","#ED596D","#E24247","#E24266","#FF6351"}
    //      2 {"扇形", "-1", "demo_style_blush_02.png","mu_style_blush_02","#F4706B","#ED596D","#E24247","#E24266","#FF6351"}
    //数据第一个显示文字, 二非-1表示有意义下标(眉毛变形类型brow_warp_type,lip_type口红类型 用到),三显示图片,四使用的模型,之后为颜色组,其中眉毛有双色,三色情况
    //这里如果需要exe直接运行需要改绝对路径
    QFile file(":/assets/items/Makeup/subs_setup.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<tr("subs_setup.json路径错误");
    }
    QString value = file.readAll();
    file.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug()<<tr("解析subs_setup.json文件错误！");
        return;
    }
    QJsonObject jsonObject = document.object();
    int index = 0;
    for(auto it = jsonObject.begin(); it != jsonObject.end(); it++)
    {
        QList<QVariant> tempList;
        tempList.append(m_tempCustomMakeup.at(index));
        index++;
        QJsonArray array = it->toArray();
        QJsonObject mainObj = array.at(0).toObject();
        if(mainObj.contains("types")){
            QString bundle = mainObj["bundle"].toString();
            QJsonArray types = mainObj["types"].toArray();
            for(int i = 0; i < types.size(); i++){
                QStringList tempStr;
                QJsonObject type = types.at(i).toObject();
                QString typeIndex = "-1";
                if(type.contains("lip_type")){
                    typeIndex = QString::number(type["lip_type"].toInt());
                }
                tempStr<<type["name"].toString()<<typeIndex<<type["icon"].toString()<<bundle;
                QJsonArray colors = mainObj["colors"].toArray();
                for(int j = 0; j < colors.size(); j++){
                    QJsonArray color = colors.at(j).toArray();
                    QString hexColor;
                    hexColor += "#" + QString("%1").arg(int(color.at( 0).toDouble() * 255),2,16,QChar('0')) +
                            QString("%1").arg(int(color.at(1).toDouble() * 255),2,16,QChar('0')) +
                            QString("%1").arg(int(color.at(2).toDouble() * 255),2,16,QChar('0'));
                    //第四个透明度都为1
                    //+QString("%1").arg(int(color.at(3).toDouble() * 255),2,16,QChar('0'));
                    tempStr.append(hexColor);
                }
                tempList.append(tempStr);
            }
        }else{
            for(int i = 0; i < array.size(); i++)
            {
                QStringList tempStr;
                QJsonObject item = array.at(i).toObject();
                QString type = "0";
                if(item.contains("type")){
                    type = QString::number(item["type"].toInt());
                }
                tempStr<<item["name"].toString()<<type<<item["icon"].toString()<<item["bundle"].toString();
                QJsonArray colorArray = item.value("colors").toArray();
                for(int j = 0; j < colorArray.size(); j++){
                    QJsonArray color = colorArray.at(j).toArray();
                    QString hexColor;
                    //这里考虑双色眼影,三色情况
                    for(int k = 0; k < color.size(); k += 4){
                        hexColor += "#" + QString("%1").arg(int(color.at(k + 0).toDouble() * 255),2,16,QChar('0')) +
                                QString("%1").arg(int(color.at(k + 1).toDouble() * 255),2,16,QChar('0')) +
                                QString("%1").arg(int(color.at(k + 2).toDouble() * 255),2,16,QChar('0'));
                        //+QString("%1").arg(int(color.at(k + 3).toDouble() * 255),2,16,QChar('0'));
                    }
                    tempStr.append(hexColor);
                }
                tempList.append(tempStr);
            }
        }
        m_customMakeup.append(tempList);
    }
}

void UIBridge::readStyleRecommendation()
{
    QString path = QString::fromStdString(g_assetDir) + "items/StyleRecommendation/style_setup.json";
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<tr("style_setup.json路径错误");
    }
    QString value = file.readAll();
    file.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug()<<tr("解析style_setup.json文件错误！");
        return;
    }
    QJsonObject jsonObject = document.object();
    for(auto it = jsonObject.begin(); it != jsonObject.end(); it++)
    {
        m_styleRecommendationParam.mNameList.append(it.key());
        QStringList tempList;
        QJsonArray array = it->toArray();
        for(int j = 0; j < array.size(); j++){
            QJsonObject tempobj = array.at(j).toObject();
            if(tempobj.contains("BeautyLevelDefault")){
                QJsonArray temparray = tempobj["BeautyLevelDefault"].toArray();
                QStringList tempList;
                for(int k = 0; k < temparray.size(); k++){
                    tempList.append(QString::number(temparray.at(k).toInt()));
                }
                m_styleRecommendationParam.mBeautySkinDefault.append(tempList);
            }
            if(tempobj.contains("ShapeLevelDefault")){
                QJsonArray temparray = tempobj["ShapeLevelDefault"].toArray();
                QStringList tempList;
                for(int k = 0; k < temparray.size(); k++){
                    tempList.append(QString::number(temparray.at(k).toInt()));
                }
                m_styleRecommendationParam.mBeautyFaceDefault.append(tempList);
            }
            if(tempobj.contains("BeautyLevel")){
                QJsonArray temparray = tempobj["BeautyLevel"].toArray();
                QStringList tempList;
                for(int k = 0; k < temparray.size(); k++){
                    tempList.append(QString::number(temparray.at(k).toInt()));
                }
                m_styleRecommendationParam.mBeautySkin.append(tempList);
            }
            if(tempobj.contains("ShapeLevel")){
                QJsonArray temparray = tempobj["ShapeLevel"].toArray();
                QStringList tempList;
                for(int k = 0; k < temparray.size(); k++){
                    tempList.append(QString::number(temparray.at(k).toInt()));
                }
                m_styleRecommendationParam.mBeautyFace.append(tempList);
            }
            if(tempobj.contains("FilterLevel")){
                m_styleRecommendationParam.mFilterLevel.append(tempobj["FilterLevel"].toInt());
            }
            if(tempobj.contains("MakeUpIntensity")){
                m_styleRecommendationParam.mMakeUpIntensity.append(tempobj["MakeUpIntensity"].toInt());
            }
        }
    }
}

void UIBridge::saveStyleRecommendation()
{
    if(!m_bSaveStyleRecommendation){
        return;
    }
    QString path = QString::fromStdString(g_assetDir) + "items/StyleRecommendation/style_setup.json";
    QFile file(path);
    QJsonObject jsonObject;
    for(int i = 0; i < m_styleRecommendationParam.mNameList.size(); i++){
        QJsonArray array;
        QJsonObject object;
        QJsonArray temparray1,temparray2,temparray3,temparray4;
        QStringList templist1 = m_styleRecommendationParam.mBeautySkinDefault.at(i).toStringList();
        QStringList templist2 = m_styleRecommendationParam.mBeautyFaceDefault.at(i).toStringList();
        QStringList templist3 = m_styleRecommendationParam.mBeautySkin.at(i).toStringList();
        QStringList templist4 = m_styleRecommendationParam.mBeautyFace.at(i).toStringList();
        for(int j = 0; j < templist1.size(); j++){
            temparray1.append(templist1.at(j).toInt());
        }
        for(int j = 0; j < templist2.size(); j++){
            temparray2.append(templist2.at(j).toInt());
        }
        for(int j = 0; j < templist3.size(); j++){
            temparray3.append(templist3.at(j).toInt());
        }
        for(int j = 0; j < templist4.size(); j++){
            temparray4.append(templist4.at(j).toInt());
        }
        object["BeautyLevelDefault"] = temparray1;
        object["ShapeLevelDefault"] = temparray2;
        object["BeautyLevel"] = temparray3;
        object["ShapeLevel"] = temparray4;
        object["FilterLevel"] = m_styleRecommendationParam.mFilterLevel.at(i);
        object["MakeUpIntensity"] = m_styleRecommendationParam.mMakeUpIntensity.at(i);
        array.append(object);
        jsonObject[m_styleRecommendationParam.mNameList.at(i)] = array;
    }
    QJsonDocument saveDoc(jsonObject);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(saveDoc.toJson());
    file.close();
}

void UIBridge::creatdefaultFrame()
{
    string defaultPicPath = "../res/frame.png";
    static cv::Mat defaulfFrame = cv::imread(defaultPicPath, cv::IMREAD_COLOR);
    cv::cvtColor(defaulfFrame, MainClass::getInstance()->m_nama->m_frame, cv::COLOR_RGB2RGBA);
}

void UIBridge::initBodyTrackConfig()
{
    m_bodyTrackConfig.FakeManHalf.Pos_y = 0;
    m_bodyTrackConfig.FakeManHalf.Pos_z = -300;
    m_bodyTrackConfig.BearFull.AnimFilterParams_n_buffer_frames = 10;
    m_bodyTrackConfig.BearFull.Pos_x = 70;
    m_bodyTrackConfig.BearFull.Pos_y = 50;
    m_bodyTrackConfig.BearFull.Pos_z = -1100;
    m_bodyTrackConfig.BearFull.TrackMoveRange_x = 0.9f;
    m_bodyTrackConfig.BearFull.TrackMoveRange_y = 0.9f;
    m_bodyTrackConfig.BearFull.TrackMoveRange_z = 0.1f;
    m_bodyTrackConfig.BearHalf.AnimFilterParams_n_buffer_frames = 10;
    m_bodyTrackConfig.BearHalf.Pos_y = 60;
    m_bodyTrackConfig.BearHalf.Pos_z = -300;
}

void UIBridge::getGSPresentFrame(const cv::Mat &frame)
{
    UIBridge *m_UIBridge = MainClass::getInstance()->m_UIBridge;
    if(m_UIBridge->m_bgsSelectVideo){
        m_UIBridge->m_bgsSelectVideo = false;
        double width = frame.cols;
        double height = frame.rows;
        m_frameSize = QString::number(int(width)) + "x" + QString::number(int(height));
        frameSizeChanged();
        double widthBgs = width/height * 9/16 * 0.5;
        if(widthBgs >= 0.5){
            double heightBgs = height/width * 16/9 * 0.5;
            double starty = 1 - heightBgs;
            m_UIBridge->m_gsStart = QPointF(0.5,starty);
            m_UIBridge->m_gsSize = QPointF(0.5,heightBgs);
        }else{
            double startx = 1 - widthBgs;
            m_UIBridge->m_gsStart = QPointF(startx,0.5);
            m_UIBridge->m_gsSize = QPointF(widthBgs,0.5);
        }
        MainClass::getInstance()->m_nama->changeGSPreviewRect(m_gsStart.x(), m_gsStart.y(), m_gsStart.x() + m_gsSize.x(), m_gsStart.y() + m_gsSize.y());
    }
    if(frame.channels() == 4){
        cv::cvtColor(frame, frame, cv::COLOR_RGBA2BGRA);
        if(m_bodyTrackType != BodyTrackType::None || m_bGSCameraImage){
            if(m_CameraType){
                m_ImageProvider->m_showImage = QImage(frame.data, frame.cols, frame.rows, frame.cols * 4, QImage::Format_ARGB32);
            }else{
                m_ImageProvider->m_showImage = QImage(frame.data, frame.cols, frame.rows, frame.cols * 4, QImage::Format_ARGB32).mirrored(true, false);
            }
            emit callQmlRefeshImg();
        }
        MainClass::getInstance()->m_nama->getPresentFrame(frame.clone());
    }else{
        cv::Mat resultframe;
        cv::cvtColor(frame, resultframe, cv::COLOR_BGR2BGRA);
        MainClass::getInstance()->m_nama->getPresentFrame(resultframe.clone());
    }
    if(!m_newImage && !m_renderNewFrame && !m_arFunction){
        m_newImage = true;
    }
}

void UIBridge::getPresentFrame(const cv::Mat &frame)
{
    if(!m_gsSelectCamera && !m_arFunction && !m_gsCameraPlay){
        return;
    }
    if((!m_bLoadBear && m_bodyTrackType != BodyTrackType::None) || m_bGSCameraImage){
        if(frame.channels() == 1){
            cv::Mat mat;
            cv::resize(frame,mat,cv::Size(frame.cols/4,frame.rows/4));
            cv::cvtColor(mat, mat, cv::COLOR_YUV2BGRA_IYUV);
            m_ImageProvider->m_showImage = QImage(mat.data, mat.cols, mat.rows, mat.cols * 4, QImage::Format_ARGB32).copy();
        }else{
            m_ImageProvider->m_showImage = QImage(frame.data, frame.cols, frame.rows, frame.cols * 4, QImage::Format_ARGB32);
        }
        if(!m_CameraType){
            m_ImageProvider->m_showImage = m_ImageProvider->m_showImage.mirrored(true, false);
        }
        emit callQmlRefeshImg();
    }
    MainClass::getInstance()->m_nama->getPresentFrame(frame);
    if(!m_newImage && !m_renderNewFrame && m_arFunction){
        m_newImage = true;
    }

}

void UIBridge::updateGreenScreenSegment(const cv::Mat &dataRGBA)
{
    MainClass::getInstance()->m_nama->UpdateGreenScreenSegment(dataRGBA);
}

void UIBridge::updateBackgroundSegment(const cv::Mat &dataRGBA)
{
    //第一次保存图标图片,截取图片中心圆
    if(m_bNeedSavebg_seg){
        m_bNeedSavebg_seg = false;
        cv::Mat saveMat;
        cv::cvtColor(dataRGBA,saveMat,cv::COLOR_RGBA2BGRA);
        QImage image((const uchar*)saveMat.data, saveMat.cols, saveMat.rows, saveMat.step, QImage::Format_ARGB32);
        image = image.scaled(128, 72, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QSize size(128, 72);
        QBitmap mask(size);
        QPainter painter(&mask);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
        painter.setBrush(QColor(0, 0, 0));
        painter.drawRoundedRect(28,0,72,72,100,100);
        QPixmap imagepix = QPixmap::fromImage(image);
        imagepix.setMask(mask);
        imagepix = imagepix.copy(28,0,72,72);
        imagepix.save("bg_seg_shot.png");
        QStringList tempList = m_categoryBundles.at(BundleCategory::BackgroundSegmentation).toStringList();
        if(!tempList.contains("bg_seg_shot")){
            tempList.insert(1, "bg_seg_shot");
            m_categoryBundles.replace(BundleCategory::BackgroundSegmentation, tempList);
        }
        emit updateBsgPic();
    }
    MainClass::getInstance()->m_nama->UpdateBackgroundSegment(dataRGBA.clone());
}

void UIBridge::setARFunction(bool ar){
    MainClass *main = MainClass::getInstance();
    Nama *nama = main->m_nama;
    m_arFunction = ar;
    //绿幕打开绿幕选中摄像头
    if(!m_arFunction){
        m_gsCameraPlay = false;
        if(nama->m_bundleCategory == BundleCategory::MusicFilter && nama->m_mp3 != nullptr)
        {
            nama->m_mp3->Pause();
        }
        m_selectCategory = BundleCategory::GreenScreen;
        //默认打开背景视频
        if(m_gsSelectCamera){
            fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
            fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
            main->m_camera->m_QCamera->start();
            m_gsVideoMediaPlayer.stop();
        }else{
            m_bgsVideoMediaPlayer.stop();
            gsSelectVideo(m_gsSelectVideoPath);
        }
        //设置绿幕颜色背景绿色
        nama->SetGSKeyColor(m_gsColor);
        //更新绿幕参数
        for(int i = 0 ; i < m_greenScreen.at(0).toStringList().size(); i++){
            namaFuItemSetParamd(nama->m_GSHandle, m_greenScreen, i, false);
        }
        if(m_gsSelectSafeArea >= 0){
            QString fullpath = QString::fromStdString(g_assetDir) + "items/GreenScreenBg/" + m_greenScreenIcon[0].toStringList()[m_gsSelectBgIndex] + ".mp4";
            gsPlayBGVideo(fullpath);
            m_selectCategory = BundleCategory::SafeArea;
            useProps(m_gsSelectSafeArea);
        }else{
            useProps(m_gsSelectBgIndex);
        }
        nama->changeRenderList(RENDER_GREEN);
    }else{
        m_gsMediaPlayer.stop();
        m_gsVideoMediaPlayer.stop();
        main->m_camera->m_QCamera->start();
        fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
        fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
        //停止绿幕背景,视频播放器
        if(m_flagARBody){
            nama->changeRenderList(RENDER_BODY);
            updateFilter();
        }else{
            nama->changeRenderList(RENDER_AR);
            if(nama->m_bundleCategory == BundleCategory::MusicFilter && nama->m_mp3 != nullptr)
            {
                nama->m_mp3->Play();
            }
        }
        m_frameSize = QString::number(MainClass::getInstance()->m_camera->m_FrameWidth) + "x" +
                QString::number(MainClass::getInstance()->m_camera->m_FrameHeight);
        emit frameSizeChanged();
        //重新设置轻美妆滤镜
        if(m_selectCategory == BundleCategory::LightMakeup && !m_flagARBody){
            if(m_selectCategoryIndex == 0){
                nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "fennen3");
                nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 1.0);
            }else if(m_selectCategoryIndex == 1){
                nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "lengsediao4");
                nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 0.7);
            }else if(m_selectCategoryIndex == 2){
                nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "xiaoqingxin1");
                nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 0.8);
            }else if(m_selectCategoryIndex == 3){
                nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "xiaoqingxin3");
                nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 0.9);
            }
        }
        //如果是自定义背景分割重新播放视频
        if(m_selectCategory == BundleCategory::BackgroundSegmentation){
            if(m_categoryBundles.at(BundleCategory::BackgroundSegmentation).toStringList().length() == 10 && (m_selectCategoryIndex == 1 || m_selectCategoryIndex == 0)){
                m_bgsVideoMediaPlayer.play();
            }
        }
        fuSetMaxFaces(4);
        //重新设置最大人脸数
        if(m_selectCategory == BundleCategory::Animoji || m_selectCategory == BundleCategory::BigHead){
            fuSetMaxFaces(1);
        }else if(m_selectCategory == BundleCategory::BackgroundSegmentation){
            //boyfrind的3个单人脸
            int length = m_categoryBundles.at(BundleCategory::BackgroundSegmentation).toStringList().length();
            if(length == 9 && m_selectCategoryIndex < 4){
                fuSetMaxFaces(1);
            }else if(m_selectCategoryIndex < 5 && m_selectCategoryIndex > 1){
                fuSetMaxFaces(1);
            }
        }else if(m_selectCategory == BundleCategory::ItemJingpin){
            if (MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][m_selectCategoryIndex]->IsOnlyOnePeople())
            {
                fuSetMaxFaces(1);
            }
        }
    }
}

void UIBridge::setSelectCategory(int index)
{
    m_selectCategory = index;
    MainClass::getInstance()->m_nama->m_bundleCategory = BundleCategory(m_selectCategory);
    if(m_selectCategory == BundleCategory::SafeArea){
        m_tip = "白色区域为安全区域,不参与绿幕抠像";
        tipChanged();
    }
}

void UIBridge::setBodyTrackType(int type)
{
    Nama *nama = MainClass::getInstance()->m_nama;
    m_bodyTrackType = BodyTrackType(type);
    if(m_bodyTrackType != BodyTrackType::None){
        nama->DestroyAll();
        if(nama->m_mp3 != nullptr)
        {
            nama->m_mp3->Pause();
        }
        if(m_bLoadBear){
            MainClass::getInstance()->m_nama->UnLoadAvatar();
            m_bLoadAvatar = false;
            m_bLoadBear = false;
        }
        if(!m_bLoadAvatar){
            std::vector<std::string> mBundleDirs;
            mBundleDirs.emplace_back(g_assetDir + "Avatars/fakeman.bundle");
            mBundleDirs.emplace_back(g_assetDir + "Avatars/default_bg.bundle");
            nama->LoadAvatarBundles(mBundleDirs);
            nama->LoadAvatarHandTrackBundle();
            m_bLoadAvatar = true;
        }
        nama->SetBodyTrackType(m_bodyTrackType);
        if(m_bodyTrackType == BodyTrackType::HalfBody){
            nama->ApplyBodyTrackConfig(m_bodyTrackConfig.FakeManHalf);
        }else{
            nama->ApplyBodyTrackConfig(m_bodyTrackConfig.FakeManFull);
        }
        nama->m_Controller->EnableHumanFollowMode(false);
    }
}

bool UIBridge::virturalCamera(){
    return MainClass::getInstance()->m_nama->m_bVirturalCamera;
}

void UIBridge::setVirturalCamera(bool vc){
    MainClass::getInstance()->m_nama->m_bVirturalCamera = vc;
}

void UIBridge::setGSCameraImage(bool flag){
    m_bGSCameraImage = flag;
}

void UIBridge::setGSCameraPlay(bool flag)
{
    if(!flag && !m_gsSelectCamera){
        m_gsCameraPlay = false;
        MainClass::getInstance()->m_camera->m_QCamera->stop();
    }else{
        m_gsCameraPlay = true;
        MainClass::getInstance()->m_camera->m_QCamera->start();
    }
}

void UIBridge::unLoadAvatar()
{
    if(m_bLoadAvatar){
        MainClass::getInstance()->m_nama->UnLoadAvatar();
        m_bodyTrackType = BodyTrackType::None;
        updateBodyTrackType(-1);
        m_bLoadAvatar = false;
        m_bLoadBear = false;
    }
}

void UIBridge::readUserConfig()
{
    //读取自定义背景分割配置
    QFile *file = new QFile(QDir::currentPath()+"/CustomUserConfig.json");
    if(file->open(QFile::ReadOnly))
    {
        QString value = file->readAll();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug()<<tr("解析json文件错误！");
        }else{
            QJsonObject jsonObject = document.object();
            if(jsonObject.contains("BgSegPath")){
                m_begUserFilePath = jsonObject["BgSegPath"].toString();
                QFile testFile(m_begUserFilePath);
                if(testFile.exists()){
                    //将背景分割里添加自定义背景分割图标。
                    QStringList tempList = m_categoryBundles.at(BundleCategory::BackgroundSegmentation).toStringList();
                    tempList.insert(1, "bg_seg_shot");
                    m_categoryBundles.replace(BundleCategory::BackgroundSegmentation, tempList);
                }else{
                    m_begUserFilePath = "";
                }
            }
            if(jsonObject.contains("GsSafePath")){
                m_gsSafeUserFilePath = jsonObject["GsSafePath"].toString();
                QFile testFile(m_gsSafeUserFilePath);
                if(testFile.exists()){
                    QStringList tempList = m_greenScreenIcon.at(1).toStringList();
                    if(!tempList.contains("gs_savearea_shot")){
                        tempList.insert(1, "gs_savearea_shot");
                        m_greenScreenIcon.replace(1, tempList);
                    }
                }else{
                    m_gsSafeUserFilePath = "";
                }
            }
            if(jsonObject.contains("CameraName")){
                m_cameraName = jsonObject["CameraName"].toString();
            }
            if(jsonObject.contains("CameraSet")){
                m_cameraSet = jsonObject["CameraSet"].toInt();
            }
            if(jsonObject.contains("BeautySkin")){
                m_beautySkin.replace(3, jsonObject["BeautySkin"].toString().split(","));
            }
            if(jsonObject.contains("BeautyFace")){
                m_beautyFace.replace(3, jsonObject["BeautyFace"].toString().split(","));
            }
            if(jsonObject.contains("BeautyBody")){
                m_beautyBody.replace(3, jsonObject["BeautyBody"].toString().split(","));
            }
            if(jsonObject.contains("GreenScreen")){
                m_greenScreen.replace(3, jsonObject["GreenScreen"].toString().split(","));
            }
            if(jsonObject.contains("FilterIndex")){
                m_filterIndex = jsonObject["FilterIndex"].toInt();
            }
            if(jsonObject.contains("Filter")){
                m_filter.replace(2, jsonObject["Filter"].toString().split(","));
            }
            if(jsonObject.contains("SelectCategory")){
                m_selectCategory = jsonObject["SelectCategory"].toInt();
            }
            if(jsonObject.contains("SelectCategoryIndex")){
                m_selectCategoryIndex = jsonObject["SelectCategoryIndex"].toInt();
            }
            if(jsonObject.contains("BodyTrackType")){
                m_bodyTrackType = BodyTrackType(jsonObject["BodyTrackType"].toInt());
            }
            if(jsonObject.contains("StickerIndex")){
                m_stickerIndex = (jsonObject["StickerIndex"].toInt());
            }
            if(jsonObject.contains("GSPreviewRect")){
                QStringList RectList = jsonObject["GSPreviewRect"].toString().split(",");
                m_gsStart.setX(RectList.at(0).toDouble());
                m_gsStart.setY(RectList.at(1).toDouble());
                m_gsSize.setX(RectList.at(2).toDouble());
                m_gsSize.setY(RectList.at(3).toDouble());
            }
            if(jsonObject.contains("GSSelectBgIndex")){
                m_gsSelectBgIndex = jsonObject["GSSelectBgIndex"].toInt();
            }
            if(jsonObject.contains("GSSelectSafeArea")){
                m_gsSelectSafeArea = jsonObject["GSSelectSafeArea"].toInt();
            }
            if(jsonObject.contains("GSSelectCamera")){
                m_gsSelectCamera = jsonObject["GSSelectCamera"].toBool();
            }
            if(jsonObject.contains("GSSelectVideoPath")){
                m_gsSelectVideoPath = jsonObject["GSSelectVideoPath"].toString();
            }
            if(jsonObject.contains("GSSelectColor")){
                QStringList colorList = jsonObject["GSSelectColor"].toString().split(",");
                m_gsColor = {uchar(colorList.at(0).toInt()), uchar(colorList.at(1).toInt()), uchar(colorList.at(2).toInt()), uchar(colorList.at(3).toInt())};
            }
            if(jsonObject.contains("CustomMakeup")){
                m_userCustomMakeup = jsonObject["CustomMakeup"].toString().split(",");
            }
            if(jsonObject.contains("GSKeyColors")){
                QStringList strList;
                QJsonArray gsKey = jsonObject["GSKeyColors"].toArray();
                for(int i = 0 ;i < gsKey.size(); i++){
                    QJsonArray tempKey = gsKey.at(i).toArray();
                    QVector3D keycolor(tempKey.at(0).toInt(),tempKey.at(1).toInt(),tempKey.at(2).toInt());
                    m_gsKeyColorList.append(keycolor);
                    QString color = "#" + QString("%1").arg(int(keycolor.x()),2,16,QChar('0')) +
                            QString("%1").arg(int(keycolor.y()),2,16,QChar('0')) +
                            QString("%1").arg(int(keycolor.z()),2,16,QChar('0'));
                    color = color.toUpper();
                    strList.append(color);
                }
                m_gsKeyColor = strList;
            }
        }
        file->close();
    }
}

void UIBridge::getCameraList()
{
    Camera *camera = MainClass::getInstance()->m_camera;
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    int cameraIndex = 0;
    if(availableCameras.size() > 0){
        camera->m_cameraList.clear();
        for(int i = 0; i < availableCameras.size(); i++){
            QString cameraName = availableCameras.at(i).description();
            camera->m_cameraList<<cameraName;
            if(cameraName.compare(m_cameraName) == 0){
                camera->m_mapCameraSet.insert(cameraName, m_cameraSet);
                m_cameraName = "";
                cameraIndex = i;
            }else{
                camera->m_mapCameraSet.insert(cameraName, 0);
            }
            cameraListChanged();
        }
    }
    if(cameraIndex != 0){
        emit updateCameraIndex(cameraIndex);
    }
    if(m_cameraSet != 0){
        emit updateCameraSet(m_cameraSet);
    }else{
        for(int i = 0; i < camera->m_cameraSetList.length(); i++){
            QString set = camera->m_cameraSetList.at(i);
            int width = set.mid(0,set.indexOf("x")).toInt();
            int height = set.mid(set.indexOf("x") + 1,set.indexOf(" ") - (set.indexOf("x") + 1)).toInt();
            int fps = set.mid(set.indexOf(" ") + 1,set.indexOf("fps") - (set.indexOf(" ") + 1)).toInt();
            //默认切换1280*720 20fps
            if(width == 1280 && height == 720 && fps == 20){
                emit updateCameraSet(i);
            }
        }
    }
}

void UIBridge::saveUserConfig(bool flag)
{
    //保存摄像头选择,和参数
    //保存美颜美妆美型参数
    //保存使用道具
    Camera *camera = MainClass::getInstance()->m_camera;
    QFile file(QDir::currentPath()+"/CustomUserConfig.json");
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError) && value.compare("") != 0)
        {
            qDebug()<<tr("解析json文件错误！");
        }
        QJsonObject jsonObject = document.object();
        QString cameraName = camera->m_arCamera.description();
        if(flag){
            //保存摄像头名称
            jsonObject["CameraName"] = cameraName;
            //保存摄像头参数
            jsonObject["CameraSet"] = camera->m_mapCameraSet[cameraName];
            //保存参数值
            jsonObject["BeautySkin"] = m_beautySkin.at(3).toStringList().join(",");
            jsonObject["BeautyFace"] = m_beautyFace.at(3).toStringList().join(",");
            jsonObject["BeautyBody"] = m_beautyBody.at(3).toStringList().join(",");
            jsonObject["GreenScreen"] = m_greenScreen.at(3).toStringList().join(",");
            jsonObject["FilterIndex"] = m_filterIndex;
            jsonObject["Filter"] = m_filter.at(2).toStringList().join(",");
            jsonObject["SelectCategory"] = m_selectCategory;
            jsonObject["SelectCategoryIndex"] = m_selectCategoryIndex;
            jsonObject["BodyTrackType"] = int(m_bodyTrackType);
            //精品贴纸
            jsonObject["StickerIndex"] = int(m_stickerIndex);
            //绿幕
            jsonObject["GSSelectSafeArea"] = m_gsSelectSafeArea;
            jsonObject["GSSelectBgIndex"] = m_gsSelectBgIndex;
            //绿幕选择视频还是摄像头,视频存路径
            jsonObject["GSSelectCamera"] = m_gsSelectCamera;
            jsonObject["GSSelectVideoPath"] = m_gsSelectVideoPath;
            QStringList gsRect;
            gsRect<<QString::number(m_gsStart.x())<<QString::number(m_gsStart.y())<<QString::number(m_gsSize.x())<<QString::number(m_gsSize.y());
            jsonObject["GSPreviewRect"] = gsRect.join(",");
            QStringList gsColor;
            gsColor<<QString::number(m_gsColor[0])<<QString::number(m_gsColor[1])<<QString::number(m_gsColor[2])<<QString::number(m_gsColor[3]);
            jsonObject["GSSelectColor"] = gsColor.join(",");
            //存自定义美妆配置
            QStringList customMakeup;
            if(m_selectCategory == BundleCategory::Makeup && m_selectCategoryIndex == 0){
                for(int i = 0; i < m_customMakeup.size(); i++){
                    QStringList tempList= m_customMakeup.at(i).at(0).toStringList();
                    customMakeup.append(tempList.at(4));
                    customMakeup.append(tempList.at(5));
                }
            }
            jsonObject["CustomMakeup"] = customMakeup.join(",");
        }
        if(m_begUserFilePath.compare("") != 0){
            jsonObject["BgSegPath"] = m_begUserFilePath;
        }
        if(m_gsSafeUserFilePath.compare("") != 0){
            jsonObject["GsSafePath"] = m_gsSafeUserFilePath;
        }
        QJsonArray gsKey;
        for(int i = 0; i < m_gsKeyColorList.size(); i++){
            QJsonArray tempKey;
            QVector3D keycolor = m_gsKeyColorList.at(i);
            tempKey.append(keycolor.x());
            tempKey.append(keycolor.y());
            tempKey.append(keycolor.z());
            gsKey.append(tempKey);
        }
        jsonObject["GSKeyColors"] = gsKey;
        QJsonDocument saveDoc(jsonObject);
        file.open(QFile::WriteOnly);
        file.write(saveDoc.toJson());
        file.close();
    }
}

void UIBridge::updateUserConfig()
{
    Nama *nama = MainClass::getInstance()->m_nama;
    //加载美颜,美肤等参数
    reloadItemParam();
    QString color = "#" + QString("%1").arg(m_gsColor[0],2,16,QChar('0')) +
            QString("%1").arg(m_gsColor[1],2,16,QChar('0')) +
            QString("%1").arg(m_gsColor[2],2,16,QChar('0'));
    color = color.toUpper();
    emit selectColorChanged(color);
    nama->m_bundleCategory = BundleCategory(m_selectCategory);
    //加载使用道具
    if(m_selectCategory >= BundleCategory::GreenScreen){
        if(!m_gsSelectCamera){
            gsSelectVideo(m_gsSelectVideoPath);
        }
        setARFunction(false);
        if(m_gsSelectSafeArea >= 0){
            m_selectCategory = BundleCategory::SafeArea;
            useProps(m_gsSelectSafeArea);
        }
        updateGSSelectIndex(m_gsSelectBgIndex, m_gsSelectSafeArea);
    }else{
        if(m_selectCategory >= 0 && m_selectCategoryIndex >= 0 ){
            if(m_selectCategory == BundleCategory::Avatar && m_selectCategoryIndex == 0){
                updateBodyTrackType(int(m_bodyTrackType));
            }else if(m_selectCategory == BundleCategory::ItemJingpin){
                downloadSticker(m_selectCategoryIndex);
            }else{
                useProps(m_selectCategoryIndex);
            }
            updateSelectCategory(m_selectCategory,m_selectCategoryIndex,m_stickerIndex);
        }
    }
    if(m_selectCategory == BundleCategory::Makeup && m_selectCategoryIndex == 0){
        if(m_userCustomMakeup.size() > 0){
            for(int i = 0; i < m_customMakeup.size(); i++){
                int index = m_userCustomMakeup.at(2*i).toInt();
                if(index >= 0){
                    setCustomMakeupIndex(i, m_userCustomMakeup.at(2*i));
                }
                int colorIndex = m_userCustomMakeup.at(2*i + 1).toInt();
                if(colorIndex >= 0){
                    setCustomMakeupColor(i, m_userCustomMakeup.at(2*i + 1));
                }
            }
            nama->changeRenderList(RENDER_AR,true);
        }
    }
    //更新鉴权
    QStringList strList;
    for(int i = 0; i < BundleCategory::Count; i++){
        if(checkModuleCode(i)){
            strList.append("true");
        }else{
            strList.append("false");
        }
    }
    m_checkMode = strList;
    emit updateCheckModule();
}

void UIBridge::updateGSSafeArea(QImage &image)
{
    cv::Mat mat;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        return;
    }
    cv::resize(mat,m_matSafeArea,cv::Size(1280, 720));
    cv::cvtColor(m_matSafeArea, m_matSafeArea, cv::COLOR_BGR2RGBA);
    //    b_needCreateTex = true;
    MainClass::getInstance()->m_nama->UpdateGreenScreenSafeArea(m_matSafeArea);
}

void UIBridge::useProps(int index)
{
    Nama *nama = MainClass::getInstance()->m_nama;
    std::unique_lock<std::mutex> lock(nama->m_frameMutex);
    m_bgsVideoMediaPlayer.stop();
    if(m_selectCategory != int(BundleCategory::GreenScreen) && m_selectCategory != int(BundleCategory::SafeArea)){
        if((m_selectCategoryLast == int(BundleCategory::ItemJingpin) && m_selectCategory != int(BundleCategory::ItemJingpin)) ||
                (m_selectCategoryLast == int(BundleCategory::Makeup) && m_selectCategory != int(BundleCategory::Makeup))){
            nama->ReloadItems();
            reloadItemParam();
        }
    }
    if(m_selectCategory == BundleCategory::StyleRecommendation){
        updateStyleRecommendation(index);
    }else{
        if(m_selectCategoryLast == BundleCategory::StyleRecommendation){
            updateStyleRecommendation();
        }
    }
    if(nama->m_mp3 != nullptr)
    {
        nama->m_mp3->Pause();
    }
    if(m_selectCategory != BundleCategory::Avatar){
        unLoadAvatar();
    }
    if(m_arFunction){
        m_selectCategoryIndex = index;
        QString name = m_categoryBundles[m_selectCategory].toStringList()[m_selectCategoryIndex];
        QString full_path;
        bool b_light_makeup = false;
        if(name.compare("bg_seg_shot") == 0){
            name = "bg_segment";
        }
        if(name.compare("bg_segment") == 0){
            //使用自定义背景分割
            bgsSelectVideo(m_begUserFilePath);
            full_path = "others/bg_segment.bundle";
        }else{
            full_path = gBundlePath[m_selectCategory] + "/" + name + ".bundle";
        }
        if(name.compare("light_makeup_peachblossom") == 0 ||
                name.compare("light_makeup_grapefruit") == 0||
                name.compare("light_makeup_clear") == 0||
                name.compare("light_makeup_boyfriend") == 0){
            //使用轻美妆
            b_light_makeup = true;
            full_path = "items/LightMakeup/light_makeup.bundle";
        }
        //qDebug()<<"使用道具"<<m_selectCategory<<m_selectCategoryIndex<<name;
        if(m_tipMap.find(name) != m_tipMap.end()){
            m_tip = m_tipMap.find(name).value();
            tipChanged();
        }
        m_bmakeupFlag = true;
        //新增冻龄,混血, 国风不需要绑定face_makeup
        if(name == "makeup_icon_combination_1_freezing_age" || name == "makeup_icon_combination_1_guo_feng" || name == "makeup_icon_combination_1_mixed_race" || name == "makeup_icon_combination_1_combination_rabbit"){
            m_bmakeupFlag = false;
        }
        if(name != "demo_icon_customize"){
            nama->SelectBundle(g_assetDir + full_path.toStdString(), 4, m_bmakeupFlag);
        }else{
            //自定义美妆不用加载bundle
            if(m_selectCategoryLast == int(BundleCategory::ItemJingpin) ||
                    m_selectCategoryLast == int(BundleCategory::StyleRecommendation)){
                nama->ReloadItems();
                reloadItemParam();
            }
            nama->DestroyAll();
            nama->LoadMakeup();
            nama->ChangeCleanFlag(false);
        }
        if(b_light_makeup){
            //轻美妆设置参数
            setLightMakeUpParam(name);
            m_lightMakeUpName = name;
        }else{
            updateFilter();
        }
        //风格推荐设置美妆,滤镜强度
        if(m_selectCategory == BundleCategory::StyleRecommendation){
            fuItemSetParamd(nama->m_bundleCurrent, "machine_level", 1.0);
            fuItemSetParamd(nama->m_bundleCurrent, "filter_level", float(m_styleRecommendationParam.mFilterLevel.at(m_styleRecommendationIndex)) / 100);
            fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity", float(m_styleRecommendationParam.mMakeUpIntensity.at(m_styleRecommendationIndex)) / 100);
        }
        if(m_flagARBody){
            nama->changeRenderList(RENDER_BODY);
        }else{
            nama->changeRenderList(RENDER_AR, m_bmakeupFlag);
        }
    }else{
        //绿幕切换背景视频
        if(m_selectCategory == BundleCategory::GreenScreen){
            m_gsSelectBgIndex = index;
            QString fullpath = QString::fromStdString(g_assetDir) + "items/GreenScreenBg/" + m_greenScreenIcon[0].toStringList()[index] + ".mp4";
            gsPlayBGVideo(fullpath);
        }
        //绿幕安全区域
        else{
            m_gsSelectSafeArea = index;
            QString name = m_greenScreenIcon[1].toStringList()[m_gsSelectSafeArea];
            nama->changeRenderList(RENDER_GREEN);
            //使用自定义安全区域
            if(name.compare("gs_savearea_shot") == 0){
                QImage image(m_gsSafeUserFilePath);
                updateGSSafeArea(image);
            }else{
                QString imagepath = gBundlePath[m_selectCategory] + "/" + name + ".jpg";
                cv::Mat image = cv::imread(imagepath.toStdString(), cv::IMREAD_REDUCED_COLOR_4);
                //                cv::resize(image,m_matSafeArea,cv::Size(1280, 720));
                cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);
                //                b_needCreateTex = true;
                MainClass::getInstance()->m_nama->UpdateGreenScreenSafeArea(image);
            }
        }
    }
    m_selectCategoryLast = m_selectCategory;
}

void UIBridge::nonuseProps()
{
    Nama *nama = MainClass::getInstance()->m_nama;
    unLoadAvatar();
    fuSetMaxFaces(4);
    if(m_arFunction){
        m_selectCategoryIndex = -1;
        nama->DestroyAll();
        updateFilter();
        if(nama->m_mp3 != nullptr)
        {
            nama->m_mp3->Pause();
        }
        if(m_flagARBody){
            nama->changeRenderList(RENDER_BODY);
        }else{
            nama->changeRenderList(RENDER_AR);
        }
        updateStyleRecommendation();
    }else{
        updateStyleRecommendation();
        if(m_selectCategory == BundleCategory::SafeArea){
            m_gsSelectSafeArea = -1;
            nama->NonuseGreenScreenSafeArea();
        }else{
            m_gsSelectBgIndex = -1;
            disconnect(m_gsvideoSurface, SIGNAL(presentGSBGFrame(const cv::Mat &)), MainClass::getInstance()->m_UIBridge, SLOT(updateGreenScreenSegment(const cv::Mat &)));
            m_gsMediaPlayer.stop();
            //取消传入透明图
            //cv::Mat defaulfFrame(nama->m_frame.rows, nama->m_frame.cols,CV_8UC4,cv::Scalar(0));
            string defaultPicPath = "../res/gsbackground.png";
            static cv::Mat defaulfFrame = cv::imread(defaultPicPath, cv::IMREAD_UNCHANGED);
            nama->UpdateGreenScreenSegment(defaulfFrame);
        }
    }
}

void UIBridge::updateItemParam(int item, int index, QString value)
{
    Nama *nama = MainClass::getInstance()->m_nama;
    ItemParam itemParam = ItemParam(item);
    switch (itemParam) {
    case BeautySkin:
        updateCategory(m_beautySkin, 3, index, value);
        //前两个是按钮
        if(index <= 1){
            namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, index, true);
        }else{
            namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, index, false);
        }
        m_styleRecommendationParam.mBeautySkin.replace(m_styleRecommendationIndex, m_beautySkin.at(3));
        saveStyleRecommendation();
        break;
    case BeautyFace:
        updateCategory(m_beautyFace, 3, index, value);
        namaFuItemSetParamd(nama->m_BeautyHandles, m_beautyFace, index, false);
        m_styleRecommendationParam.mBeautyFace.replace(m_styleRecommendationIndex, m_beautyFace.at(3));
        saveStyleRecommendation();
        break;
    case BeautyBody:
        //显示AR功能跟美体模块无法共用
        if(!m_flagARBody && value.toInt() > 0){
            m_flagARBody = true;
            nama->changeRenderList(RENDER_BODY);
            updateFilter();
            m_tip = "AR功能跟美体模块无法共用";
            tipChanged();
            arBodyFlagChanged(m_flagARBody);
        }
        updateCategory(m_beautyBody, 3, index, value);
        checkBodyParam();
        namaFuItemSetParamd(nama->m_BodyShapeHandle, m_beautyBody, index, false);
        break;
    case ItemGreenScreen:
        updateCategory(m_greenScreen, 3, index, value);
        namaFuItemSetParamd(nama->m_GSHandle, m_greenScreen, index, false);
        break;
    }
}

void UIBridge::resetItemParam(int item)
{
    Nama *nama = MainClass::getInstance()->m_nama;
    ItemParam itemParam = ItemParam(item);
    switch (itemParam) {
    case BeautySkin:
        m_beautySkin.replace(3, m_styleRecommendationParam.mBeautySkinDefault.at(m_styleRecommendationIndex));
        for(int i = 0 ; i < m_beautySkin.at(0).toStringList().size(); i++){
            if(i <= 1){
                namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, i, true);
            }else{
                namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, i, false);
            }
        }
        emit updateBeautySkinParam();
        break;
    case BeautyFace:
        m_beautyFace.replace(3, m_styleRecommendationParam.mBeautyFaceDefault.at(m_styleRecommendationIndex));
        for(int i = 0 ; i < m_beautyFace.at(0).toStringList().size(); i++){
            namaFuItemSetParamd(nama->m_BeautyHandles, m_beautyFace, i, false);
        }
        emit updateBeautyFaceParam();
        break;
    case BeautyBody:
        m_flagARBody = false;
        MainClass::getInstance()->m_nama->changeRenderList(RENDER_AR);
        m_beautyBody.replace(3, m_defaultBeautyBody);
        for(int i = 0 ; i < m_beautyBody.at(0).toStringList().size(); i++){
            namaFuItemSetParamd(nama->m_BodyShapeHandle, m_beautyBody, i, false);
        }
        break;
    case ItemGreenScreen:
        m_greenScreen.replace(3, m_defaultGreenScreen);
        for(int i = 0 ; i < m_greenScreen.at(0).toStringList().size(); i++){
            namaFuItemSetParamd(nama->m_GSHandle, m_greenScreen, i, false);
        }
        emit updateGreenScreenParam();
        break;
    }
}

void UIBridge::setFilter(QString value)
{
    updateCategory(m_filter, 2, m_filterIndex, value);
    Nama *nama = MainClass::getInstance()->m_nama;
    nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", value.toDouble() / 100);
}

void UIBridge::resetFilterIndex(int index) {
    m_filterIndex = index;
    Nama *nama = MainClass::getInstance()->m_nama;
    //qDebug()<<"+++++++"<<m_filter.at(1).toStringList()[index];
    nama->itemSetParams(nama->m_BeautyHandles, "filter_name", m_filter.at(1).toStringList()[index].toStdString());
}

void UIBridge::setCustomMakeupValue(int index, QString value)
{
    QList<QVariant> tempList = m_customMakeup.at(index);
    QStringList strList = tempList.at(0).toStringList();
    strList.replace(3, value);
    tempList.replace(0, strList);
    m_customMakeup.replace(index, tempList);
    //nama更新sdk参数
    Nama *nama = MainClass::getInstance()->m_nama;
    //qDebug()<<strList.at(2)<<value.toDouble() / 100;
    nama->itemSetParamd(nama->m_MakeUpHandle, strList.at(2).toStdString(), value.toDouble() / 100);
}

void UIBridge::setCustomMakeupIndex(int index, QString value)
{
    QList<QVariant> tempList = m_customMakeup.at(index);
    QStringList strList = tempList.at(0).toStringList();
    strList.replace(4, value);
    tempList.replace(0, strList);
    m_customMakeup.replace(index, tempList);
    Nama *nama = MainClass::getInstance()->m_nama;
    //这里要做加载道具,如果是眉毛口红,更新type
    //(眉毛变形类型brow_warp_type,口红类型lip_type)
    //第一组 0 {"腮红", "makeup_blusher_color", "makeup_intensity_blusher", "100", "-1", "-1"}
    //      1 {"苹果肌", "-1", "demo_style_blush_01.png","mu_style_blush_01","#F4706B","#ED596D","#E24247","#E24266","#FF6351"}
    //      2 {"扇形", "-1", "demo_style_blush_02.png","mu_style_blush_02","#F4706B","#ED596D","#E24247","#E24266","#FF6351"}
    //例如选苹果肌(下标0),加载mu_style_blush_01道具
    QString typeName = strList.at(0);
    QString name = tempList.at(value.toInt() + 1).toStringList().at(3);
    QString bundleName = gBundlePath[m_selectCategory] + "/subs/" + name + ".bundle";
    int type = tempList.at(value.toInt() + 1).toStringList().at(1).toInt();
    m_bmakeup_moisturized = false;
    if(index == 9){
        int form = 0;
        //口红要加载道具还要设置类型
        if(name.compare("mu_style_lip_05") == 0){
            m_bmakeup_moisturized = true;
        }
        nama->appendMakeUpItems(typeName.toStdString(), bundleName.toStdString(), type, form);
    }else{
        nama->appendMakeUpItems(typeName.toStdString(), bundleName.toStdString());
    }
    //qDebug()<<"setCustomMakeupIndex"<<index<<value<<typeName<<bundleName<<type<<form;
    //设置完再设置强度
    nama->itemSetParamd(nama->m_MakeUpHandle, strList.at(2).toStdString(), strList.at(3).toDouble() / 100);
}

std::vector<double> UIBridge::GetColorDouble(QString color)
{
    std::vector<double> ret;
    if(color.compare("") != 0){
        cv::Vec4b cvC = { uchar(color.mid(1,2).toInt(NULL,16)),uchar(color.mid(3,2).toInt(NULL,16)), uchar(color.mid(5,2).toInt(NULL,16)), 255 };
        ret.push_back((double)cvC[0] / 255.0f);
        ret.push_back((double)cvC[1] / 255.0f);
        ret.push_back((double)cvC[2] / 255.0f);
        ret.push_back((double)cvC[3] / 255.0f);
    }
    return ret;
}

void UIBridge::setCustomMakeupColor(int index, QString value)
{
    QList<QVariant> tempList = m_customMakeup.at(index);
    QStringList strList = tempList.at(0).toStringList();
    strList.replace(5, value);
    tempList.replace(0, strList);
    QString color0 = "", color1 = "", color2 = "";
    QStringList colorList = tempList.at(strList.at(4).toInt() + 1).toStringList();
    if(colorList.size() < 5){
        return;
    }
    QString colorFull = tempList.at(strList.at(4).toInt() + 1).toStringList().at(value.toInt() + 4);
    if(colorFull.length() >= 7){
        color0 = colorFull.mid(0,7);
    }
    if(colorFull.length() >= 14){
        color1 = colorFull.mid(7,14);
    }
    if(colorFull.length() >= 21){
        color2 = colorFull.mid(14);
    }
    m_customMakeup.replace(index, tempList);
    QString name = strList.at(1);
    if(m_bmakeup_moisturized){
        name = "makeup_lip_color_v2";
    }
    //qDebug()<<"setCustomMakeupColor"<<name<<colorFull;
    MainClass::getInstance()->m_nama->setMakeUpColor(name.toStdString(), GetColorDouble(color0), GetColorDouble(color1), GetColorDouble(color2));
}

void UIBridge::resetCustomMakeup()
{
    m_customMakeup = m_defaultCustomMakeup;
    MainClass::getInstance()->m_nama->ClearAllCM();
}

void UIBridge::changeCamera(int index)
{
    MainClass::getInstance()->m_camera->changeCamera(index);
}

void UIBridge::changeCameraSet(int index)
{
    if(index >= 0){
        MainClass::getInstance()->m_camera->changeCameraSet(index);
    }
}

void UIBridge::gsCameraConfirm()
{
    //暂停选择视频播放
    m_gsSelectCamera = true;
    m_gsSelectVideoPath = "";
    gsSelectCameraChanged();
    m_gsVideoMediaPlayer.stop();
    fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
    fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
    m_gsStart = QPointF(0.5,0.5);
    m_gsSize = QPointF(0.5,0.5);
    MainClass::getInstance()->m_nama->changeGSPreviewRect(0.5, 0.5, 1, 1);
    m_frameSize = QString::number(MainClass::getInstance()->m_camera->m_FrameWidth) + "x" +
            QString::number(MainClass::getInstance()->m_camera->m_FrameHeight);
    emit frameSizeChanged();
}

void UIBridge::selectColor(int mouseX, int mouseY)
{
    MainClass *main = MainClass::getInstance();
    cv::Mat frameMat = main->m_nama->m_frame.clone();
    if(frameMat.channels() == 1){
        cv::cvtColor(frameMat, frameMat, cv::COLOR_YUV2BGRA_IYUV);
    }
    double ratioX = frameMat.cols / double(main->m_UIBridge->m_cameraWidth);
    double ratioY = frameMat.rows / double(main->m_UIBridge->m_cameraHeight);
    //绿幕选择相机,图像要水平翻转
    if(m_gsSelectCamera){
        cv::flip(frameMat, frameMat, 1);
    }
    if (frameMat.data)
    {
        int posx = ratioX * mouseX;
        int posy = ratioY * mouseY;
        auto dataBGR = frameMat.at<cv::Vec4b>(posy, posx);
        QString color = "#" + QString("%1").arg(dataBGR[2],2,16,QChar('0')) +
                QString("%1").arg(dataBGR[1],2,16,QChar('0')) +
                QString("%1").arg(dataBGR[0],2,16,QChar('0'));
        color = color.toUpper();
        //不是蓝绿白,保存最近使用颜色
        if(color.compare("#FFFFFF") != 0 && color.compare("#0000FF") != 0 && color.compare("#00FF00") != 0){
            saveGSKeyColor(QVector3D(dataBGR[2],dataBGR[1],dataBGR[0]));
        }
        emit selectColorChanged(color);
    }
}

void UIBridge::saveGSKeyColor(QVector3D color3d)
{
    bool sameFlag = false;
    for(int i = 0; i < m_gsKeyColorList.size(); i++){
        if(m_gsKeyColorList.at(i) == color3d){
            sameFlag = true;
        }
    }
    if(!sameFlag){
        if(m_gsKeyColorList.size() == 4){
            m_gsKeyColorList.push_front(color3d);
            m_gsKeyColorList.removeLast();
        }else{
            m_gsKeyColorList.push_front(color3d);
        }
        saveUserConfig();
        QStringList strList;
        for(int i = 0; i < m_gsKeyColorList.size(); i++){
            QVector3D vector3d = m_gsKeyColorList.at(i);
            QString color = "#" + QString("%1").arg(int(vector3d.x()),2,16,QChar('0')) +
                    QString("%1").arg(int(vector3d.y()),2,16,QChar('0')) +
                    QString("%1").arg(int(vector3d.z()),2,16,QChar('0'));
            color = color.toUpper();
            strList.append(color);
        }
        m_gsKeyColor = strList;
    }
}

void UIBridge::gsColorChange(QString color)
{
    m_gsColor = { uchar(color.mid(1,2).toInt(NULL,16)),uchar(color.mid(3,2).toInt(NULL,16)), uchar(color.mid(5,2).toInt(NULL,16)), 255 };
    MainClass::getInstance()->m_nama->SetGSKeyColor(m_gsColor);
    int m_GSHandle = MainClass::getInstance()->m_nama->m_GSHandle;
    //取色完成后更新绿幕参数
    updateCategory(m_greenScreen, 3, 0, QString::number(fuItemGetParamd(m_GSHandle, "chroma_thres") * 100));
    updateCategory(m_greenScreen, 3, 1, QString::number(fuItemGetParamd(m_GSHandle, "chroma_thres_T") * 100));
    updateCategory(m_greenScreen, 3, 2, QString::number(int(fuItemGetParamd(m_GSHandle, "alpha_L") * 100)));
    //qDebug()<< fuItemGetParamd(m_GSHandle, "chroma_thres") << fuItemGetParamd(m_GSHandle, "chroma_thres_T")  << fuItemGetParamd(m_GSHandle, "alpha_L");
    emit updateGreenScreenParam();
}

void UIBridge::gsPlayBGVideo(QString videopath)
{
    //qDebug()<<"gsPlayBGVideo"<<videopath;
    m_gsMediaPlayer.stop();
    m_gsvideoSurface = new QtCameraCapture(true);
    m_gsMediaPlayer.setVideoOutput(m_gsvideoSurface);
    QMediaPlaylist *playlist = new QMediaPlaylist;
    //循环播放
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    playlist->addMedia(QUrl::fromLocalFile(videopath));
    m_gsMediaPlayer.setPlaylist(playlist);
    connect(m_gsvideoSurface, SIGNAL(presentGSBGFrame(const cv::Mat &)), MainClass::getInstance()->m_UIBridge, SLOT(updateGreenScreenSegment(const cv::Mat &)), Qt::AutoConnection);
    m_gsMediaPlayer.play();
}

void UIBridge::gsSelectColor()
{
    m_bSelectColor = true;
    MainClass::getInstance()->m_nama->changeRenderList(RENDER_PICKCOLOR);
    MainClass::getInstance()->m_nama->changeGSPreviewRect(0, 0, 1, 1);
}

void UIBridge::gsUnSelectColor()
{
    m_bSelectColor = false;
    MainClass::getInstance()->m_nama->changeRenderList(RENDER_GREEN);
    gsTranslocation(0,0);
}

void UIBridge::gsSelectVideo(QString path)
{
    m_gsSelectCamera = false;
    gsSelectCameraChanged();
    MainClass::getInstance()->m_camera->m_QCamera->stop();
    //关闭摄像头输入
    fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0);
    fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0);
    MainClass::getInstance()->m_nama->m_FrameID = 0;
    m_bgsSelectVideo = true;
    if(m_gsSelectVideoPath==path){
        m_gsVideoMediaPlayer.play();
        return;
    }
    m_gsSelectVideoPath = path;
    QtCameraCapture *videoSurface = new QtCameraCapture(true);
    m_gsVideoMediaPlayer.setVideoOutput(videoSurface);
    QMediaPlaylist *playlist = new QMediaPlaylist;
    //单次播放,视频则在界面显示重新播放按钮
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    //    //循环播放
    //    if(path.contains(".png") || path.contains(".jpg")){
    //        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    //    }else{
    //        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    //    }
    emit updateReplayButton(false);
    playlist->addMedia(QUrl(path));
    m_gsVideoMediaPlayer.setPlaylist(playlist);
    if(path.contains(".mp4") || path.contains(".avi") || path.contains(".wmv")){
        connect(&m_gsVideoMediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(changedStatus(QMediaPlayer::MediaStatus)));
    }else{
        disconnect(&m_gsVideoMediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(changedStatus(QMediaPlayer::MediaStatus)));
    }
    //第一帧改变绿幕起始位置,绿幕图像大小
    connect(videoSurface, SIGNAL(presentGSBGFrame(const cv::Mat &)), MainClass::getInstance()->m_UIBridge, SLOT(getGSPresentFrame(const cv::Mat &)), Qt::AutoConnection);
    m_gsVideoMediaPlayer.play();
}

void UIBridge::bgsSelectVideo(QString path)
{
    m_begUserFilePath = path;
    saveUserConfig();
    m_bNeedSavebg_seg = true;
    MainClass::getInstance()->m_nama->m_FrameID = 0;
    m_bgsVideoMediaPlayer.stop();
    QtCameraCapture *videoSurface = new QtCameraCapture(true);
    m_bgsVideoMediaPlayer.setVideoOutput(videoSurface);
    QMediaPlaylist *playlist = new QMediaPlaylist;
    m_bgsVideoMediaPlayer.setPlaylist(playlist);
    //循环播放
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    playlist->addMedia(QUrl(path));
    m_bgsVideoMediaPlayer.setPlaylist(playlist);
    connect(videoSurface, SIGNAL(presentGSBGFrame(const cv::Mat &)), MainClass::getInstance()->m_UIBridge, SLOT(updateBackgroundSegment(const cv::Mat &)), Qt::AutoConnection);
    m_bgsVideoMediaPlayer.play();
}

void UIBridge::gsSafeAreaSelect(QString path)
{
    path = path.mid(8);
    QImage image(path);
    updateGSSafeArea(image);
    image = image.scaled(128, 72, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QSize size(128, 72);
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(28,0,72,72,100,100);
    QPixmap imagepix = QPixmap::fromImage(image);
    imagepix.setMask(mask);
    imagepix = imagepix.copy(28,0,72,72);
    imagepix.save("gs_savearea_shot.png");
    QStringList tempList = m_greenScreenIcon.at(1).toStringList();
    if(!tempList.contains("gs_savearea_shot")){
        tempList.insert(1, "gs_savearea_shot");
        m_greenScreenIcon.replace(1, tempList);
    }
    emit updateGSSafePic();
    m_gsSafeUserFilePath = path;
    saveUserConfig();
}

void UIBridge::openHelpText()
{
    WinExec("notepad.exe ../README.md", SW_SHOW);
}

int UIBridge::getStickerLength()
{
    if(MainClass::getInstance()->m_stickerHolder->mTagBundleList.size() > 0){
        return MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex].size();
    }else{
        return 0;
    }
}

QStringList UIBridge::cameraSetList(){
    return MainClass::getInstance()->m_camera->m_cameraSetList;
}


QStringList UIBridge::cameraList(){
    return MainClass::getInstance()->m_camera->m_cameraList;
}

QStringList UIBridge::stickerTip()
{
    if(m_stickerTip.size() == 0){
        for(int i = 0; i < MainClass::getInstance()->m_stickerHolder->mTags.size(); i++){
            m_stickerTip.append(QString::fromStdString(MainClass::getInstance()->m_stickerHolder->mTags[i]).mid(0,2));
        }
    }
    return m_stickerTip;
}

QVariant UIBridge::getStickerValue(int index, int n)
{
    switch (n){
    case 0:
        return QString::fromLocal8Bit(MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mIconDir.c_str());
        break;
    case 1:
        return bool(MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mBundleIsDownload);
        break;
    case 2:
        return bool(MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mIsDownloading);
        break;
    default:
        return "";
        break;
    }
}

void UIBridge::downloadSticker(int index)
{
    m_selectCategoryIndex = index;
    //没下载下载
    if (MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mBundleIsDownload == false &&
            MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mIsDownloading == false && m_GIsDownloading == false)
    {
        std::packaged_task<void()> task([&, index]() {
            MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mIsDownloading = true;
            auto strUrl = MainClass::getInstance()->m_stickerHolder->RequestBundleUrl(MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mId);
            if (strUrl.length() > 0)
            {
                for (auto& bundledir : MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mBundleDirs)
                {
                    //如果包括,则是zip文件,替换文件名
                    bool bZip = false;
                    string dirpath;
                    if(bundledir.find(",") != string::npos){
                        dirpath = bundledir.substr(0,bundledir.find("/",10) + 1);
                        bundledir = dirpath + "bundle.zip";
                        bZip = true;
                    }
                    MainClass::getInstance()->m_stickerHolder->DownLoadFile(strUrl, bundledir);
                    if(bZip){
                        MainClass::getInstance()->m_stickerHolder->UnzipFile(bundledir, dirpath, m_stickerIndex, index);
                        break;
                    }
                }
                MainClass::getInstance()->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mBundleIsDownload = true;
                emit finishDownload(index);
                useBoutique(index);
            }
            m_GIsDownloading = false;
            m_GDownloadBundleThread.detach();
        });
        m_GIsDownloading = true;
        m_GDownloadBundleThread = std::thread(std::move(task));
    }
    //已经下载直接使用
    else{
        useBoutique(index);
    }
}


void UIBridge::useBoutique(int index)
{
    m_selectCategoryLast = m_selectCategory;
    m_bLoadBear = false;
    int maxPeople = 4;
    MainClass *main = MainClass::getInstance();
    Nama *nama = MainClass::getInstance()->m_nama;
    std::unique_lock<std::mutex> lock(nama->m_frameMutex);
    //显示提示文字
    string tipText = main->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mEventToast;
    if(tipText != ""){
        m_tip = QString::fromStdString(tipText);
        tipChanged();
    }
    if (main->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->IsOnlyOnePeople())
    {
        maxPeople = 1;
    }
    if(nama->m_mp3 != nullptr)
    {
        nama->m_mp3->Pause();
    }
    updateFilter();
    //小熊贴纸特有
    if(main->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mBundleDirs.size() > 1){
        unLoadAvatar();
        m_bodyTrackType = BodyTrackType::FullBody;
        std::vector<std::string> mBundleDirs;
        for (auto& index: main->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mBundleDirs){
            mBundleDirs.emplace_back(index);
        }
        nama->LoadAvatarBundles(mBundleDirs);
        nama->SetBodyTrackType(BodyTrackType::FullBody);
        nama->ApplyBodyTrackConfig(m_bodyTrackConfig.BearFull);
        nama->m_Controller->EnableHumanFollowMode(false);
        nama->DestroyAll();
        m_bLoadAvatar = true;
        m_bLoadBear = true;
    }else{
        unLoadAvatar();
        for (auto& index: main->m_stickerHolder->mTagBundleList[m_stickerIndex][index]->mBundleDirs){
            nama->SelectBundle(index,maxPeople);
            if(m_flagARBody){
                nama->changeRenderList(RENDER_BODY);
            }else{
                nama->changeRenderList(RENDER_AR);
            }
        }

    }
}

void UIBridge::reloadItemParam()
{
    Nama *nama = MainClass::getInstance()->m_nama;
    for(int i = 0 ; i < m_beautySkin.at(0).toStringList().size(); i++){
        if(i <= 1){
            namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, i, true);
        }else{
            namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, i, false);
        }
    }
    for(int i = 0 ; i < m_beautyFace.at(0).toStringList().size(); i++){
        namaFuItemSetParamd(nama->m_BeautyHandles, m_beautyFace, i, false);
    }
    for(int i = 0 ; i < m_beautyBody.at(0).toStringList().size(); i++){
        namaFuItemSetParamd(nama->m_BodyShapeHandle, m_beautyBody, i, false);
    }
    for(int i = 0 ; i < m_greenScreen.at(0).toStringList().size(); i++){
        namaFuItemSetParamd(nama->m_GSHandle, m_greenScreen, i, false);
    }
    updateFilter();
    nama->changeGSPreviewRect(m_gsStart.x(), m_gsStart.y(), m_gsStart.x() + m_gsSize.x(), m_gsStart.y() + m_gsSize.y());
}

void UIBridge::checkBodyParam()
{
    Nama *nama = MainClass::getInstance()->m_nama;
    QStringList valueList = m_beautyBody.at(3).toStringList();
    bool allzeroFlag = true;
    for(int i = 0 ; i < valueList.size(); i++){
        if(valueList.at(i).toInt() > 0){
            allzeroFlag = false;
        }
    }
    if(allzeroFlag){
        m_flagARBody = false;
    }
    arBodyFlagChanged(m_flagARBody);
    if(m_flagARBody){
        nama->changeRenderList(RENDER_BODY);
        if(nama->m_bundleCategory == BundleCategory::MusicFilter)
        {
            nama->m_mp3->Pause();
        }
    }else{
        nama->changeRenderList(RENDER_AR, m_bmakeupFlag);
        if(nama->m_bundleCategory == BundleCategory::MusicFilter && nama->m_bundleCurrent > 0)
        {
            nama->m_mp3->Play();
        }else if(nama->m_bundleCategory == BundleCategory::LightMakeup && nama->m_bundleCurrent > 0)
        {
            setLightMakeUpParam(m_lightMakeUpName);
        }
    }
}

void UIBridge::itemJingpinClick()
{
    MainClass::getInstance()->m_nama->itemJingpinClick();
}

QPointF UIBridge::gsLocation(double moveX, double moveY)
{
    QPointF gsTemp = QPointF(m_gsStart.x() + moveX, m_gsStart.y() + moveY);
    QPointF maxPos(1 - m_gsSize.x(), 1 - m_gsSize.y());
    if(gsTemp.x() < 0){
        gsTemp.setX(0);
    }else if(gsTemp.x() > maxPos.x()){
        gsTemp.setX(maxPos.x());
    }
    if(gsTemp.y() < 0){
        gsTemp.setY(0);
    }else if(gsTemp.y() > maxPos.y()){
        gsTemp.setY(maxPos.y());
    }
    return gsTemp;
}

void UIBridge::gsTranslocation(double moveX, double moveY)
{
    QPointF gsTemp = gsLocation(moveX, moveY);
    QPointF gsEnd = QPointF(gsTemp.x() + m_gsSize.x(), gsTemp.y() + m_gsSize.y());
    emit updateGSPreviewRect(gsTemp.x(), gsTemp.y(), gsEnd.x(), gsEnd.y());
    MainClass::getInstance()->m_nama->changeGSPreviewRect(gsTemp.x(), gsTemp.y(), gsEnd.x(), gsEnd.y());
}

void UIBridge::gsZoom(bool zoom)
{
    if(zoom){
        m_gsSize.setX(m_gsSize.x()*1.1);
        m_gsSize.setY(m_gsSize.y()*1.1);
    }else{
        m_gsSize.setX(m_gsSize.x()*0.9);
        m_gsSize.setY(m_gsSize.y()*0.9);
    }
    //比例限制0.1-1.0
    if(m_gsSize.x() > 1.0){
        m_gsSize.setY(m_gsSize.y() / m_gsSize.x());
        m_gsSize.setX(1);
    }else if(m_gsSize.y() > 1.0){
        m_gsSize.setX(m_gsSize.x() / m_gsSize.y());
        m_gsSize.setY(1);
    }else if(m_gsSize.x() < 0.1){
        m_gsSize.setY(m_gsSize.y() * 0.1 / m_gsSize.x());
        m_gsSize.setX(0.1);
    }else if(m_gsSize.y() < 0.1){
        m_gsSize.setX(m_gsSize.x() * 0.1 / m_gsSize.y());
        m_gsSize.setY(0.1);
    }
    gsTranslocation(0,0);
}

void UIBridge::gsUpdatelocation(double moveX, double moveY)
{
    m_gsStart = gsLocation(moveX, moveY);
}

void UIBridge::namaFuItemSetParamd(int handle, QList<QVariant> &valueList, int index, bool flag)
{
    Nama *nama = MainClass::getInstance()->m_nama;
    QString name = valueList[2].toStringList().at(index);
    double dValue = valueList[3].toStringList().at(index).toDouble();
    if(flag){
        //按钮选项
        nama->itemSetParamd(handle, name.toStdString(), dValue);
    }else{
        //滑块
        bool bFlag = valueList[4].toStringList().at(index).toInt();
        //如果有负数
        if (bFlag)
        {
            dValue += 50;
        }
        //磨皮0-6
        if(name == "blur_level"){
            nama->itemSetParamd(handle, name.toStdString(), dValue * 6.0/ 100.0);
        }else{
            nama->itemSetParamd(handle, name.toStdString(), dValue / 100.0);
        }
    }
}

void UIBridge::getFPS()
{
    Nama *nama = MainClass::getInstance()->m_nama;
    int FrameID = nama->m_FrameID;//视频帧Id
    static int FrameIDLast = 0;//上一秒计算时的视频帧id计算rendertime用
    static int FrameIDFPSLast = 0;//上一秒计算时的帧id计算fps用
    int frameCount = FrameID - FrameIDLast;
    m_FPS = m_FrameIDFPS - FrameIDFPSLast;
    //最小化不刷新
    if(frameCount == 0){
        frameCount = 1;
    }
    m_renderTime = m_secondRenderTime / frameCount;
    emit fpsChanged();
    emit renderTimeChanged();
    m_secondRenderTime = 0;
    FrameIDFPSLast = m_FrameIDFPS;
    FrameIDLast = FrameID;
    if(m_FPS > 100){
        Sleep(1000);
        m_FPS = 0;
    }
}

void UIBridge::openWebCamera(QString path)
{
    //qDebug()<<path;
    QtCameraCapture *videoSurface = new QtCameraCapture(true);
    m_webcamMediaPlayer.setVideoOutput(videoSurface);
    QMediaPlaylist *playlist = new QMediaPlaylist;
    //循环播放
    playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    playlist->addMedia(QUrl(path));
    m_webcamMediaPlayer.setPlaylist(playlist);
    connect(videoSurface, SIGNAL(presentGSBGFrame(const cv::Mat &)), MainClass::getInstance()->m_UIBridge, SLOT(getGSPresentFrame(const cv::Mat &)), Qt::AutoConnection);
    m_webcamMediaPlayer.play();
}

void UIBridge::stopStartWebCamera(bool flag)
{
    if(flag){
        m_webcamMediaPlayer.pause();
    }else{
        m_webcamMediaPlayer.play();
    }
}

void UIBridge::changeCameraType(bool type)
{
    m_CameraType = type;
    if(type){
        MainClass::getInstance()->m_camera->m_QCamera->stop();
        m_webcamMediaPlayer.play();
        fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0);
        fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0);
    }
    else{
        MainClass::getInstance()->m_camera->m_QCamera->start();
        m_webcamMediaPlayer.stop();
        fuSetInputCameraBufferMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
        fuSetInputCameraTextureMatrix(TRANSFORM_MATRIX::CCROT0_FLIPHORIZONTAL);
    }
}

void UIBridge::setLightMakeUpParam(QString name)
{
    Nama *nama = MainClass::getInstance()->m_nama;
    fuItemSetParamd(nama->m_bundleCurrent, "is_makeup_on", 1.0);
    if(name.compare("light_makeup_peachblossom") == 0){
        setLightMakeUpTex("tex_blusher", g_assetDir + "items/LightMakeup/blusher/mu_blush_01.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_blusher", 0.9);
        setLightMakeUpTex("tex_eye", g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_01.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eye", 0.9);
        setLightMakeUpTex("tex_brow", g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_01.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eyeBrow", 0.5);
        setLightMakeUpLipColor(g_assetDir + "items/LightMakeup/lipstick/mu_lip_01.json");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity", 0.9);
        nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "fennen3");
        nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 1.0);
    }else if(name.compare("light_makeup_grapefruit") == 0){
        setLightMakeUpTex("tex_blusher", g_assetDir + "items/LightMakeup/blusher/mu_blush_23.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_blusher", 1.0);
        setLightMakeUpTex("tex_eye", g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_21.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eye", 0.75);
        setLightMakeUpTex("tex_brow", g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_19.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eyeBrow", 0.6);
        setLightMakeUpLipColor(g_assetDir + "items/LightMakeup/lipstick/mu_lip_21.json");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity", 0.8);
        nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "lengsediao4");
        nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 0.7);
    }else if(name.compare("light_makeup_clear") == 0){
        setLightMakeUpTex("tex_blusher", g_assetDir + "items/LightMakeup/blusher/mu_blush_22.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_blusher", 0.9);
        setLightMakeUpTex("tex_eye", g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_20.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eye", 0.65);
        setLightMakeUpTex("tex_brow", g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_18.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eyeBrow", 0.45);
        setLightMakeUpLipColor(g_assetDir + "items/LightMakeup/lipstick/mu_lip_20.json");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity", 0.8);
        nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "xiaoqingxin1");
        nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 0.8);
    }else if(name.compare("light_makeup_boyfriend") == 0){
        setLightMakeUpTex("tex_blusher", g_assetDir + "items/LightMakeup/blusher/mu_blush_20.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_blusher", 0.8);
        setLightMakeUpTex("tex_eye", g_assetDir + "items/LightMakeup/eyeshadow/mu_eyeshadow_18.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eye", 0.9);
        setLightMakeUpTex("tex_brow", g_assetDir + "items/LightMakeup/eyebrow/mu_eyebrow_16.png");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity_eyeBrow", 0.65);
        setLightMakeUpLipColor(g_assetDir + "items/LightMakeup/lipstick/mu_lip_18.json");
        fuItemSetParamd(nama->m_bundleCurrent, "makeup_intensity", 1.0);
        nama->itemSetParams(nama->m_BeautyHandles, "filter_name", "xiaoqingxin3");
        nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", 0.9);
    }
}

void UIBridge::setLightMakeUpTex(string value, string imagepath)
{
    cv::Mat image = cv::imread(imagepath,cv::IMREAD_UNCHANGED);
    cv::cvtColor(image, image, cv::COLOR_BGRA2RGBA);
    fuCreateTexForItem(MainClass::getInstance()->m_nama->m_bundleCurrent, value.data(), image.data, image.cols, image.rows);
}

void UIBridge::setLightMakeUpLipColor(string colorpath)
{
    std::vector<double> color;
    QFile *file = new QFile(QString::fromStdString(colorpath));
    if(file->open(QFile::ReadOnly))
    {
        QString value = file->readAll();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug()<<tr("解析json文件错误！");
        }else{
            QJsonObject jsonObject = document.object();
            if(jsonObject.contains("rgba")){
                QJsonArray jsonA = jsonObject["rgba"].toArray();
                for(int i = 0; i < jsonA.size(); i++){
                    color.push_back(jsonA.at(i).toDouble());
                }
            }
        }
        file->close();
    }
    fuItemSetParamdv(MainClass::getInstance()->m_nama->m_bundleCurrent, "makeup_lip_color", color.data(), color.size());

}

void UIBridge::updateFilter()
{
    Nama *nama = MainClass::getInstance()->m_nama;
    nama->itemSetParams(nama->m_BeautyHandles, "filter_name", m_filter.at(1).toStringList()[m_filterIndex].toStdString());
    nama->itemSetParamd(nama->m_BeautyHandles, "filter_level", m_filter.at(2).toStringList()[m_filterIndex].toDouble() / 100);
}

void UIBridge::updateStyleRecommendation(int index)
{
    Nama *nama = MainClass::getInstance()->m_nama;
    m_bSaveStyleRecommendation = false;
    //替换对应风格推荐参数下标,美颜默认名称"BeautyParam"
    if(index == -1){
        index = m_styleRecommendationParam.mNameList.indexOf("BeautyParam");
    }else{
        index = m_styleRecommendationParam.mNameList.indexOf(m_styleRecommendationName.at(index));
    }
    m_styleRecommendationIndex = index;
    m_beautySkin.replace(3, m_styleRecommendationParam.mBeautySkin.at(index));
    m_beautyFace.replace(3, m_styleRecommendationParam.mBeautyFace.at(index));
    for(int i = 0 ; i < m_beautySkin.at(0).toStringList().size(); i++){
        if(i <= 1){
            namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, i, true);
        }else{
            namaFuItemSetParamd(nama->m_BeautyHandles, m_beautySkin, i, false);
        }
    }
    for(int i = 0 ; i < m_beautyFace.at(0).toStringList().size(); i++){
        namaFuItemSetParamd(nama->m_BeautyHandles, m_beautyFace, i, false);
    }
    //更新界面美颜,美型参数
    emit updateBeautySkinParam();
    emit updateBeautyFaceParam();
    m_bSaveStyleRecommendation = true;
    updateFilter();
}

void UIBridge::setBackgroundSegType(int type)
{
    //fuSetHumanSegScene(FUAIHUMANSEGSCENETYPE(type));
    m_bSetBackgroundSegType = type;
    //需要同线程,渲染中修改
    //MainClass::getInstance()->m_nama->setBackgroundSegType(type);
}

void UIBridge::changedStatus(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::MediaStatus::EndOfMedia){
        gsTranslocation(0,0);
        emit updateReplayButton(true);
    }
}

bool UIBridge::checkModuleCode(int category)
{
    int passCode = fuGetModuleCode(g_checkIndex[category]);
    int needCode = g_checkID[category];
    bool bOK = false;
    if ((passCode & needCode) == needCode) {
        bOK = true;
    }
    return bOK;
}

bool UIBridge::checkModuleCodeSide(int side)
{
    int passCode = fuGetModuleCode(g_checkSideIndex[side]);
    int needCode = g_checkSideID[side];
    bool bOK = false;
    if ((passCode & needCode) == needCode) {
        bOK = true;
    }
    return bOK;
}

void UIBridge::detectionTip()
{
    if(MainClass::getInstance()->m_nama->m_bundleCategory == BundleCategory::GestureRecognition){
        if (fuHandDetectorGetResultNumHands() == 0) {
            m_tipExtra = "未检测到手势";
            tipExtraChanged();
        }else{
            tipExtraChanged(0);
        }
    }else{
        int passCode = fuGetModuleCode(1);
        int needCode = g_checkID[DEFINE_GET_FACEINFO];
        bool bOK = false;
        if ((passCode & needCode) == needCode) {
            bOK = true;
        }
        if(bOK){
            if(fuFaceProcessorGetNumResults() == 0){
                m_tipExtra = "未检测到人脸";
                tipExtraChanged();
            }else{
                tipExtraChanged(0);
            }
        }else{
            static bool once = false;
            if(!once){
                once = true;
                m_tipExtra = "没有获取人脸信息权限";
                tipExtraChanged(10);
            }
        }
    }
}

void UIBridge::detectionBodyTip(){
    int passCode = fuGetModuleCode(1);
    int needCode = g_checkID[DEFINE_BODY_TRACK];
    bool bOK = false;
    if ((passCode & needCode) == needCode) {
        bOK = true;
    }
    if(bOK){
        if(fuHumanProcessorGetNumResults() == 0){
            m_tipExtra = "未检测到人体";
            tipExtraChanged();
        }else{
            tipExtraChanged(0);
        }
    }else{
        static bool once = false;
        if(!once){
            once = true;
            m_tipExtra = "没有获取人体信息权限";
            tipExtraChanged(10);
        }
    }
}

void UIBridge::updateTapContent(int index)
{
    switch (index)
    {
    case 0:
        if (!checkModuleCodeSide(index)){
            m_tip = "美颜权限不足";
            tipChanged();
        }
        break;
    case 1:
        if (!checkModuleCodeSide(index)){
            m_tip = "美颜权限不足";
            tipChanged();
        }
        break;
    case 2:
        if (!checkModuleCodeSide(index)){
            m_tip = "滤镜权限不足";
            tipChanged();
        }
        break;
    case 3:
        if (!checkModuleCodeSide(index)){
            m_tip = "美体权限不足";
            tipChanged();
        }
        break;
    }
}
