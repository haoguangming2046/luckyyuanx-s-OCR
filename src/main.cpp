
#include <QtGui/QApplication>
#include <QTextCodec>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//�����������
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));
	 MainWindow main;
	main.show();
	return a.exec();
}
