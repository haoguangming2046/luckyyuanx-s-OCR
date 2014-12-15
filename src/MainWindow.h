#ifndef MAINWINDOW_H_
#define MIAINWINDOW_H_

#include <QtGui/QMainWindow>

class QListWidget;
class QMenuBar;
class DataBase;
class QStackedWidget;
class QueryPage;
class ScanPage;
class AuthPage;
class SysconfPage;

class MainWindow : public QMainWindow
{
		Q_OBJECT

public:
		MainWindow();
		~MainWindow();

		public slots:
				void open();
				void save();
				void scan();
				void imageProcess();
				void loadMasks();
				void recognizeText();
				void auth();
				void clearMasks();

private:
		void buildUI();
		//action
		QAction* actionOpen;
		QAction* actionSave;
		QAction* actionScan;
		QAction* actionExit;
		QAction* actionProcessImage;
		QAction* actionRecognize;
		QAction* actionAuth;
		QAction* actionLoadMasks;
		QAction* actionClearMasks;

		//controls
		QListWidget* listWidget;
		QStackedWidget* stackedWidget;
		QueryPage* queryPage;
		ScanPage* scanPage;
		AuthPage* authPage;
		SysconfPage* sysconfPage;

		//internal
		
};

#endif