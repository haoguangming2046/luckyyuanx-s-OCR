#ifndef WORKERTHREAD_H_
#define WORKERTHREAD_H_

#include "ScanPage.h"
#include "share/TesseractOCR.h"
#include "share/ImageProcess.h"

extern QQueue<QString> MsgQueue;				//��Ϣ����
extern QMutex mutex;										//��
extern QWaitCondition waitProduce;				
extern QWaitCondition waitConsume;
//extern ImageProcessParam g_param;

QString getPicture();
QString getModeDir();
void processImage(ImageProcess *mImageProcess, std::vector<OCRMask>* mOCRMasks, BillInfo& mBillInfo);
void loadMasks(std::vector<OCRMask>* mOCRMasks);
void recognizeText(ImageProcess *mImageProcess, std::vector<OCRMask>* mOCRMasks, TesseractOCR* mTesseractOCR);
void initBillInfo(const std::vector<OCRMask>* mOCRMasks, BillInfo& mBillInfo);

class Worker : public QRunnable
{
public:
		void run();
};

#endif