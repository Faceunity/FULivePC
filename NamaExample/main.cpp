
#include "NamaExample.h"
#include <QtWidgets/QApplication>
#include <QTextStream>
int main(int argc, char *argv[])
{
	//将exe控制台打印信息重定向到nama_log.txt文件中
	freopen("nama_log.txt", "w", stdout);
	
	QApplication a(argc, argv);
	QFile f(":qdarkstyle/style.qss");
	if (!f.exists())
	{
		printf("Unable to set stylesheet, file not found\n");
	}
	else
	{
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		a.setStyleSheet(ts.readAll());
		f.close();
	}
	
	NamaExample w;		
	w.show();
	return a.exec();
}
