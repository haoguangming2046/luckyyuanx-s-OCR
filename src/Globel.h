#ifndef GLOBEL_H_
#define GLOBEL_H_

enum
{
		Bill_Code = 0,
		Bill_NO,
		Bill_Provider,
		Bill_Customer,
		Bill_SecreArea,
		Bill_Amount,
		Bill_Tax,
		Bill_Date,
		Bill_Type,

		Bill_RecordDate,
		Bill_AuthDate,
		Bill_AuthCounts,
		Bill_AuthResult
};


//��Ʊ����

//enum BillType
//{
//		AUTHED,	//����֤ͨ��
//		NOTPASS,		//��֤δͨ��
//		WAITAUTH		//δ��֤
//};

struct BillInfo
{
		QString fileName;				//��Ʊ
		//��Ʊ��Ϣ
		QString billCode;				
		QString billNO;
		QString billProvider;
		QString billCustomer;
		QString billSecreatArea;
		QString billAmount;
		QString billTax;
		QString billDate;
		//QString bill

};

#endif