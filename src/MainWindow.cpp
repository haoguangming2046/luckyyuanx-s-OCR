#include "MainWindow.h"
#include "DataBase.h"
#include "Share/ImageProcess.h"
#include "Share/TesseractOCR.h"
#include "Share/ImageAdapter.h"
#include "Share/AppInfo.h"
#include "Share/AbbyyOCR.h"
#include "twain/qtwain.h"
#include "twain/dib.h"

#include "QueryPage.h"
#include "ScanPage.h"
#include "ImageWidget.h"
#include "SysconfPage.h"
#include "AuthPage.h"

#include <QMenuBar>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>

MainWindow ::MainWindow()
{
		DataBase::createConnection();
		DataBase::createDatabase();
		buildUI();
		

}

MainWindow ::~MainWindow()
{
		if(queryPage)
				delete queryPage;
		if(scanPage)
				delete scanPage;
		if(authPage)
				delete authPage;
		if(sysconfPage)
				delete sysconfPage;
}

void MainWindow :: buildUI()
{
		//build action
		actionOpen = new QAction(tr("打开"), this);
		actionSave = new QAction(tr("保存"), this);
		actionScan = new QAction(tr("扫描"), this);
		actionProcessImage = new QAction(tr("图片处理"), this);
		actionLoadMasks = new QAction(tr("导入模板"), this);
		actionClearMasks = new QAction(tr("清除模板"), this);
		actionRecognize = new QAction(tr("文本识别"), this);
		actionAuth = new QAction(tr("认证"), this);
		actionExit = new QAction(tr("退出"), this);

		actionOpen->setIcon(QIcon(":/open.png"));
		actionSave->setIcon(QIcon(":/save.png"));
		actionScan->setIcon(QIcon(":/scan.png"));
		actionLoadMasks->setIcon(QIcon(":/load2.png"));
		actionClearMasks->setIcon(QIcon(":/save2.png"));
		actionProcessImage->setIcon(QIcon(":/paint.png"));
		actionRecognize->setIcon(QIcon(":/ocr.png"));
		actionAuth->setIcon(QIcon(":/rgb.png"));

		//build controls
		queryPage = new QueryPage;
		scanPage = new ScanPage;
		authPage = new AuthPage;
		sysconfPage = new SysconfPage;

		stackedWidget = new QStackedWidget(this);
		stackedWidget->addWidget(scanPage);
		stackedWidget->addWidget(authPage);
		stackedWidget->addWidget(queryPage);
		stackedWidget->addWidget(sysconfPage);
		stackedWidget->setCurrentIndex(0);

		listWidget = new QListWidget;
		listWidget->addItem(tr("扫描/录入"));
		listWidget->addItem(tr("实时认证"));
		listWidget->addItem(tr("详细查询"));
		//listWidget->addItem(tr("下载/打印"));				//暂时去掉
		listWidget->addItem(tr("系统设置"));
		//listWidget->addItem(tr("退出"));
		listWidget->setMidLineWidth(40);
		listWidget->setMaximumWidth(160);
		//build centralWidget
		QWidget* centralWidget = new QWidget;
		this->setCentralWidget(centralWidget);
		//layout
		QHBoxLayout* mainlayout = new QHBoxLayout;
		mainlayout->addWidget(listWidget);
		mainlayout->addWidget(stackedWidget);
		centralWidget->setLayout(mainlayout);
		//build menu
		QMenu* File = menuBar()->addMenu(tr("文件"));
		File->addAction(actionOpen);
		File->addAction(actionSave);
		File->addAction(actionExit);
		menuBar()->addMenu(tr("扫描/认证"));
		menuBar()->addMenu(tr("查询"));
		menuBar()->addMenu(tr("系统设置"));
		menuBar()->addMenu(tr("帮助"));
		//build toolbar
		QToolBar* toolBar = addToolBar(tr("mainToolBar"));
		toolBar->addAction(actionScan);
		toolBar->addAction(actionOpen);
		toolBar->addAction(actionSave);
		toolBar->addAction(actionLoadMasks);
		toolBar->addAction(actionClearMasks);
		toolBar->addAction(actionProcessImage);
		toolBar->addAction(actionRecognize);
		toolBar->addAction(actionAuth);

		//set title
		setWindowTitle(tr("专用发票网上认证系统"));
		//connect
		connect(listWidget, SIGNAL(currentRowChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));
		connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
		connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
		connect(actionScan, SIGNAL(triggered()), this, SLOT(scan()));
		connect(actionLoadMasks, SIGNAL(triggered()), this, SLOT(loadMasks()));
		connect(actionClearMasks, SIGNAL(triggered()), this, SLOT(clearMasks()));
		connect(actionProcessImage, SIGNAL(triggered()), this, SLOT(imageProcess()));
		connect(actionRecognize, SIGNAL(triggered()), this, SLOT(recognizeText()));
		connect(actionAuth, SIGNAL(triggered()), this, SLOT(auth()));
}

void MainWindow :: open()
{
		scanPage->clearMasks();
		QString fileName = QFileDialog::getOpenFileName(this,
				tr("Open Image"), AppInfo::instance()->appDir().c_str(),
				tr("Image files (*.png *.jpg *.bmp *.tiff)"));
		if (!fileName.isEmpty())
		{
				IplImage* cvImage = cvLoadImage(fileName.toLocal8Bit().constData());
				if(cvImage)
				{
						scanPage->getImageProcessPtr()->setImage(cvImage);
						QImage* qtImage= ImageAdapter::IplImage2QImage(cvImage);
						scanPage->getImageWidgetPtr()->setImage(qtImage);
				}
		}
}

void MainWindow :: save()
{
		QString fileName = QFileDialog::getSaveFileName(this,
				tr("Save Image"), AppInfo::instance()->appDir().c_str(),
				tr("Image files (*.png *.jpg *.bmp *.tiff)"));
		if (!fileName.isEmpty())
		{
				IplImage* cvImage = scanPage->getImageProcessPtr()->getProcessedImage();
				if(cvImage)
				{
						cvSaveImage(fileName.toLocal8Bit().constData(), cvImage);
				}
		}
}

void MainWindow ::scan()
{
		scanPage->scanImage();
}

void MainWindow::imageProcess()
{
		scanPage->processImage();
}

void MainWindow::loadMasks()
{
		scanPage->loadMasks();


}

void MainWindow::clearMasks()
{
		scanPage->clearMasks();
}

void MainWindow::recognizeText()
{
		scanPage->recognizeText();
}

void MainWindow::auth()
{
		//scanPage->on_pbtnAuth_clicked();
}
