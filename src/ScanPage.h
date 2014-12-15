#ifndef SCANPAGE_H_
#define SCANPAGE_H_

#include <QtGui>
#include <vector>

#include "Globel.h"
#include "Share/ImageProcess.h"

class ImageWidget;
class AbbyyOCR;
class TesseractOCR;
class ImageProcess;
class OCRMask;
class QTwain;
class CDIB;
class QSqlQuery;
class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;
class QSslConfiguration;

class SysconfPage;
class Worker;

//ȫ�ֱ���
extern QQueue<QString> MsgQueue;				//��Ϣ����
extern QMutex mutex;										//��
extern QWaitCondition waitProduce;				
extern QWaitCondition waitConsume;

class ScanPage : public QWidget
{
		Q_OBJECT

public:
		ScanPage();
		~ScanPage();

		//friend class Worker;
		ImageWidget* getImageWidgetPtr();
		ImageProcess* getImageProcessPtr();
		void showRecognizeResult();
		void clearMasks();
		void clearControlsText();
		QString createPostData();
		bool insertAuthData(QString & strType, QString& strResult);
		void releasevBillInfo();

protected:
		void showEvent(QShowEvent* thisEvent);
		bool winEvent(MSG* pMsg, long* result);
		void getImageProcessParam(ImageProcessParam& param);

		public slots:
				void on_pbtnSave_clicked();//temp fuction

				void on_pbtnRecognize_click();
				void saveData();
				void on_pbtnAuth_clicked();

				void onDibAcquired(CDIB* pDib);
				void recognizeText();
				void scanImage();
				void enableChinese();
				void loadMasks();
				void AutoLoad();
				void processImage();
				void finishedSlot(QNetworkReply*);

				void showFirst();
				void showPre();
				void showNext();
				void showLast();
				void inputByHand();

private:
		void buildUI();
		void showBillInfo(const BillInfo& billInfo);
		void saveModifiedData();//�����޸Ĺ�����Ʊ��Ϣ��vBillinfo
		void insertDataToDB(const BillInfo& billInfo);
		//controls
		ImageWidget* imageWidget;

		QPushButton* pbtnScan;			//temp
		QPushButton* pbtnAuth;			//temp
		QPushButton* pbtnSave;			//temp

		QPushButton* pbtnProcessImage;			//temp
		QPushButton* pbtnRecognizeText;			//temp
		QPushButton* pbtnLoadMasks;						//temp

		QLineEdit* leditBillCode;
		QLineEdit* leditBillNO;
		QLineEdit* leditBillProvider;
		QLineEdit* leditBillCustomer;
		QLineEdit* leditBillSecret1;
		QLineEdit* leditBillSecret2;
		QLineEdit* leditBillSecret3;
		QLineEdit* leditBillSecret4;
		QLineEdit* leditBillAmount;
		QLineEdit* leditBillTax;
		QLineEdit* leditBillDate;
		QLineEdit* leditBillVersion;//���ܰ汾

		QPushButton* pbtnFirst;
		QPushButton* pbtnPre;
		QPushButton* pbtnNext;
		QPushButton* pbtnLast;

		QPushButton* pbtnSingleScan;
		QPushButton* pbtnMultiScan;
		QPushButton* pbtnMunual;				//�ֹ�¼��
		QPushButton* pbtnSaveData;

		QLabel* labelCurrent;
		QLabel* labelTotal;

		QAction* actionEnableChinese;

		//internal variables
		QTwain* mTwain;

		//AbbyyOCR* mAbbyyOCR;
		TesseractOCR* mTesseractOCR;
		ImageProcess* mImageProcess;

		std::vector<OCRMask>* mOCRMasks;

		SysconfPage* sysconfPage;		//������ȡparam�����������д�����

		//database operation variants
		QString strBillCode, strBillProvider, strBillCustomer, strBillSecret, strBillType;
		QString strBillNO;
		float fBillAmount, fBillTax;
		QDate dBillDate, dRecordDate;

		QString strurl, strfpid, strczydm, strpasswd, straction, strnsrsbh;//������֤��Ҫ�ļ������ݶ�ֵ
		QString strDBDir;//�������ݿ������õ�ģ��

		//variants about network
		QNetworkAccessManager* networkAccessManager;
		QSslConfiguration* sslConfig;
		QNetworkRequest* networkRequest;
		QNetworkReply* networkReply;
		//thread
		QThreadPool* threadpool;
		Worker* worker;
		//���淢Ʊ����
		BillInfo mBillData;
		int nCurrentBill;
		bool bAuto;//true��ʾ��ɨ���ǣ�false��ʾ�ֶ�¼��
		QStringList mlistBillSectret;
};

#endif