#ifndef NAMA_H
#define NAMA_H

#include "FuController.h"
#include "MP3.h"
#include <map>

using namespace std;
//#define SynchronizingCamera   //打开则同步摄像头帧率,适配低端机,开启降低cpu,gpu占有率,但是画面可能有延迟

enum BundleCategory
{
    ItemJingpin,            //精品
    ItemSticker,            //道具贴图
    StyleRecommendation,    //风格推荐
    Makeup,                 //美妆
    LightMakeup,            //轻美妆
    BackgroundSegmentation, //人像分割
    GestureRecognition,     //手势识别
    ExpressionRecognition,  //表情识别
    Animoji,                //动物
    Avatar,                 //虚拟人
    BeautyHair,             //美发
    ARMask,                 //AR面具
    MusicFilter,            //音乐滤镜
    MagicMirror,            //哈哈镜
    BigHead,                //搞笑大头
    GreenScreen,            //绿幕
    SafeArea,               //绿幕安全区域
    Count
};


enum RenderType{
    RENDER_BODY,
    RENDER_AR,
    RENDER_GREEN,
    RENDER_PICKCOLOR
};


class Nama{
public:
    Nama();
    ~Nama();
public:
    //avator类
    std::shared_ptr<FuController> m_Controller;
    //视频帧Id
    int m_FrameID = 0;
    //纹理Id
    unsigned int m_texID = 0;
    //实时图像帧
    cv::Mat m_frame;
    //图像帧更新锁
    std::mutex m_frameMutex;
    //记录图像长,长度变化需要调用函数
    int m_FrameWidth = 0;
    //是否获取到新的帧,同步摄像头帧率,为提高帧率,如果摄像头输入图像没更新,仍用前一个texture
    bool m_getNewFrame = true;
    //渲染道具类型
    RenderType m_renderType;
    //证书权限码
    int m_ModuleCode, m_ModuleCode1;
    //nama是否注册成功
    bool m_EnableNama = false;
    //记录handle id
    int m_BeautyHandles, m_MakeUpHandle, m_BodyShapeHandle = 0, m_GSHandle = 0, m_FxaaHandles = 0;
    //最多识别人脸数
    int m_maxFace = 1;
    //当前调用的模型类别
    BundleCategory m_bundleCategory = Count;
    //当前调用道具bundle
    int m_bundleCurrent = 0;
    //道具Bundle列表,道具名,道具nama中id
    //    std::map<std::string, int> m_BundlesMap;
    //播放音乐
    Mp3 *m_mp3 = nullptr;
    //开始播放音乐
    bool b_start_mp3 = false;
    //美妆道具id,切换美妆时取消绑定
    int m_curMakeupItem = 0;
    //自定义美妆道具,string存道具类型,int存道具id
    map<string, int> m_mapMakeUpItem;
    //渲染道具数组
    vector<int> m_renderList;
    //avatar调用到的bundle
    std::vector<std::string> avatarBundles;
    //打开虚拟摄像头
    bool m_bVirturalCamera = false;
    bool m_bIsCreateVirturalCamera = false;
public:
    void InitOpenGL();
    bool CheckGLContext();
    // 加载bundle
    // @param filepath 相对路径 ,macosx 上为相对于 Resource.bundle 文件
    // @param data 加载成功后返回的数据
    static bool LoadBundle(const string& filepath, vector<char>& data);
    // 从路径中提取路径包含的序号，仅适用于 *_05.*类型的路径，
    // 例如 items/BeautyHair/hair_normal_05.bundle =》 05
    int extractIntFromPath(string path);
    //初始化nama,需要和fuRender处理图像在同一个线程
    void InitNama();
    //改变render数组
    void changeRenderList(RenderType type,bool makeupFlag = true);
    //调用nama处理图像
    void RenderDefNama();
    //初始化avatar
    bool InitController();
    //avatar加入动作识别
    bool LoadAvatarHandTrackBundle();
    //创建avatar道具
    void LoadAvatarBundles(const std::vector<std::string>& bundleNames);
    //销毁avatar
    void UnLoadAvatar();
    //调用nama设置string道具
    bool itemSetParamd(int handles, string name, double value);
    //调用nama设置double道具
    bool itemSetParams(int handles, string name, string value);
    //精品道具切美妆重新加载道具
    void ReloadItems();
    //加载美妆
    void LoadMakeup();
    //卸载美妆
    void UnLoadMakeup();
    //清除所有自定义美妆
    void ClearAllCM();
    //nama销毁道具
    void DestroyAll();
    //选中一个道具
    bool SelectBundle(string bundleName,int person = 0,bool bindFlag = true);
    //改变清除美妆flag
    void ChangeCleanFlag(bool bOpen);
    //将图片转换成avatar形象
    void RenderP2A();
    //将图片先美颜再转avatar
    void RenderBear();
    //设置Avatar类型
    void SetBodyTrackType(BodyTrackType type);
    //设置绿幕背景色
    void SetGSKeyColor(cv::Vec4b data);
    //改变绿幕相机输入图像位置
    void changeGSPreviewRect(double startX, double startY, double endX, double endY);
    //美妆设置颜色 name道具名
    void setMakeUpColor(string name,std::vector<double> color0,std::vector<double> color1, std::vector<double> color2);
    //美妆增加道具 name是道具种类名(腮红) item是道具名(mu_style_blush_01) type眉毛和口红特有,设置眉毛变形类型brow_warp_type(新接入方式后不需要设置，替换道具即可),口红类型lip_type,form0口红
    void appendMakeUpItems(string name, string item, int type = -1, int form = -1);
    //美妆绑定道具
    void bindMakeUp(string name, string item);
    //精品道具,马赛克道具点击触发
    void itemJingpinClick();
    //从摄像头读取到一帧,保存一个实时frame
    void getPresentFrame(const cv::Mat &frame);
    //更新绿幕背景
    void UpdateGreenScreenSegment(const cv::Mat & dataRGBA);
    //更新背景分割背景
    void UpdateBackgroundSegment(const cv::Mat & dataRGBA);
    //更新绿幕安全区域
    void UpdateGreenScreenSafeArea(const cv::Mat & dataRGBA);
    void UpdateGreenScreenSafeArea(int texid);
    //不使用绿幕安全区域
    void NonuseGreenScreenSafeArea();
    //更新avator参数
    void ApplyBodyTrackConfig(BodyTrackParam params);
    //改变摄像头
    void CameraChange();
};

#endif // NAMA_H
