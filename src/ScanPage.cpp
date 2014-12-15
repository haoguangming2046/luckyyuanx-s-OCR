#include "ScanPage.h"
#include "ImageWidget.h"
#include "Share/ImageProcess.h"
#include "Share/TesseractOCR.h"
#include "Share/ImageAdapter.h"
#include "Share/AppInfo.h"
#include "Share/AbbyyOCR.h"
#include "twain/qtwain.h"
#include "twain/dib.h"
#include "twain/dibutil.h"

//#include <QSqlDatabase>
#include <QSqlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslSocket>
#include <QSslConfiguration>

#include <QMutex>
#include <QWaitCondition>
#include <QThreadPool>

#include "DataBase.h"
#include "SysconfPage.h"
#include "WorkerThread.h"

QQueue<QString> MsgQueue;				//��Ϣ����
QMutex mutex;										//��
QWaitCondition waitProduce;				
QWaitCondition waitConsume;
ImageProcessParam param;

extern std::vector<BillInfo*> vBillInfo;

ScanPage :: ScanPage()
{
		mTwain = new QTwain();

		mOCRMasks = new std::vector<OCRMask>();
		mImageProcess = new ImageProcess();
		//mAbbyyOCR = NULL;//û����
		mTesseractOCR = NULL;

		buildUI();
		worker = NULL;
		threadpool = new QThreadPool;
		//threadpool->setMaxThreadCount(3);
		nCurrentBill = 1;//Ĭ����ʾ��һ�ţ�ע�⣬�Ǵ�1��ʼ�����
		bAuto = true;
		getImageProcessParam(param);
}

ScanPage :: ~ScanPage()
{
		if(mOCRMasks)
		{
				delete mOCRMasks;
		}
		if(mImageProcess)
		{
				delete mImageProcess;
		}

		if(mTesseractOCR)
		{
				delete mTesseractOCR;
		}

		if(sysconfPage)
		{
				delete sysconfPage;
		}

		if(threadpool)
		{
				delete threadpool;
		}
}

void ScanPage :: buildUI()
{
		imageWidget = new ImageWidget;
		imageWidget->setMasks(mOCRMasks);

		QScrollArea* scrollArea = new QScrollArea();
		scrollArea->setWidget(imageWidget);
		scrollArea->setWidgetResizable(true);
		
		pbtnScan = new QPushButton(tr("ɨ��"));
		pbtnAuth = new QPushButton(tr("��֤"));
		pbtnSave = new QPushButton(tr("����"));
		
		pbtnProcessImage = new QPushButton(tr("ͼƬ����"));			//temp
		pbtnRecognizeText = new QPushButton(tr("ʶ��"));				//temp
		pbtnLoadMasks = new QPushButton(tr("����XML"));		//temp
		
		sysconfPage = new SysconfPage;															//temp
		
		QGridLayout* glayoutTemp = new QGridLayout;
		glayoutTemp->addWidget(pbtnScan, 0, 0);
		glayoutTemp->addWidget(pbtnAuth, 0, 1);		
		glayoutTemp->addWidget(pbtnSave, 0, 2);
		glayoutTemp->addWidget(pbtnProcessImage, 1, 0);
		glayoutTemp->addWidget(pbtnLoadMasks, 1, 1);
		glayoutTemp->addWidget(pbtnRecognizeText, 1, 2);
		
		QLabel* label1 = new QLabel(tr("��Ʊ����"));
		leditBillCode = new QLineEdit;
		QLabel* label2 = new QLabel(tr("��Ʊ����"));
		leditBillNO = new QLineEdit;
		QLabel* label3 = new QLabel(tr("��������"));
		leditBillProvider = new QLineEdit;
		QLabel* label4 = new QLabel(tr("��������"));
		leditBillCustomer = new QLineEdit;
		QLabel* labelSecret1 = new QLabel(tr("����1"));
		leditBillSecret1 = new QLineEdit;
		QLabel* labelSecret2 = new QLabel(tr("����2"));
		leditBillSecret2 = new QLineEdit;
		QLabel* labelSecret3 = new QLabel(tr("����3"));
		leditBillSecret3 = new QLineEdit;
		QLabel* labelSecret4 = new QLabel(tr("����4"));
		leditBillSecret4 = new QLineEdit;
		QLabel* label6 = new QLabel(tr("��Ʊ���"));
		leditBillAmount = new QLineEdit;
		QLabel* label7 = new QLabel(tr("��Ʊ˰��"));
		leditBillTax = new QLineEdit;
		QLabel* label8 = new QLabel(tr("��Ʊ����"));
		leditBillDate = new QLineEdit;
		QLabel* label9 = new QLabel(tr("���ܰ汾"));
		leditBillVersion = new QLineEdit;
		leditBillVersion->setText(tr("01"));

		pbtnFirst = new QPushButton(tr("��һ��"));
		pbtnPre = new QPushButton(tr("��һ��"));
		pbtnNext = new QPushButton(tr("��һ��"));
		pbtnLast = new QPushButton(tr("�����"));
		/*
		pbtnFirst->setEnabled(false);
		pbtnLast->setEnabled(false);
		pbtnNext->setEnabled(false);
		pbtnPre->setEnabled(false);
		*/
		QVBoxLayout* vLayoutShow = new QVBoxLayout;
		vLayoutShow->addWidget(pbtnFirst);
		vLayoutShow->addWidget(pbtnPre);
		vLayoutShow->addWidget(pbtnNext);
		vLayoutShow->addWidget(pbtnLast);

		pbtnSingleScan = new QPushButton(tr("����ɨ��"));
		pbtnMultiScan = new QPushButton(tr("����ɨ��"));
		//pbtnMultiScan->setEnabled(false);
		pbtnMunual = new QPushButton(tr("�ֹ�¼��"));
		//pbtnMunual->setEnabled(false);
		pbtnSaveData = new QPushButton(tr("���ݱ���"));

		QLabel* label10 = new QLabel(tr("��"));
		labelCurrent = new QLabel(tr("0"));
		QLabel* label11 = new QLabel(tr("/"));
		labelTotal = new QLabel(tr("0"));
		QLabel* label12 = new QLabel(tr("��"));

		QHBoxLayout* hLayoutNum = new QHBoxLayout;
		hLayoutNum->addWidget(label10);
		hLayoutNum->addWidget(labelCurrent);
		hLayoutNum->addWidget(label11);
		hLayoutNum->addWidget(labelTotal);
		hLayoutNum->addWidget(label12);

		QVBoxLayout* vLayoutOperate = new QVBoxLayout;
		vLayoutOperate->addWidget(pbtnSingleScan);
		vLayoutOperate->addWidget(pbtnMultiScan);
		vLayoutOperate->addWidget(pbtnMunual);
		vLayoutOperate->addWidget(pbtnSaveData);
		vLayoutOperate->addLayout(hLayoutNum);

		QGridLayout* gLayout1 = new QGridLayout;
		gLayout1->addWidget(label4, 0, 0);
		gLayout1->addWidget(leditBillCustomer, 0, 1);
		gLayout1->addWidget(label3, 1, 0);
		gLayout1->addWidget(leditBillProvider, 1, 1);

		QGridLayout* gLayout2 = new QGridLayout;
		gLayout2->addWidget(label1, 0, 0);
		gLayout2->addWidget(leditBillCode, 0, 1);
		gLayout2->addWidget(label2, 0, 2);
		gLayout2->addWidget(leditBillNO, 0, 3);
		gLayout2->addWidget(label8, 1, 0);
		gLayout2->addWidget(leditBillDate, 1, 1);
		gLayout2->addWidget(label9, 1, 2);
		gLayout2->addWidget(leditBillVersion);
		gLayout2->addWidget(label6, 2, 0);
		gLayout2->addWidget(leditBillAmount, 2, 1);
		gLayout2->addWidget(label7, 2, 2);
		gLayout2->addWidget(leditBillTax, 2, 3);

		QVBoxLayout* vLayoutLeft = new QVBoxLayout;
		vLayoutLeft->addLayout(gLayout1);
		vLayoutLeft->addLayout(gLayout2);

		QGridLayout* gLayoutMiddle = new QGridLayout;
		gLayoutMiddle->addWidget(labelSecret1, 0, 0);
		gLayoutMiddle->addWidget(leditBillSecret1, 0, 1);
		gLayoutMiddle->addWidget(labelSecret2, 1, 0);
		gLayoutMiddle->addWidget(leditBillSecret2, 1, 1);
		gLayoutMiddle->addWidget(labelSecret3, 2, 0);
		gLayoutMiddle->addWidget(leditBillSecret3, 2, 1);
		gLayoutMiddle->addWidget(labelSecret4, 3, 0);
		gLayoutMiddle->addWidget(leditBillSecret4, 3, 1);

		QHBoxLayout* hLayoutUpper = new QHBoxLayout;
		hLayoutUpper->addLayout(vLayoutLeft);
		hLayoutUpper->addLayout(gLayoutMiddle);
		hLayoutUpper->addLayout(vLayoutShow);
		hLayoutUpper->addLayout(vLayoutOperate);
		hLayoutUpper->addLayout(glayoutTemp);

		QVBoxLayout* vmainlayout = new QVBoxLayout;
		vmainlayout->addLayout(hLayoutUpper);
		vmainlayout->addWidget(scrollArea, 1);		

		this->setLayout(vmainlayout);

		actionEnableChinese = new QAction(tr("enableChinese"), this);
		actionEnableChinese->setCheckable(false);
		//connect(actionEnableChinese, SIGNAL(triggered()), this, SLOT(enableChinese()));

		connect(mTwain, SIGNAL(dibAcquired(CDIB*)), this, SLOT(onDibAcquired(CDIB*)), Qt::QueuedConnection);		
		connect(pbtnSingleScan, SIGNAL(clicked()), this, SLOT(scanImage()));
		connect(pbtnMultiScan, SIGNAL(clicked()), this, SLOT(scanImage()));
		connect(pbtnMunual, SIGNAL(clicked()), this, SLOT(inputByHand()));
		connect(pbtnSaveData, SIGNAL(clicked()), this, SLOT(saveData()));							//2014-4-21
		//connect(pbtnSaveData, SIGNAL(clicked()), this, SLOT(on_pbtnSave_clicked()));		//2014-4-21
				
		connect(pbtnFirst, SIGNAL(clicked()), this, SLOT(showFirst()));
		connect(pbtnPre, SIGNAL(clicked()), this, SLOT(showPre())); 
		connect(pbtnNext, SIGNAL(clicked()), this, SLOT(showNext()));
		connect(pbtnLast, SIGNAL(clicked()), this, SLOT(showLast()));

		connect(pbtnProcessImage, SIGNAL(clicked()), this, SLOT(processImage()));				//temp
		connect(pbtnRecognizeText, SIGNAL(clicked()), this, SLOT(recognizeText()));				//temp
		connect(pbtnLoadMasks, SIGNAL(clicked()), this, SLOT(loadMasks()));					//temp
		connect(pbtnSave, SIGNAL(clicked()), this, SLOT(on_pbtnSave_clicked()));					//temp
		connect(pbtnAuth, SIGNAL(clicked()), this, SLOT(on_pbtnRecognize_click()));				//temp
		connect(pbtnScan, SIGNAL(clicked()), this, SLOT(scanImage()));									//temp
		
		//��ȡϵͳ�����е�fpid, action, czydm, passwd
		QSqlQuery* sysQuery = new QSqlQuery;
		bool isok = sysQuery->exec("select * from systemparams where key = 'url' ");
		sysQuery->next();
		strurl = sysQuery->value(1).toString();
		sysQuery->exec("select * from systemparams where key = 'nsrsbh' ");
		sysQuery->next();
		strnsrsbh = sysQuery->value(1).toString();
		sysQuery->exec("select * from systemparams where key = 'fpid' ");
		sysQuery->next();
		strfpid = sysQuery->value(1).toString();
		sysQuery->exec("select * from systemparams where key = 'action' ");
		sysQuery->next();
		straction = sysQuery->value(1).toString();
		sysQuery->exec("select * from systemparams where key = 'czydm' ");
		sysQuery->next();
		strczydm = sysQuery->value(1).toString();
		sysQuery->exec("select * from systemparams where key = 'passwd' ");
		sysQuery->next();
		strpasswd = sysQuery->value(1).toString();
		sysQuery->exec("select * from systemparams where key = 'modedir' ");
		sysQuery->next();
		strDBDir = sysQuery->value(1).toString();

		//�����Ա
		sslConfig = new QSslConfiguration;
		networkRequest = new QNetworkRequest;
		networkAccessManager = new QNetworkAccessManager(this);
		connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
		networkRequest->setUrl(strurl);
		networkRequest->setHeader(QNetworkRequest::ContentTypeHeader,QString("application/x-www-form-urlencoded").toUtf8());
		//����SSL
		sslConfig->setPeerVerifyMode(QSslSocket::VerifyNone);
		sslConfig->setProtocol(QSsl::TlsV1);
		networkRequest->setSslConfiguration(*sslConfig);
}

/**********************************fuctions**********************************/
void ScanPage :: getImageProcessParam(ImageProcessParam& param)
{
		param.debug = true;

		param.gray =sysconfPage->getParamPtr()->grayGroup->isChecked();

		param.threshold = sysconfPage->getParamPtr()->threshold->isChecked();
		param.thresholdBlockSize = sysconfPage->getParamPtr()->thresholdBlockSize->value();
		param.thresholdMeanBias = sysconfPage->getParamPtr()->thresholdMeanBias->value();

		param.hough = sysconfPage->getParamPtr()->houghGroup->isChecked();
		param.houghRho = sysconfPage->getParamPtr()->houghRho->value();
		param.houghTheta = sysconfPage->getParamPtr()->houghTheta->value();
		param.houghThreshold = sysconfPage->getParamPtr()->houghThreshold->value();
		param.houghParam1 = sysconfPage->getParamPtr()->houghParam1->value();
		param.houghParam2 = sysconfPage->getParamPtr()->houghParam2->value();

		param.corner = sysconfPage->getParamPtr()->cornerGroup->isChecked();
		param.cornerDegree = sysconfPage->getParamPtr()->cornerDegree->value();
		param.cornerGap = sysconfPage->getParamPtr()->cornerGap->value();

		param.background = sysconfPage->getParamPtr()->backgroundGroup->isChecked();

		param.normalize = sysconfPage->getParamPtr()->normalizeGroup->isChecked();
		param.normalizeTopMargin = sysconfPage->getParamPtr()->normalizeTopMargin->value();
		param.normalizeSideMargin =sysconfPage->getParamPtr()->normalizeSideMargin->value();
		param.normalizeWidth = sysconfPage->getParamPtr()->normalizeWidth->value();
		param.normalizeHeight = sysconfPage->getParamPtr()->normalizeHeight->value();

		param.blueText =  sysconfPage->getParamPtr()->blueTextGroup->isChecked();
		param.blueTextMin = sysconfPage->getParamPtr()->blueTextMin->value();
		param.blueTextMax = sysconfPage->getParamPtr()->blueTextMax->value();
}

ImageWidget* ScanPage :: getImageWidgetPtr()
{
		return imageWidget;
}

ImageProcess* ScanPage :: getImageProcessPtr()
{
		return mImageProcess;
}

void ScanPage :: releasevBillInfo()
{
		int n = vBillInfo.size();

		if(n != 0)
		{
				mutex.lock();
				for(int i = 0; i < n; ++i)
				{
						delete vBillInfo[i];
						vBillInfo.pop_back();
				}
				mutex.unlock();
		}
}

void ScanPage :: showRecognizeResult()
{
		//QString strDate = NULL;
		//QString strYear = NULL;
		//QString strMonth = NULL;
		//QString strDay = NULL;

		for(int i=0;i<mOCRMasks->size();++i)
		{
				OCRMask mask = mOCRMasks->at(i);
				
				if(mask.key == "��Ʊ����")
				{
						mBillData.billCode = mask.value.c_str();
						leditBillCode->setText(mBillData.billCode);
				}
				else if(mask.key == "��Ʊ����")
				{
						mBillData.billNO = mask.value.c_str();
						leditBillNO->setText(mBillData.billNO);
				}
				else if(mask.key == "������λ")
				{
						mBillData.billProvider = mask.value.c_str();
						leditBillProvider->setText(mBillData.billProvider);
				}
				else if(mask.key == "������λ")
				{
						mBillData.billCustomer = mask.value.c_str();
						leditBillCustomer->setText(mBillData.billCustomer); 
				}
				else if(mask.key == "������")
				{
						QString strSecretArea = mask.value.c_str();
						QStringList listSecret = strSecretArea.split('\n');
						mBillData.billSecreatArea = strSecretArea;
						
						leditBillSecret1->setText(listSecret[0]);
						leditBillSecret2->setText(listSecret.at(1));
						leditBillSecret3->setText(listSecret.at(2));
						leditBillSecret4->setText(listSecret.at(3));
				}
				else if(mask.key == "���")
				{
						mBillData.billAmount = mask.value.c_str();
						leditBillAmount->setText(mBillData.billAmount); 
				}
				else if(mask.key == "˰��")
				{
						mBillData.billTax = mask.value.c_str();
						leditBillTax->setText(mBillData.billTax); 
				}
				else if(mask.key == "��Ʊ����")
				{
						mBillData.billDate = mask.value.c_str();
						leditBillDate->setText(mBillData.billDate);
				}
				//else if(mask.key == "��")
				//		strYear = mask.value.c_str();
				//else if(mask.key == "��")
				//		strMonth = mask.value.c_str();
				//else	if(mask.key == "��")
				//		strDay = mask.value.c_str();			
		}
		//strDate= strYear + '-' + strMonth + '-'  + strDay;
		
		//leditBillDate->setText(strDate.trimmed()); 	
}

/**
 * �ڽ�������ʾ��Ʊ����Ϣ
 */
void ScanPage :: showBillInfo(const BillInfo& billInfo)
{
		IplImage* cvImage = cvLoadImage(billInfo.fileName.toLocal8Bit().data());
		QImage* qtImage= ImageAdapter::IplImage2QImage(cvImage);
		imageWidget->setImage(qtImage);

		leditBillCode->setText(billInfo.billCode);
		leditBillNO->setText(billInfo.billNO);
		leditBillProvider->setText(billInfo.billProvider);
		leditBillCustomer->setText(billInfo.billCustomer);

		mlistBillSectret = billInfo.billSecreatArea.split('\n');
		leditBillSecret1->setText(mlistBillSectret.at(0));
		leditBillSecret2->setText(mlistBillSectret.at(1));
		leditBillSecret3->setText(mlistBillSectret.at(2));
		leditBillSecret4->setText(mlistBillSectret.at(3));

		leditBillAmount->setText(billInfo.billAmount);
		leditBillTax->setText(billInfo.billTax);
		leditBillDate->setText(billInfo.billDate);

		labelCurrent->setText(QString::number(nCurrentBill, 10));
		labelTotal->setText(QString::number(vBillInfo.size(), 10));
}

void ScanPage::showEvent(QShowEvent* thisEvent)
{
		mTwain->setParent(this);
}

bool ScanPage::winEvent(MSG* pMsg, long* result)
{
		mTwain->processMessage(*pMsg); 
		return false;
}

/*********************slots*********************/
/**
 * ��ʾ��һ�ŷ�Ʊ��Ϣ
 */
void ScanPage :: showFirst() 
{
		saveModifiedData();

		nCurrentBill = 1;
		labelCurrent->setText(QString::number(nCurrentBill, 10));
		showBillInfo(*vBillInfo[nCurrentBill - 1]);
}

/**
 * ��ʾǰһ�ŷ�Ʊ��Ϣ
 */
void ScanPage :: showPre() 
{
		saveModifiedData();

		nCurrentBill--;		

		if(nCurrentBill == 1)
		{				
				pbtnPre->setEnabled(false);
		}

		if(nCurrentBill == (vBillInfo.size() - 1))
		{
				pbtnNext->setEnabled(true);
		}		

		labelCurrent->setText(QString::number(nCurrentBill, 10));
		showBillInfo(*vBillInfo[nCurrentBill - 1]);
}

/**
 * ��ʾ��һ�ŷ�Ʊ��Ϣ
 */
void ScanPage :: showNext()
{
		saveModifiedData();

		nCurrentBill ++;

		if(nCurrentBill == (vBillInfo.size() - 1))
		{
				pbtnNext->setEnabled(false);
		}

		if(nCurrentBill == 1)
		{
				pbtnPre->setEnabled(true);
		}

		labelCurrent->setText(QString::number(nCurrentBill, 10));
		showBillInfo(*vBillInfo[nCurrentBill - 1]);
}

/**
 * ��ʾ���һ�ŷ�Ʊ��Ϣ
 */
void ScanPage ::showLast()
{
		saveModifiedData();
		nCurrentBill = vBillInfo.size();

		labelCurrent->setText(QString::number(nCurrentBill, 10));
		showBillInfo(*vBillInfo[nCurrentBill - 1]);
}

/**
 * �����ֶ�¼��
 */
void ScanPage :: inputByHand()
{
		bAuto = false;
		clearControlsText();		
}

/**
 * ʶ��Ʊ
 */
void ScanPage::on_pbtnRecognize_click()
{
		processImage();
		enableChinese();
		recognizeText();
		//showRecognizeResult();
}

/**
 * ��֤��Ʊ
 */
void ScanPage::on_pbtnAuth_clicked()
{
		if(!networkRequest->url().isValid())
		{
				QMessageBox::information(this, tr("��ʾ"), tr("��֤URL�����ã�����ϵͳ���ã�"));
				return;
		}
		//�ж������Ƿ��Ѵ������ݿ���
		QSqlQuery query;
		query.prepare("select * from billtable where Bill_Code = ? and Bill_NO = ?");
		query.addBindValue(mBillData.billCode);
		query.addBindValue(mBillData.billNO);
		bool isok = query.exec();
		QString strcode1 = query.value(0).toString();
		bool ok1 = query.isValid();
		query.next();
		QString strcode2 = query.value(0).toString();
		bool ok2 =  query.isValid();
		int nrow = query.size();
		if(isok)
		{
				QMessageBox::information(this, tr("��ʾ"), tr("�������Ѿ����ڣ�"));
				clearControlsText();
				return;
		}

		QString strpost = createPostData();
		networkRequest->setHeader(QNetworkRequest::ContentLengthHeader, strpost.length());
		networkReply = networkAccessManager->post(*networkRequest, strpost.toUtf8());
}

/**
 * ������֤���
 */
void ScanPage::finishedSlot(QNetworkReply* reply)
{
		if (networkReply->error() == QNetworkReply::NoError) 
		{
				QByteArray bytes = reply->readAll();
				QString string = QString::fromLocal8Bit(bytes);
				QStringList strList= string.split('\n');

				int returnid = strList.at(0).toInt();
				QString strAuthResult = strList.at(1);
				if(0 == returnid)
				{
						QString strType = "AUTHED";
						insertAuthData(strType, strAuthResult);
				}
				else
				{
						QString strType = "NOTPASS";
						insertAuthData(strType, strAuthResult);
				}
				QMessageBox::information(this, tr("��֤���"), strAuthResult);
		}
		else
		{
				QByteArray bytes = reply->readAll(); 
				QString string = QString::fromUtf8(bytes);
				QMessageBox::information(this, "Warmming Error", string);
		}
		clearControlsText();
		reply->deleteLater();  
}

/**
 * �����ݿ��в�����֤��Ʊ��Ϣ
 */
bool ScanPage::insertAuthData(QString & strType, QString& strResult)
{
		QSqlQuery query;
		QString strSql = "insert into billtable values(?,?,?,?,?,?,?,?,?,?,?,?,?)";
		query.prepare(strSql);
		query.addBindValue(mBillData.billCode);
		query.addBindValue(mBillData.billNO);
		query.addBindValue(mBillData.billProvider);
		query.addBindValue(mBillData.billCustomer);
		query.addBindValue(mBillData.billSecreatArea);
		query.addBindValue(mBillData.billAmount.toDouble());
		query.addBindValue(mBillData.billTax.toDouble());
		query.addBindValue(QDate::fromString(QString(mBillData.billDate), "yyyy-MM-dd"));
		query.addBindValue(strType);
		query.addBindValue(QDate::currentDate());		
		query.addBindValue(QDate::currentDate());
		query.addBindValue(1);
		query.addBindValue(strResult);

		return query.exec();
}

/**
 * ���淢Ʊ��Ϣ
 */
void ScanPage ::saveData()
{
		if(bAuto)
		{
				saveModifiedData();//���浱ǰ��ʾ�ķ�Ʊ��Ϣ

				int n = vBillInfo.size();
				for(int i = 0; i < n; i ++)
				{
						//mBillData = *vBillInfo[i];
						insertDataToDB(*vBillInfo[i]);
				}
		}
		else
		{
				mBillData.billCode = leditBillCode->text();
				mBillData.billNO = leditBillNO->text();
				mBillData.billProvider = leditBillProvider->text();
				mBillData.billCustomer = leditBillCustomer->text();
				mBillData.billSecreatArea = leditBillSecret1->text() + "\n" +
						leditBillSecret2->text() + "\n" +
						leditBillSecret3->text() + "\n" + 
						leditBillSecret4->text();
				mBillData.billAmount = leditBillAmount->text();
				mBillData.billTax = leditBillTax->text();
				mBillData.billDate = leditBillDate->text();				

				insertDataToDB(mBillData);
				clearControlsText();
		}
}

/**
 * �����ݿ��в�������
 */
void ScanPage:: insertDataToDB(const BillInfo & billInfo)
{		
		QSqlQuery* query = new QSqlQuery;

		//����������Ƿ��Ѿ����������ݿ���
		query->prepare("select * from billtable where Bill_Code = ? and Bill_NO = ?");
		query->addBindValue(billInfo.billCode);
		query->addBindValue(billInfo.billNO);
		query->exec();
		if(query->next())
		{
				QMessageBox::information(this, tr("��ʾ"), tr("������") + billInfo.billCode
						+ tr("/") + billInfo.billNO +tr("�Ѿ�����"));

				return;
		}
		
		QString strBillType = "WAITAUTH";

		dRecordDate = QDate::currentDate();
		dBillDate = QDate::fromString(QString(billInfo.billDate), "yyyy-MM-dd");

		QString strSql = "insert into billtable(Bill_Code, Bill_NO, Bill_Provider, Bill_Customer, Bill_SecretArea, Bill_Amount, Bill_Tax, Bill_Date,  Bill_Type, Bill_RecordDate) values(?,?,?,?,?,?,?,?,?,?)";
		query->prepare(strSql);
		query->addBindValue(billInfo.billCode); 
		query->addBindValue(billInfo.billNO);
		query->addBindValue(billInfo.billProvider);
		query->addBindValue(billInfo.billCustomer);
		query->addBindValue(billInfo.billSecreatArea);
		query->addBindValue(billInfo.billAmount.toDouble());
		query->addBindValue(billInfo.billTax.toDouble());
		query->addBindValue(dBillDate);
		query->addBindValue(strBillType);
		query->addBindValue(dRecordDate);
		bool success = query->exec();	

		//clearMasks();//��ȥ��
		
		if(success)
			QMessageBox::information(this, tr("��ʾ"), tr("�������ݳɹ�..."));
		else
			QMessageBox::information(this, tr("��ʾ"), tr("��������ʧ�ܣ���ȷ������������ȷ���¼���Ѿ����ڣ�"));

		//delete query;
		//clearControlsText();
}

/**
 * ɨ������
 * 1.ѡ��ɨ���ǡ�
 * 2.��ʼɨ��
 */
void ScanPage::scanImage()
{
		//�������ȫ�ֱ���
		releasevBillInfo();
		mutex.lock();
		MsgQueue.clear();
		mutex.unlock();

		clearControlsText();
		bAuto = true;

		mTwain->selectSource();
		mTwain->acquire();
}

/**
 * ɨ�����ʱ����������ɨ��һ�Ŵ���һ�Σ��ڽ�������ʾ��һ��
 */
void ScanPage::onDibAcquired(CDIB* pDib)
{
		static int ni = 0;
		if(pDib)
		{
				QString strfilename = "filetemp" + QString::number(ni, 10) + ".bmp";
				SaveDIB(pDib->DIBHandle(), strfilename.toLocal8Bit().data());

				//if(!bSingleScan)//Ĭ�ϵ���ɨ��
				//{
				//		int width = pDib->Width();
				//		int height = pDib->Height();
				//		int depth = pDib->GetBitmapInfo()->bmiHeader.biBitCount == 1?IPL_DEPTH_1U : IPL_DEPTH_8U;
				//		int nChannels = 3;
			
				//		mBillData.fileName = strfilename;
				//		IplImage* cvImage = cvLoadImage(strfilename.toLocal8Bit().data());
				//		//Convert dib to iplImage herer!!

				//		if(cvImage)
				//		{
				//				mImageProcess->setImage(cvImage);
				//				QImage* qtImage= ImageAdapter::IplImage2QImage(cvImage);
				//				imageWidget->setImage(qtImage); 
				//		}
				//}
				//else	//����ɨ��
				//{
						mutex.lock();
						if(MsgQueue.size() == 5)
						{
								waitProduce.wait(&mutex);						
						}
						mutex.unlock();

						mutex.lock();
						MsgQueue.enqueue(strfilename);
						waitConsume.wakeAll();
						mutex.unlock();
						worker = new Worker;						
						threadpool->start(worker);
						threadpool->waitForDone();
						
						memcpy(&mBillData, vBillInfo[0], sizeof(BillInfo));

						labelTotal->setText(QString::number	(vBillInfo.size(), 10));
						labelCurrent->setText(QString::number(1, 10));
						showBillInfo(mBillData);
				//}				
				
				delete pDib;
		}
		ni ++;
} 

/*******************************fuctions*******************************/
/**
 * ���澭���޸ĵ�����
 */
void ScanPage ::saveModifiedData()
{
		mutex.lock();
		vBillInfo[nCurrentBill - 1]->billCode = leditBillCode->text();
		vBillInfo[nCurrentBill - 1]->billNO = leditBillNO->text();
		vBillInfo[nCurrentBill - 1]->billProvider = leditBillProvider->text();
		vBillInfo[nCurrentBill - 1]->billCustomer = leditBillCustomer->text();
		vBillInfo[nCurrentBill - 1]->billSecreatArea = leditBillSecret1->text() + "\n" +
				leditBillSecret2->text() + "\n" +
				leditBillSecret3->text() + "\n" + 
				leditBillSecret4->text();
		vBillInfo[nCurrentBill - 1]->billAmount = leditBillAmount->text();
		vBillInfo[nCurrentBill - 1]->billTax = leditBillTax->text();
		vBillInfo[nCurrentBill - 1]->billDate = leditBillDate->text();
		mutex.unlock();
}

/**
 * ��ɨ��֮���ͼ����д���
 */
void ScanPage::processImage()
{
		if(mImageProcess->getOriginalImage() == NULL)
		{
				QMessageBox msgBox;
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setText(tr("No image loaded.\nPlease load an image file before OpenCV."));
				msgBox.exec();
				return;
		}

		ImageProcessParam param;

		param.debug = true;

		param.gray =sysconfPage->getParamPtr()->grayGroup->isChecked();

		param.threshold = sysconfPage->getParamPtr()->threshold->isChecked();
		param.thresholdBlockSize = sysconfPage->getParamPtr()->thresholdBlockSize->value();
		param.thresholdMeanBias = sysconfPage->getParamPtr()->thresholdMeanBias->value();

		param.hough = sysconfPage->getParamPtr()->houghGroup->isChecked();
		param.houghRho = sysconfPage->getParamPtr()->houghRho->value();
		param.houghTheta = sysconfPage->getParamPtr()->houghTheta->value();
		param.houghThreshold = sysconfPage->getParamPtr()->houghThreshold->value();
		param.houghParam1 = sysconfPage->getParamPtr()->houghParam1->value();
		param.houghParam2 = sysconfPage->getParamPtr()->houghParam2->value();

		param.corner = sysconfPage->getParamPtr()->cornerGroup->isChecked();
		param.cornerDegree = sysconfPage->getParamPtr()->cornerDegree->value();
		param.cornerGap = sysconfPage->getParamPtr()->cornerGap->value();

		param.background = sysconfPage->getParamPtr()->backgroundGroup->isChecked();

		param.normalize = sysconfPage->getParamPtr()->normalizeGroup->isChecked();
		param.normalizeTopMargin = sysconfPage->getParamPtr()->normalizeTopMargin->value();
		param.normalizeSideMargin =sysconfPage->getParamPtr()->normalizeSideMargin->value();
		param.normalizeWidth = sysconfPage->getParamPtr()->normalizeWidth->value();
		param.normalizeHeight = sysconfPage->getParamPtr()->normalizeHeight->value();

		param.blueText =  sysconfPage->getParamPtr()->blueTextGroup->isChecked();
		param.blueTextMin = sysconfPage->getParamPtr()->blueTextMin->value();
		param.blueTextMax = sysconfPage->getParamPtr()->blueTextMax->value();

		mImageProcess->setMasks(mOCRMasks);
		mImageProcess->run(&param);

		IplImage* cvImage = mImageProcess->getProcessedImage();
		QImage* qtImage = ImageAdapter::IplImage2QImage(cvImage);
		imageWidget->setImage(qtImage);
}

/**
 * �Դ�����ͼ����ʶ��
 */
void ScanPage::recognizeText()
{
	IplImage* cvImage = mImageProcess->getProcessedImage();
	if(!cvImage)
	{
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText(tr("No image loaded.\nPlease load an image file before OCR."));
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
	if(actionEnableChinese->isChecked())
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
		msgBox.setText(tr("Can not initialize tesseract"));
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
	imageWidget->setBoxes(rects, rectCount);
	
	showRecognizeResult();
}

/**
 * �ֶ�����ģ��
 */
void ScanPage :: enableChinese()
{
		if (!strDBDir.isEmpty())
		{
				QFile file(strDBDir);
				if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
						QMessageBox msgBox;
						msgBox.setIcon(QMessageBox::Warning);
						msgBox.setText(tr("Cannot open file to read.\nMaybe this file does not exist."));
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
						msgBox.setText(tr("Parse XML failed.\nMaybe this file is not the right one."));
						msgBox.exec();
				}
				xml.clear();
				imageWidget->update();
		}
}

/**
 * �Զ�����ģ��
 */
void ScanPage ::AutoLoad()
{
		if (!strDBDir.isEmpty())
		{
				QFile file(strDBDir);
				if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
						QMessageBox msgBox;
						msgBox.setIcon(QMessageBox::Warning);
						msgBox.setText(tr("Cannot open file to read.\nMaybe this file does not exist."));
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
						msgBox.setText(tr("Parse XML failed.\nMaybe this file is not the right one."));
						msgBox.exec();
				}
				xml.clear();
				imageWidget->update();
		}
		QMessageBox::information(NULL, tr("Warmming"), tr("can not load...."));
}

/**
 * ����ģ��
 */
void ScanPage::loadMasks()
{
		QString fileName = QFileDialog::getOpenFileName(this,
				tr("Load Masks"), AppInfo::instance()->appDir().c_str(),
				tr("XML files (*.xml)"));
		if (!fileName.isEmpty())
		{
				QFile file(fileName);
				if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
				{
						QMessageBox msgBox;
						msgBox.setIcon(QMessageBox::Warning);
						msgBox.setText(tr("Cannot open file to read.\nMaybe this file does not exist."));
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
						msgBox.setText(tr("Parse XML failed.\nMaybe this file is not the right one."));
						msgBox.exec();
				}
				xml.clear();
				imageWidget->update();
				//ocrWidget->update();
		}
}

/**
 * ���ģ��
 */
void ScanPage::clearMasks()
{
		mOCRMasks->clear();
		imageWidget->update();
		//ocrWidget->update();
}

/**
 * ����ؼ���ʾ�ķ�Ʊ��Ϣ
 */
void ScanPage::clearControlsText()
{
		leditBillCode->clear();
		leditBillNO->clear();
		leditBillProvider->clear();
		leditBillCustomer->clear();
		leditBillSecret1->clear();
		leditBillSecret2->clear();
		leditBillSecret3->clear();
		leditBillSecret4->clear();
		leditBillAmount->clear();
		leditBillTax->clear();
		leditBillDate->clear();
}

/**
 * �ѷ�Ʊ��Ϣ������ݰ�
 */
QString ScanPage::createPostData()
{
		QStringList strMiwen = mBillData.billSecreatArea.split('\n');
		QTextCodec *utf8 = QTextCodec::codecForName("utf-8");
		QString strdate = leditBillDate->text();
		QStringList strTimeList = strdate.split('-');
		QString stryear = strTimeList.at(0);
		QString strmonth = strTimeList.at(1);
		QString strday = strTimeList.at(2);

		QString strpost = 
				"nsrsbh=" + leditBillCustomer->text()															//��˰��ʶ���
				+  "&czydm=" + strczydm																			//����Ա����
				+ "&passwd=" + strpasswd																			//����
				+ "&fpdm=" + leditBillCode->text()																//��Ʊ����
				+ "&fphm=" + leditBillNO->text()																	//��Ʊ����
				+ "&kpyear=" + stryear																				//��Ʊ��
				+ "&kpmonth=" + strmonth																			//��Ʊ��
				+ "&kpday=" + strday																					//��Ʊ��
				+ "&gsbh=" + leditBillCustomer->text()														//����ʶ���
				+ "&miwen1=" + utf8->fromUnicode(QString::fromUtf8(strMiwen.at(0).toUtf8())).toPercentEncoding()		//���ģ�
				+ "&miwen2=" + utf8->fromUnicode(QString::fromUtf8(strMiwen.at(1).toUtf8())).toPercentEncoding()		//���ģ�
				+ "&miwen3=" + utf8->fromUnicode(QString::fromUtf8(strMiwen.at(2).toUtf8())).toPercentEncoding()		//���ģ�
				+ "&miwen4=" + utf8->fromUnicode(QString::fromUtf8(strMiwen.at(3).toUtf8())).toPercentEncoding()		//���ģ�
				+ "&jine=" + leditBillAmount->text()															//���
				+ "&shuie=" + leditBillTax->text()																	//˰��
				+ "&xsbh=" + leditBillProvider->text()															//����ʶ���
				+ "&fpid=" + strfpid																						//��Ʊid
				+ "&action=" + straction																				//action
				;

		return strpost;
}



//temp fuction
void ScanPage::on_pbtnSave_clicked()
{		
		saveModifiedData();
		mBillData = *vBillInfo[0];

		QSqlQuery* query = new QSqlQuery;

		QString strBillType = "WAITAUTH";

		strBillCode = mBillData.billCode;
		strBillNO = mBillData.billNO;
		strBillProvider = mBillData.billProvider;
		strBillCustomer = mBillData.billCustomer;
		//QStringList strlist = mBillData.billScreatArea.split('\n');
		strBillSecret = mBillData.billSecreatArea;
		fBillAmount = mBillData.billAmount.toDouble();
		fBillTax = mBillData.billTax.toDouble();
		dRecordDate = QDate::currentDate();

		dBillDate = QDate::fromString(QString(mBillData.billDate), "yyyy-MM-dd");

		QString strSql = "insert into billtable(Bill_Code, Bill_NO, Bill_Provider, Bill_Customer, Bill_SecretArea, Bill_Amount, Bill_Tax, Bill_Date,  Bill_Type, Bill_RecordDate) values(?,?,?,?,?,?,?,?,?,?)";
		query->prepare(strSql);
		query->addBindValue(strBillCode); 
		query->addBindValue(strBillNO);
		query->addBindValue(strBillProvider);
		query->addBindValue(strBillCustomer);
		query->addBindValue(strBillSecret);
		query->addBindValue(fBillAmount);
		query->addBindValue(fBillTax);
		query->addBindValue(dBillDate);
		query->addBindValue(strBillType);
		query->addBindValue(dRecordDate);
		bool success = query->exec();	

		clearMasks();

		if(success)
				QMessageBox::information(this, "Warming", "Insert successful...");
		else
				QMessageBox::information(this, tr("��ʾ"), tr("��������ʧ�ܣ���ȷ������������ȷ���¼���Ѿ����ڣ�"));

		clearControlsText();
}
