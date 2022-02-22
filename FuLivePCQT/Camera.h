#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <QCamera>
#include <QTimer>
#include <QCameraInfo>

//这是一个摄像头获取实时帧或绿幕背景视频帧的类
class QtCameraCapture : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QtCameraCapture(bool video = false,QObject *parent = 0);
    //这个虚函数必须实现,否则无法创建
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    bool present(const QVideoFrame &frame) override;
    //true为绿幕背景视频,false为摄像头图像
    bool m_bVideo;
signals:
    void presentFrame(const cv::Mat &frame);
    //更新绿幕背景或背景分割背景
    void presentGSBGFrame(const cv::Mat &frame);
};

//相机管理类
class Camera : public QObject
{
    Q_OBJECT
public:
    Camera();
    ~Camera();
    QCamera         *m_QCamera = nullptr;
    QTimer          *m_cameraTimer = nullptr;
    QtCameraCapture *m_cameraCapture = nullptr;
    QCameraViewfinderSettings m_cameraSet;
    int m_FrameWidth = 1280, m_FrameHeight = 720;
    //设置获取相机可设置比例过滤最小宽度值
    int m_minWidth = 640;
    //最小帧率
    int m_minRate = 20;
    //选中相机
    QCameraInfo      m_arCamera;
    //相机列表
    QStringList      m_cameraList;
    //摄像头分辨率刷新率显示列表
    QStringList      m_cameraSetList;
    //各个相机选中哪一个相机参数
    QMap<QString, int> m_mapCameraSet;
    //有用的摄像头参数改变
    bool  m_usefulCameraSetChange = false;
public:
    //获取摄像头
    void captureCamera();
    //初始化选择摄像头
    void initializeCamera(QCameraInfo info = QCameraInfo::defaultCamera());
    //遍历摄像头获取符合的分辨率,帧率
    void getCameraSupportedSet();
public slots:
    //获取摄像头列表
    void getCameraList();
    //改变选中摄像头
    void changeCamera(int index);
    //改变摄像头参数
    void changeCameraSet(int index);
};

#endif

