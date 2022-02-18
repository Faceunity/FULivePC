#include "Camera.h"
#include <QDebug>
#include <MainClass.h>

QtCameraCapture::QtCameraCapture(bool video, QObject *parent)
{
    m_bVideo = video;
}

QList<QVideoFrame::PixelFormat> QtCameraCapture::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()<< QVideoFrame::Format_RGB32<< QVideoFrame::Format_YUYV<< QVideoFrame::Format_BGR24<< QVideoFrame::Format_YUV420P;
    //现在默认选择Format_RGB32, 如果打开下面QVideoFrame会选择Format_YUYV 或者 Format_BGR24
    //                << QVideoFrame::Format_ARGB32
    //                << QVideoFrame::Format_ARGB32_Premultiplied
    //                << QVideoFrame::Format_RGB32
    //                << QVideoFrame::Format_RGB24
    //                << QVideoFrame::Format_RGB565
    //                << QVideoFrame::Format_RGB555
    //                << QVideoFrame::Format_ARGB8565_Premultiplied
    //                << QVideoFrame::Format_BGRA32
    //                << QVideoFrame::Format_BGRA32_Premultiplied
    //                << QVideoFrame::Format_BGR32
    //                << QVideoFrame::Format_BGR24
    //                << QVideoFrame::Format_BGR565
    //                << QVideoFrame::Format_BGR555
    //                << QVideoFrame::Format_BGRA5658_Premultiplied
    //                << QVideoFrame::Format_AYUV444
    //                << QVideoFrame::Format_AYUV444_Premultiplied
    //                << QVideoFrame::Format_YUV444
    //                << QVideoFrame::Format_YUV420P
    //                << QVideoFrame::Format_YV12
    //                << QVideoFrame::Format_UYVY
    //                << QVideoFrame::Format_YUYV
    //                << QVideoFrame::Format_NV12
    //                << QVideoFrame::Format_NV21
    //                << QVideoFrame::Format_IMC1
    //                << QVideoFrame::Format_IMC2
    //                << QVideoFrame::Format_IMC3
    //                << QVideoFrame::Format_IMC4
    //                << QVideoFrame::Format_Y8
    //                << QVideoFrame::Format_Y16
    //                << QVideoFrame::Format_Jpeg
    //                << QVideoFrame::Format_CameraRaw
    //                << QVideoFrame::Format_AdobeDng;
}

bool QtCameraCapture::present(const QVideoFrame &frame)
{
    if(frame.isValid()) {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        //处理绿幕视频
        if(m_bVideo){
            cv::Mat mat;
            mat = cv::Mat(cloneFrame.height(), cloneFrame.width(), CV_8UC4, (void*)cloneFrame.bits(), cloneFrame.bytesPerLine());
            //将4通道设为255不透明,绿幕视频有透明
            cv::Mat bgra[4];
            cv::split(mat, bgra);
            bgra[3] = 255.0;
            cv::merge(bgra, 4, mat);
            cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
            emit presentGSBGFrame(mat);
            return true;
        }
        //处理摄像头图像
        else{
            if(cloneFrame.pixelFormat() == QVideoFrame::Format_YUYV)//yuv422
            {
              cv::Mat mat = cv::Mat(cloneFrame.height(), cloneFrame.width(), CV_8UC2, (void*)cloneFrame.bits(), cloneFrame.bytesPerLine());
              cv::cvtColor(mat, mat, cv::COLOR_YUV2BGRA_YUY2);
              emit presentFrame(mat);
              return true;
            }
            if(cloneFrame.pixelFormat() == QVideoFrame::Format_RGB32)//bgra
            {
                cv::Mat mat = cv::Mat(cloneFrame.height(), cloneFrame.width(), CV_8UC4, (void*)cloneFrame.bits(), cloneFrame.bytesPerLine());
                //沿x轴翻转,垂直翻转
                cv::flip(mat, mat, 0);
                emit presentFrame(mat.clone());
                return true;
            }
            if(cloneFrame.pixelFormat() == QVideoFrame::Format_BGR24)//bgr
            {
                cv::Mat mat = cv::Mat(cloneFrame.height(), cloneFrame.width(), CV_8UC3, (void*)cloneFrame.bits(), cloneFrame.bytesPerLine());
                cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
                //沿x轴翻转,垂直翻转
                cv::flip(mat, mat, 0);
                emit presentFrame(mat);
            }
            if(cloneFrame.pixelFormat() == QVideoFrame::Format_YUV420P)
            {
                cv::Mat mat = cv::Mat(cloneFrame.height() * 3 / 2, cloneFrame.width(), CV_8UC1, (void*)cloneFrame.bits(), cloneFrame.bytesPerLine());
                cv::cvtColor(mat, mat, cv::COLOR_YUV2BGRA_IYUV);
                emit presentFrame(mat);
            }
        }
    }
    return false;
}

Camera::Camera()
{
    qRegisterMetaType<cv::Mat>("cv::Mat");
}

Camera::~Camera()
{
}

void Camera::captureCamera()
{
    //识别动态插拔相机
    m_cameraTimer = new QTimer();
    connect(m_cameraTimer, SIGNAL(timeout()), this, SLOT(getCameraList()));
    m_cameraTimer->start(3000);
}

void Camera::initializeCamera(QCameraInfo info)
{
    UIBridge *uibridge = MainClass::getInstance()->m_UIBridge;
    //创建摄像头
    if(m_QCamera != nullptr){
        m_QCamera->stop();
    }
    m_QCamera = new QCamera(info);
    //创建摄像头实时帧回调class
    if(m_cameraCapture == nullptr){
        m_cameraCapture = new QtCameraCapture();
    }
    //设置摄像头图像长宽默认 1280*720 20fps
    m_QCamera->setViewfinder(m_cameraCapture);
    m_QCamera->setCaptureMode(QCamera::CaptureStillImage);
    //处理完摄像头一帧发送到nama中保存
    connect(m_cameraCapture, SIGNAL(presentFrame(const cv::Mat &)),
            uibridge, SLOT(getPresentFrame(const cv::Mat &)), Qt::QueuedConnection);
    m_QCamera->start();
    MainClass::getInstance()->m_nama->CameraChange();
    //切换上一次选中
    auto mapCS = m_mapCameraSet.find(info.description());
    if(mapCS != m_mapCameraSet.end()){
        uibridge->m_selectCameraSet = mapCS.value();
    }
    //获取摄像头支持的分辨率、帧率等参数
    getCameraSupportedSet();
}

void Camera::getCameraSupportedSet()
{
    float rationeed = float(16) / 9;
    m_cameraSetList.clear();
    UIBridge *uibridge = MainClass::getInstance()->m_UIBridge;
    QList<QCameraViewfinderSettings > ViewSets = m_QCamera->supportedViewfinderSettings();
    foreach (QCameraViewfinderSettings ViewSet, ViewSets) {
        QSize size = ViewSet.resolution();
        int width = size.width();
        int height = size.height();
        float ratio = float(width) / height;
        double rate = ViewSet.maximumFrameRate();
        QVideoFrame::PixelFormat format = ViewSet.pixelFormat();
        if(ratio == rationeed && width >= m_minWidth && rate >= m_minRate){
            QString set = QString::number(width) + "x" + QString::number(height) + " " + QString::number(int(rate)) + "fps";
            if(format == QVideoFrame::Format_YUYV){
                set += " YUYV";
            }else if(format == QVideoFrame::Format_Jpeg){
                set += " BGRA";
            }else if(format == QVideoFrame::Format_BGR24){
                set += " BGR";
            }else if(format == QVideoFrame::Format_YUV420P){
                set += " YUV420P";
            }
            if(!m_cameraSetList.contains(set)){
                m_cameraSetList.append(set);
            }
        }
    }
    emit MainClass::getInstance()->m_UIBridge->cameraSetListChanged();
}

void Camera::getCameraList()
{
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    if(availableCameras.size() > 0){
        for(int i = 0; i < availableCameras.size(); i++){
            QString cameraName = availableCameras.at(i).description();
            if(m_mapCameraSet.find(cameraName) == m_mapCameraSet.end()){
                m_cameraList.append(cameraName);
                m_mapCameraSet.insert(cameraName, 0);
                emit MainClass::getInstance()->m_UIBridge->cameraListChanged();
            }
        }
    }
}

void Camera::changeCamera(int index)
{
    QString cameraname = m_cameraList.at(index);
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    //比较设备名,替换选中摄像头
    for(int i = 0; i < availableCameras.size(); i++){
        if(cameraname == availableCameras.at(i).description()){
            if(m_arCamera != availableCameras.at(i)){
                m_arCamera = availableCameras.at(i);
                m_usefulCameraSetChange = false;
                initializeCamera(m_arCamera);
            }
        }
    }
}

void Camera::changeCameraSet(int index)
{
    QString set = m_cameraSetList.at(index);
    m_FrameWidth = set.mid(0,set.indexOf("x")).toInt();
    m_FrameHeight = set.mid(set.indexOf("x") + 1,set.indexOf(" ") - (set.indexOf("x") + 1)).toInt();
    int fps = set.mid(set.indexOf(" ") + 1,set.indexOf("fps") - (set.indexOf(" ") + 1)).toInt();
    QString format = set.mid(set.indexOf("fps") + 4);
    if(format == "YUYV"){
        m_cameraSet.setPixelFormat(QVideoFrame::Format_YUYV);
    }else if(format == "BGRA"){
        m_cameraSet.setPixelFormat(QVideoFrame::Format_Jpeg);
    }else if(format == "BGR"){
        m_cameraSet.setPixelFormat(QVideoFrame::Format_BGR24);
    }else if(format == "YUV420P"){
        m_cameraSet.setPixelFormat(QVideoFrame::Format_YUV420P);
    }
    m_cameraSet.setResolution(m_FrameWidth, m_FrameHeight);
    m_cameraSet.setMinimumFrameRate(fps);
    m_cameraSet.setMaximumFrameRate(fps);
    m_QCamera->setViewfinderSettings(m_cameraSet);
    MainClass::getInstance()->m_UIBridge->m_frameSize = set.mid(0, set.indexOf(" "));
    emit MainClass::getInstance()->m_UIBridge->frameSizeChanged();
    m_mapCameraSet.insert(m_arCamera.description(), index);
}
