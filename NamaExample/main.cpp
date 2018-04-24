
#include "NamaExample.h"
#include <QtWidgets/QApplication>
#include <QTextStream>
int main(int argc, char *argv[])
{
	ShowWindow(GetConsoleWindow(), 0);
	//将exe控制台打印信息重定向到nama_log.txt文件中
	freopen("nama_log.txt", "w", stdout);
	freopen("nama_log.txt", "w", stderr);
	
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
	QDesktopWidget* desktopWidget = QApplication::desktop();	
	QRect deskRect = desktopWidget->availableGeometry();
	
	if (deskRect.width() < 1920){
		printf("desktop width is %d \n", deskRect.width());
		float scale = deskRect.width() / 1920.0;
		QObjectList lst = w.children();
		QPoint pt(0, 0);
		for (auto l : lst)
		{
			auto widget = qobject_cast<QWidget*>(l);
			//auto widget = qobject_cast<QLayout*>(l);
			if (widget)
			{
				pt = widget->pos();

				widget->move(QPoint(pt.x() * scale, pt.y() * scale));
				widget->resize(widget->width() * scale, widget->height() * scale);

				QFont font;
				int nIndex = widget->metaObject()->indexOfProperty("font");
				auto propertyFont = widget->metaObject()->property(nIndex);

				QVariant var = propertyFont.read(widget);
				if (var.canConvert(QMetaType::QFont))
				{
					font = var.value<QFont>();

					font.setPointSizeF(font.pointSizeF() * scale);
					propertyFont.write(widget, QVariant::fromValue(font));
				}

				QPixmap pixmap;
				nIndex = widget->metaObject()->indexOfProperty("pixmap");
				auto pixmapProp = widget->metaObject()->property(nIndex);

				var = pixmapProp.read(widget);
				if (var.canConvert(QMetaType::QPixmap))
				{
					pixmap = var.value<QPixmap>();
					if (!pixmap.isNull())
					{
						QSize sizePixmap(pixmap.size().width() * scale, pixmap.size().height() * scale);

						pixmap = pixmap.scaled(sizePixmap);
						pixmapProp.write(widget, QVariant::fromValue(pixmap));
					}
				}
			}

		}

		w.resize(w.width() * scale, w.height() * scale);
	}
	
	return a.exec();
}
