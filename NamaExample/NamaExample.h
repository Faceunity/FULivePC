#pragma once


#include <QtWidgets/QMainWindow>

#include "ui_NamaExample.h"
class NamaExample : public QMainWindow
{
	Q_OBJECT

public:
	NamaExample(QWidget *parent = Q_NULLPTR);
	~NamaExample();
private:
	Ui::NamaExampleClass ui;
	QTimer *timer_show;
	QTimer *timer_log;
	int log_file_size;
public slots:
	void SetStatusBar();
	void on_SliderConnect_moved0();
	void on_comboBoxCurrentIndexChanged();
	void on_shapeSliderConnect_moved();
	void on_popLabelTimeOut();
	void on_virtualCameraCheckStateChanged();
	
	void on_pushButtonConnect_clicked0();
	void on_pushButtonConnect_clicked1();
	void on_pushButtonConnect_clicked2();
	void on_pushButtonConnect_clicked3();
	void on_pushButtonConnect_clicked4();
	void on_pushButtonConnect_clicked5();
	void on_pushButtonConnect_clicked6();
	void on_pushButtonConnect_clicked7();
	void on_pushButtonConnect_clicked8();
	void on_filterButtonConnect_clicked();
	void on_shapeButtonConnect_clicked();
};


