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


//发票类型

//enum BillType
//{
//		AUTHED,	//已认证通过
//		NOTPASS,		//认证未通过
//		WAITAUTH		//未认证
//};

struct BillInfo
{
		QString fileName;				//发票
		//发票信息
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