#include "NamaExample.h"

NamaExample::NamaExample(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);	
	int count = ui.glwidget->nama->CameraCount();
	for (int i = 0; i < count; i++)
	{
		char temp[1024];
		int buffer_size = 1024;
		ui.glwidget->nama->CameraName(i, temp, buffer_size);
		QString item_name(temp);
		ui.comboBox->addItem(item_name);
	}
	ui.poplabel_0->hide();
	ui.statusBar->addPermanentWidget(ui.official_website_label);
	timer_show = new QTimer(this);
	timer_log = new QTimer(this);
	timer_fps = new QTimer(this);
	connect(timer_show, SIGNAL(timeout()), this, SLOT(on_popLabelTimeOut()));	
	connect(timer_log, SIGNAL(timeout()), this, SLOT(SetStatusBar()));	
	connect(timer_fps, SIGNAL(timeout()), this, SLOT(SetFPSLaybel()));
	timer_log->start(1000);
	timer_fps->start(1000);
	log_file_size = 0;
	
}

void NamaExample::SetStatusBar()
{
	if (ui.glwidget->nama->IsTracking())
	{
		ui.poplabel_0->hide();
	}
	else
	{
		ui.poplabel_0->setText(QStringLiteral("Î´Ê¶±ð"));
		ui.poplabel_0->show();
	}
	//
	QFile f("nama_log.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	
	if (f.size() != log_file_size)
	{
		QTextStream ts(&f);
		QString s;
		while (!ts.atEnd())
		{
			s = ts.readLine();			
		}	
		ui.statusBar->showMessage(s, 1000);
		log_file_size = f.size();
	}	
	f.close();
}

void NamaExample::SetFPSLaybel()
{	
	ui.label_fps->setText(QString(std::to_string(ui.glwidget->fps).c_str()));
}

void NamaExample::on_comboBoxCurrentIndexChanged()
{
	QComboBox  *box = qobject_cast<QComboBox *>(sender());
	ui.glwidget->nama->m_curCameraIdx = box->currentIndex();
	ui.glwidget->nama->ReOpenCamera();
}

NamaExample::~NamaExample()
{
	fclose(stdout);
}

void NamaExample::on_SliderConnect_moved0()
{
	QSlider *silder = qobject_cast<QSlider*>(sender());
	float current_value = (float)silder->value() / 100.0f;
	QString name = silder->objectName();
	int index = name.split("_")[1].toInt();
	switch (index)
	{
	case 0:
		ui.glwidget->nama->m_curBlurLevel = current_value;
		break;
	case 1:
		ui.glwidget->nama->m_curColorLevel = current_value;
		break;
	case 2:
		ui.glwidget->nama->m_redLevel = current_value;
		break;
	default:
		break;
	}
	
	ui.glwidget->nama->UpdateBeauty();
}

void NamaExample::on_shapeSliderConnect_moved()
{
	QSlider *silder = qobject_cast<QSlider*>(sender());
	float current_value = (float)silder->value() / 100.0f;
	QString name = silder->objectName();
	int index = name.split("_")[1].toInt();
	switch (index)
	{
	case 0:
		ui.glwidget->nama->m_faceShapeLevel = current_value;
		break;
	case 1:
		ui.glwidget->nama->m_curEyeEnlarging = current_value;
		break;
	case 2:
		ui.glwidget->nama->m_curCheekThinning = current_value;
		break;
	default:
		break;
	}

	ui.glwidget->nama->UpdateBeauty();
}

void NamaExample::on_popLabelTimeOut()
{
	ui.poplabel_0->hide();
}

void NamaExample::on_virtualCameraCheckStateChanged()
{
	QCheckBox *checkBox = qobject_cast<QCheckBox*>(sender());
	ui.glwidget->is_need_ipc_write = checkBox->isChecked();
}

void NamaExample::on_chooseCheckStateChanged()
{
	QRadioButton *checkBox = qobject_cast<QRadioButton*>(sender());
	QString name = checkBox->objectName();
	std::string sp = name.split("_")[1].toStdString();
	if (sp == "tiezhi")
	{
		ui.glwidget->nama->m_isDrawProp = checkBox->isChecked();
	}
	else if(sp == "lvjing")
	{
		ui.glwidget->nama->m_isBeautyOn = checkBox->isChecked();
	}
	else if (sp == "meiyan")
	{
		ui.glwidget->nama->m_isBeautyOn = checkBox->isChecked();
	}

	
}

void NamaExample::on_pushButtonConnect_clicked0()
{
	ui.poplabel_0->hide();
	ui.glwidget->nama->m_maxFace = 4;
	ui.glwidget->nama->SetCurrentBundle(0);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_pushButtonConnect_clicked1()
{
	ui.poplabel_0->hide();
	ui.glwidget->nama->m_maxFace = 4;
	ui.glwidget->nama->SetCurrentBundle(1);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_pushButtonConnect_clicked2()
{
	ui.glwidget->nama->m_maxFace = 4;
	ui.poplabel_0->setText(QStringLiteral("Çëà½×ì"));
	ui.poplabel_0->show();	
	timer_show->start(2000);
	timer_show->setSingleShot(true);
	ui.glwidget->nama->SetCurrentBundle(2);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_pushButtonConnect_clicked3()
{
	ui.glwidget->nama->m_maxFace = 4;
	ui.poplabel_0->hide();
	ui.glwidget->nama->SetCurrentBundle(3);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_pushButtonConnect_clicked4()
{
	ui.glwidget->nama->m_maxFace = 4;
	ui.poplabel_0->hide();
	ui.glwidget->nama->SetCurrentBundle(4);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_pushButtonConnect_clicked5()
{
	ui.glwidget->nama->m_maxFace = 1;
	ui.poplabel_0->hide();
	ui.glwidget->nama->SetCurrentBundle(5);
	ui.glwidget->is_need_draw_landmarks = true;
}

void NamaExample::on_pushButtonConnect_clicked6()
{
	ui.glwidget->nama->m_maxFace = 4;
	ui.poplabel_0->setText(QStringLiteral("ÇëÎ¢Ð¦"));
	ui.poplabel_0->show();
	timer_show->start(2000);
	timer_show->setSingleShot(true);
	ui.glwidget->nama->SetCurrentBundle(6);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_pushButtonConnect_clicked7()
{
	ui.glwidget->nama->m_maxFace = 4;
	ui.poplabel_0->hide();
	ui.glwidget->nama->SetCurrentBundle(7);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_pushButtonConnect_clicked8()
{
	ui.glwidget->nama->m_maxFace = 4;
	ui.poplabel_0->hide();
	ui.glwidget->nama->SetCurrentBundle(8);
	ui.glwidget->is_need_draw_landmarks = false;
}

void NamaExample::on_filterButtonConnect_clicked()
{
#define START_OF_FILTER_BUTTON 9
	QPushButton* btn = qobject_cast<QPushButton*>(sender());
	QString name = btn->objectName();
	int index = name.split("_")[1].toInt()- START_OF_FILTER_BUTTON;

	ui.glwidget->nama->m_curFilterIdx = index;
	ui.glwidget->nama->UpdateFilter();
}

void NamaExample::on_shapeButtonConnect_clicked()
{
	QRadioButton* btn = qobject_cast<QRadioButton*>(sender());
	QString name = btn->objectName();
	int index = name.split("_")[1].toInt();
	
	ui.glwidget->nama->SetCurrentShape(index);
}
