/**
* FU SDK使用者可以将拿到处理后的frame图像与自己的原有项目对接
* 请FU SDK使用者直接参考示例放至代码至对应位置
*
* 这里的界面设计在考虑后，使用的界面库是GLUI，如项目需要可更改为MFC或者WPF、QT等
*
* Created by liujia on 2017/3/24 mybbs2200@gmail.com
*/
#include "Nama.h"
#include <iostream>
#include <memory>

#include <string.h>
#include <gl\glui.h>
#include <windows.h>

#define GL_BGRA 0x80E1
#define PRE_BUNDLE_ID        200
#define NEXT_BUNDLE_ID		 201
#define NEXT_SHAPE_ID		 202
#define FILTER_INSTENSITY_ID 240
#define BEAUTY_INTENSITY_ID  250
#define ENABLE_ID            300
#define DISABLE_ID           301
#define SHOWPROP_ID          302
#define HIDE_ID              303
#define FPS_NUM_ID			 304

namespace NE
{
	int wndWidth = 1280;
	int wndHeight = 720;
	GLuint texID = 0;
	std::tr1::shared_ptr<Nama> nama;
	float timeSinceStart;
	float deltaTime;
	int frameRate = 25;
	float xy_aspect;
	/** These are the live variables passed into GLUI ***/
	int   wireframe = 0;
	bool m_isOptionPanelEnable = false;
	int   main_window;
	GLUI *glui, *glui2;
	GLUI_Spinner    *fps_spinner;
	GLUI_RadioGroup *radio;
	GLUI_Panel      *options_panel;
	GLUI_Rollout *beautification;
	GLUI_Rollout* prop_rollout;

	void setOpenGLState();
	void genTexture();
	void setTextureData(std::tr1::shared_ptr<unsigned char> frame);
	void drawFrame();

	void control_cb(int control);
	void namaGlutKeyboard(unsigned char Key, int x, int y);
	void namaGlutMenu(int value);
	void namaGlutIdle(void);
	void namaGlutMotion(int x, int y);
	void namaGlutReshape(int x, int y);
	void namaGlutMouse(int button, int button_state, int x, int y);
	void namaGlutDisplay(void);
	void createUI(int window);
}

int main(int argc, char* argv[])
{
	using namespace NE;
	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	glutInit(&argc, argv);
	timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(250, 150);
	glutInitWindowSize(wndWidth + 200, wndHeight);
	main_window = glutCreateWindow("NamaExample");
	const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字  
	const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台  
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION); //返回当前OpenGL实现的版本号  
	const GLubyte* gluVersion = gluGetString(GLU_VERSION); //返回当前GLU工具库版本  

	printf("OpenGL实现厂商的名字：%s\n", name);
	printf("渲染器标识符：%s\n", biaoshifu);
	printf("OpenGL实现的版本号：%s\n", OpenGLVersion);
	printf("OGLU工具库版本：%s\n", gluVersion);
	nama = std::tr1::shared_ptr<Nama>(new Nama);
	nama->Init(wndWidth, wndHeight);
	setOpenGLState();
	genTexture();
	//因为UI数据绑定使用了nama里的成员变量，所以是先初始化nama后创建UI
	createUI(main_window);

	return EXIT_SUCCESS;
}

void NE::control_cb(int control)
{
	if (control == PRE_BUNDLE_ID) {
		nama->PreBundle();
	}
	else if (control == NEXT_BUNDLE_ID) {
		nama->NextBundle();
	}
	else if (control == NEXT_SHAPE_ID)
	{
		nama->NextShape();
	}
	else if (control == BEAUTY_INTENSITY_ID) {
		nama->UpdateBeauty();
	}
	else if (control == FILTER_INSTENSITY_ID) {
		nama->UpdateFilter();
	}
	else if (control == ENABLE_ID)
	{
		nama->SwitchRenderMode();
		if (m_isOptionPanelEnable)
		{
			options_panel->disable();
			beautification->enable();
			prop_rollout->enable();
			m_isOptionPanelEnable = false;
		}
		else
		{
			options_panel->enable();
			beautification->disable();
			prop_rollout->disable();
			m_isOptionPanelEnable = true;
		}
	}
}

void NE::namaGlutKeyboard(unsigned char Key, int x, int y)
{
	switch (Key)
	{
	case 27:
	case 'q':
		exit(0);
		break;
	};

	glutPostRedisplay();
}

void NE::namaGlutMenu(int value)
{
	namaGlutKeyboard(value, 0, 0);
}

void NE::namaGlutIdle(void)
{
	/* According to the GLUT specification, the current window is
	undefined during an idle callback.  So we need to explicitly change
	it if necessary */
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	GLUI_Master.sync_live_all();
	glutPostRedisplay();
}

void NE::namaGlutMouse(int button, int button_state, int x, int y)
{
}

void NE::namaGlutMotion(int x, int y)
{
	glutPostRedisplay();
}

void NE::namaGlutReshape(int x, int y)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);
	xy_aspect = (float)tw / (float)th;

	glutPostRedisplay();
}

void NE::namaGlutDisplay(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	
	std::tr1::shared_ptr<unsigned char> frame = nama->Render();
	//std::tr1::shared_ptr<unsigned char> frame = nama->RenderEx();	
	setTextureData(frame);
	drawFrame();
	glutSwapBuffers();

	deltaTime = GetTickCount() - timeSinceStart;
	timeSinceStart = GetTickCount();
	
	float frameTime = floor(1000.0 / frameRate);
	if (deltaTime < frameTime)
	{		
		Sleep((frameTime)-deltaTime);
	}
}

void NE::createUI(int window)
{
	glutDisplayFunc(namaGlutDisplay);
	GLUI_Master.set_glutReshapeFunc(namaGlutReshape);
	GLUI_Master.set_glutKeyboardFunc(namaGlutKeyboard);
	GLUI_Master.set_glutSpecialFunc(NULL);
	GLUI_Master.set_glutMouseFunc(namaGlutMouse);
	glutMotionFunc(namaGlutMotion);

	glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);
	prop_rollout = new GLUI_Rollout(glui, "Effects                      ", true);
	new GLUI_Checkbox(prop_rollout, "Enabled", (int*)&nama->m_isDrawProp, SHOWPROP_ID, control_cb);
	new GLUI_Button(prop_rollout, "Pre Prop ", PRE_BUNDLE_ID, control_cb);
	new GLUI_Button(prop_rollout, "Next Prop ", NEXT_BUNDLE_ID, control_cb);
	beautification = new GLUI_Rollout(glui, "Beautification", true);
	GLUI_Panel *beauty = new GLUI_Panel(beautification, "Beauty");
	GLUI_Panel *filter = new GLUI_Panel(beautification, "Filter");

	new GLUI_Checkbox(beauty, "Enabled", (int*)&nama->m_isBeautyOn);
	GLUI_Scrollbar *sb;
	new GLUI_StaticText(beauty, "Color level:");
	sb = new GLUI_Scrollbar(beauty, "color_level", GLUI_SCROLL_HORIZONTAL, &nama->m_curColorLevel, BEAUTY_INTENSITY_ID, control_cb);
	sb->set_float_limits(0, 1.0f);
	new GLUI_StaticText(beauty, "Blur level:");
	sb = new GLUI_Scrollbar(beauty, "blur_level", GLUI_SCROLL_HORIZONTAL, &nama->m_curBlurLevel, BEAUTY_INTENSITY_ID, control_cb);
	sb->set_float_limits(0.1f, 6.0f);
	new GLUI_StaticText(beauty, "Red level:");
	sb = new GLUI_Scrollbar(beauty, "red_level", GLUI_SCROLL_HORIZONTAL, &nama->m_redLevel, BEAUTY_INTENSITY_ID, control_cb);
	sb->set_float_limits(0, 1.0f);
	new GLUI_StaticText(beauty, "Cheek thinning:");
	sb = new GLUI_Scrollbar(beauty, "cheek_thinning", GLUI_SCROLL_HORIZONTAL, &nama->m_curCheekThinning, BEAUTY_INTENSITY_ID, control_cb);
	sb->set_float_limits(0, 1.0f);
	new GLUI_StaticText(beauty, "Eye enlargingor level:");
	sb = new GLUI_Scrollbar(beauty, "eye_enlarging", GLUI_SCROLL_HORIZONTAL, &nama->m_curEyeEnlarging, BEAUTY_INTENSITY_ID, control_cb);
	sb->set_float_limits(0, 1.0f);
	new GLUI_Button(beauty, "Change Shape ", NEXT_SHAPE_ID, control_cb);
	new GLUI_StaticText(beauty, "Face shape level:");
	sb = new GLUI_Scrollbar(beauty, "face_shape_level", GLUI_SCROLL_HORIZONTAL, &nama->m_faceShapeLevel, BEAUTY_INTENSITY_ID, control_cb);
	sb->set_float_limits(0, 1.0f);

	/**** 滤镜参数控制 ****/
	GLUI_Panel *filter_panel = new GLUI_Panel(filter, "Filter Type");
	filter_panel->set_alignment(GLUI_ALIGN_LEFT);
	radio = new GLUI_RadioGroup(filter_panel, &nama->m_curFilterIdx, FILTER_INSTENSITY_ID, control_cb);
	new GLUI_RadioButton(radio, "nature");
	new GLUI_RadioButton(radio, "delta");
	new GLUI_RadioButton(radio, "electric");
	new GLUI_RadioButton(radio, "slowlived");
	new GLUI_RadioButton(radio, "tokyo");
	new GLUI_RadioButton(radio, "warm");
	new GLUI_StaticText(filter, "");

	new GLUI_Button(glui, "Switch Render Mode", ENABLE_ID, control_cb);
	options_panel = new GLUI_Rollout(glui, "Display Options              ", true);
	options_panel->disable();
	new GLUI_Checkbox(options_panel, "Feature Points", &nama->m_isDrawPoints, 1, control_cb);
	new GLUI_StaticText(glui, "");
	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	/**** 连接窗口和GLUI,注册空闲回调函数 ******/
	glui->set_main_gfx_window(main_window);

	/*** 创建底层子窗口 ***/
	glui2 = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_BOTTOM);
	glui2->set_main_gfx_window(main_window);
	GLUI_StaticText* state = new GLUI_StaticText(glui2, "Status: OK");
	state->set_alignment(GLUI_ALIGN_LEFT);
	state->set_w(50);
	new GLUI_Column(glui2, true);
	fps_spinner = new GLUI_Spinner(glui2, "FPS:", &frameRate);
	fps_spinner->set_float_limits(25.0f, 60.0f);
	new GLUI_Column(glui2, true);
	GLUI_EditText *counter_edittext = new GLUI_EditText(glui2, "ColorLevel:", &nama->m_curColorLevel);
	new GLUI_Column(glui2, false);
	GLUI_EditText *counter_edittext1 = new GLUI_EditText(glui2, "BlurLevel:", &nama->m_curBlurLevel);
	new GLUI_Column(glui2, false);
	GLUI_EditText *counter_edittext2 = new GLUI_EditText(glui2, "CheekThinning:", &nama->m_curCheekThinning);
	new GLUI_Column(glui2, false);
	GLUI_EditText *counter_edittext3 = new GLUI_EditText(glui2, "EyeEnlarging:", &nama->m_curEyeEnlarging);
	new GLUI_Column(glui2, true);
	//GLUI_EditText *counter_edittext4 = new GLUI_EditText(glui2, "Translation:", nama->m_curTranslation);
	//counter_edittext4->set_w(230);
	//new GLUI_Column(glui2, false);
	//GLUI_EditText *counter_edittext5 = new GLUI_EditText(glui2, "Rotation:", &nama->m_curRotation);
	//counter_edittext5->set_w(330);
	//new GLUI_Column(glui2, true);
	glui2->disable();
	fps_spinner->enable();
#if 1
	/**** We register the idle callback with GLUI, *not* with GLUT ****/
	GLUI_Master.set_glutIdleFunc(namaGlutIdle);
#endif
	int err = glGetError();
	glutMainLoop();
}

void NE::setOpenGLState()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, wndWidth, 0, wndHeight, 0, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void NE::genTexture()
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void NE::setTextureData(std::tr1::shared_ptr<unsigned char> frame)
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wndWidth, wndHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, frame.get());
}

void NE::drawFrame()
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1, -1, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1, 1, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1, 1, 0);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1, -1, 0.0f);
	glEnd();
}
