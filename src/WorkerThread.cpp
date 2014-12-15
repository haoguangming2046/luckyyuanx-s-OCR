#include "WorkerThread.h"
#include "Globel.h"
#include "ParamWidget.h"
#include "Share/ImageProcess.h"
#include "Share/TesseractOCR.h"
#include "Share/ImageAdapter.h"
#include "Share/AppInfo.h"
#include "Share/AbbyyOCR.h"
#include "twain/qtwain.h"
#include "twain/dib.h"
#include "twain/dibutil.h"

#include <QSqlQuery>
#include <QRect>

std::vector<BillInfo*> vBillInfo;//假设一次最多扫100张
//const ParamWidget* paramWidget = new ParamWidget();
extern ImageProcessParam param;

/**
 * 从消息队列中获取图片
 */
QString getPicture()
{
		mutex.lock();
		if(MsgQueue.isEmpty())
				waitConsume.wait(&mutex);
		QString strfilename = MsgQueue.dequeue();
		waitProduce.wakeAll();
		mutex.unlock();

		return strfilename;
}

/**
 * 获取模板
 */
QString getModeDir()
{
		QSqlQuery* query = new QSqlQuery;
		query->exec("select * from systemparams where key = 'modedir' ");
		query->next();
		
		return query->value(1).toString();
}

/**
 * 对图片进行处理
 */
void processImage(ImageProcess *mImageProcess, std::vector<OCRMask>* mOCRMasks, BillInfo& mBillInfo)
{
		//ImageProcess *mImageProcess = new ImageProcess();
		//ParamWidget* paramWidget = new ParamWidget();

		QString fileName = getPicture();
		mBillInfo.fileName = fileName;

		IplImage* cvImage = cvLoadImage(fileName.toLocal8Bit().data());
		if(cvImage)
		{
				mImageProcess->setImage(cvImage);
				//QImage* qtImage= ImageAdapter::IplImage2QImage(cvImage);
				//imageWidget->setImage(qtImage); 
				//mBillInfo.fileName = *qtImage;//保存图片，不知道会不会有问题
		}

		if(mImageProcess->getOriginalImage() == NULL)
		{
				QMessageBox msgBox;
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setText(QObject::tr("No image loaded.\nPlease load an image file before OpenCV."));
				msgBox.exec();
				return;
		}
		/*
		ImageProcessParam param;

		param.debug = true;

		param.gray =paramWidget->grayGroup->isChecked();

		param.threshold = paramWidget->threshold->isChecked();
		param.thresholdBlockSize = paramWidget->thresholdBlockSize->value();
		param.thresholdMeanBias = paramWidget->thresholdMeanBias->value();

		param.hough = paramWidget->houghGroup->isChecked();
		param.houghRho = paramWidget->houghRho->value();
		param.houghTheta = paramWidget->houghTheta->value();
		param.houghThreshold = paramWidget->houghThreshold->value();
		param.houghParam1 = paramWidget->houghParam1->value();
		param.houghParam2 = paramWidget->houghParam2->value();

		param.corner = paramWidget->cornerGroup->isChecked();
		param.cornerDegree = paramWidget->cornerDegree->value();
		param.cornerGap = paramWidget->cornerGap->value();

		param.background = paramWidget->backgroundGroup->isChecked();

		param.normalize = paramWidget->normalizeGroup->isChecked();
		param.normalizeTopMargin = paramWidget->normalizeTopMargin->value();
		param.normalizeSideMargin =paramWidget->normalizeSideMargin->value();
		param.normalizeWidth = paramWidget->normalizeWidth->value();
		param.normalizeHeight = paramWidget->normalizeHeight->value();

		param.blueText =  paramWidget->blueTextGroup->isChecked();
		param.blueTextMin = paramWidget->blueTextMin->value();
		param.blueTextMax = paramWidget->blueTextMax->value();
		*/
		mImageProcess->setMasks(mOCRMasks);
		mImageProcess->run(&param);

		//IplImage* cvImage2 = mImageProcess->getProcessedImage();
		//QImage* qtImage = ImageAdapter::IplImage2QImage(cvImage2);
		//imageWidget->setImage(qtImage);

		//delete paramWidget;
}

/**
 * 载入模板
 */
void loadMasks(std::vector<OCRMask>* mOCRMasks)
{
		QString strDBDir = getModeDir();

		if (!strDBDir.isEmpty())
		{
				QFile file(strDBDir);
				if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
						QMessageBox msgBox;
						msgBox.setIcon(QMessageBox::Warning);
						msgBox.setText(QObject::tr("Cannot open file to read.\nMaybe this file does not exist."));
						msgBox.exec();
						return;
				}

				mOCRMasks->clear();

				QXmlStreamReader xml;
				xml.setDevice(&file);

				while(!xml.atEnd() && !xml.hasError())
				{
						QXmlStreamReader::TokenType token = xml.readNext();
						if(token == QXmlStreamReader::StartDocument)
						{
								continue;
						}
						if(token == QXmlStreamReader::StartElement)
						{
								if(xml.name() == "masks")
								{
										continue;
								}
								if(xml.name() == "mask")
								{
										QXmlStreamAttributes attributes = xml.attributes();
										OCRMask mask;
										mask.key = attributes.value("key").toString().toLocal8Bit();
										mask.rect.x = (attributes.value("x").toString().toInt());
										mask.rect.y = (attributes.value("y").toString().toInt());
										mask.rect.width = (attributes.value("w").toString().toInt());
										mask.rect.height = (attributes.value("h").toString().toInt());
										mOCRMasks->push_back(mask);
								}
						}
				}
				if(xml.hasError())
				{
						QMessageBox msgBox;
						msgBox.setIcon(QMessageBox::Warning);
						msgBox.setText(QObject::tr("Parse XML failed.\nMaybe this file is not the right one."));
						msgBox.exec();
				}
				xml.clear();
				//imageWidget->update();
				//ocrWidget->update();
		}
}

/**
 * 初始化发票信息
 */
void initBillInfo(const std::vector<OCRMask>* mOCRMasks, BillInfo& mBillInfo)
{
		for(int i=0;i<mOCRMasks->size();i++)
		{
				OCRMask mask = mOCRMasks->at(i);

				if(mask.key == "发票代码")
						mBillInfo.billCode = mask.value.c_str();
				else if(mask.key == "发票号码")
						mBillInfo.billNO = mask.value.c_str();
				else if(mask.key == "销货单位")
						mBillInfo.billProvider = mask.value.c_str();
				else if(mask.key == "购货单位")
						mBillInfo.billCustomer = mask.value.c_str(); 
				else if(mask.key == "密码区")
						mBillInfo.billSecreatArea = mask.value.c_str();
				else if(mask.key == "金额")
						mBillInfo.billAmount = mask.value.c_str(); 
				else if(mask.key == "税额")
						mBillInfo.billTax = mask.value.c_str(); 
				else if(mask.key == "开票日期")
						mBillInfo.billDate = mask.value.c_str();
		}
	
}

/**
 * 识别发票信息
 */
void recognizeText(ImageProcess *mImageProcess, std::vector<OCRMask>* mOCRMasks, TesseractOCR* mTesseractOCR)
{
		IplImage* cvImage = mImageProcess->getProcessedImage();
	if(!cvImage)
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(QObject::tr("No image loaded.\nPlease load an image file before OCR."));
		msgBox.exec();
		return;
	}

	/*
	if(mAbbyyOCR == NULL)
	{
		mAbbyyOCR = new AbbyyOCR();
	}
	mAbbyyOCR->setImage(cvImage);
	mAbbyyOCR->setMasks(masks);
	QString abbyyText = mAbbyyOCR->recognizeText();
	*/

	if(mTesseractOCR == NULL)
	{
		mTesseractOCR = new TesseractOCR();
	}
	bool initSuccess = false;
	if(initSuccess)//actionEnableChinese->isChecked())
	{
		initSuccess = mTesseractOCR->init(TesseractOCR::TESSERACTOCR_CHINESE);
	}
	else
	{
		initSuccess = mTesseractOCR->init(TesseractOCR::TESSERACTOCR_ENGLISH);
	}
	if(initSuccess == false)
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText(QObject::tr("Can not initialize tesseract"));
		msgBox.exec();
		return; 
	}
	mTesseractOCR->setImage(cvImage);
	mTesseractOCR->setMasks(mOCRMasks);

	QString tesseractText = QString::fromLocal8Bit( mTesseractOCR->recognizeText().c_str() );

	//ocrWidget->update();
	//textEdit->clear();
	//textEdit->setText(QString("%1\n%2\n%3\n%4").arg("Abbyy OCR:", abbyyText, "Tesseract OCR:", tesseractText));
	//teditBillSecretArea->setText(QString("%1").arg(tesseractText));

	int rectCount = mTesseractOCR->getBoxes()->size();
	QRect* rects = new QRect[rectCount];
	for(int i = 0; i< rectCount; i++)
	{
		rects[i].setX(mTesseractOCR->getBoxes()->at(i).x);
		rects[i].setY(mTesseractOCR->getBoxes()->at(i).y);
		rects[i].setWidth(mTesseractOCR->getBoxes()->at(i).width);
		rects[i].setHeight(mTesseractOCR->getBoxes()->at(i).height);
	}
	//imageWidget->setBoxes(rects, rectCount);
}

/**
 * 完整的发票识别认证流程
 */
void Worker :: run()
{
		ImageProcess* threadImageProcess = new ImageProcess();
		std::vector<OCRMask>* threadOCRMasks = new std::vector<OCRMask>();
		BillInfo* threadBillInfo = new BillInfo;
		TesseractOCR* threadTesseractOCR = new TesseractOCR();

		processImage(threadImageProcess, threadOCRMasks, *threadBillInfo);
		loadMasks(threadOCRMasks);
		recognizeText(threadImageProcess, threadOCRMasks, threadTesseractOCR);
		initBillInfo(threadOCRMasks, *threadBillInfo);

		mutex.lock();
		vBillInfo.push_back(threadBillInfo);
		mutex.unlock();

		if(threadOCRMasks)
		{
				delete threadOCRMasks;
		}

		if(threadImageProcess)
		{
				delete threadImageProcess;
		}

		if(threadTesseractOCR)
		{
				delete threadTesseractOCR; 
		}
		
}