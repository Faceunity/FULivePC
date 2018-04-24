#include "GLWidget.h"

GLWidget::GLWidget(QWidget *parent)
	: QGLWidget(parent)
{
	nama = std::tr1::shared_ptr<NamaExampleNameSpace::Nama>(new NamaExampleNameSpace::Nama);
	is_need_draw_landmarks = false;
	is_need_ipc_write = false;
	is_frame_null = false;

	if (!ipcBridge.open(IPC_FILE_NAME, IpcBridge::OpenMode::Write))
	{
		printf("open ipc file failed!");
	}
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
	nama->Init(wndWidth, wndHeight);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, wndWidth, 0, wndHeight, 0, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//这个texture只是为在avatar模式左上角显示预览窗口，不使用时可注释
	glGenTextures(1, &landmarks_textureID);
	glBindTexture(GL_TEXTURE_2D, landmarks_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	
}

int get_fps()
{
	static int fps = 0;
	static int lastTime = GetTickCount(); // ms
	static int frameCount = 0;

	++frameCount;

	int curTime = GetTickCount();
	if (curTime - lastTime > 1000) // 取固定时间间隔为1秒
	{		
		fps = frameCount;
		frameCount = 0;
		lastTime = curTime;
	}
	return fps;
}

void GLWidget::paintGL()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.5, 0.5, 0.0, 1.0);
	glDisable(GL_BLEND);

	fps = get_fps();	
	std::tr1::shared_ptr<unsigned char> frame = nama->QueryFrame();
	if (!frame)
	{
		is_frame_null = true;
		return;
	}
	nama->RenderItems(frame);
	if (is_need_ipc_write)
	{
		size_t frameSize = ipcBridge.write(MEDIASUBTYPE_RGB32, wndWidth, wndHeight, frame.get());
	}	
	setTextureData(frame);
	drawFrame();
	if (is_need_draw_landmarks)
	{
		//这里直接取了下一帧，如果要avatar数据完全吻合，可使用同一帧数据
		std::tr1::shared_ptr<unsigned char> second_frame = nama->QueryFrame();
		nama->DrawLandmarks(second_frame);
		setLandMarksTextureData(second_frame);
		drawLandMarks();
	}

	this->update();
}

void GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, 0, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void GLWidget::resizeEvent(QResizeEvent *size) 
{	
	if (size->size().width() == 0 || size->size().height() == 0)
		return;
	int width = size->size().width();
	int height = size->size().height();
	glViewport(0, 0, width, height);
};

void GLWidget::drawFrame()
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0, 0, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0, wndHeight, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(wndWidth, wndHeight, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(wndWidth, 0, 0.0f);
	glEnd();
}

void GLWidget::drawLandMarks()
{
	glBindTexture(GL_TEXTURE_2D, landmarks_textureID);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0, 0, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0, (float)wndHeight / 5.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f((float)wndWidth/4.0f, (float)wndHeight / 5.0f, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f((float)wndWidth / 4.0f, 0, 0.0f);
	glEnd();
}

void GLWidget::setTextureData(std::tr1::shared_ptr<unsigned char> frame)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wndWidth, wndHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, frame.get());
}

void GLWidget::setLandMarksTextureData(std::tr1::shared_ptr<unsigned char> frame)
{
	glBindTexture(GL_TEXTURE_2D, landmarks_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wndWidth, wndHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, frame.get());
}
