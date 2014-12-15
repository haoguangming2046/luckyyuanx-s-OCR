#include "DataBase.h"
#include <QSqlDatabase>
#include <QSqlQuery>

DataBase :: DataBase()
{

}

DataBase :: ~DataBase()
{
		db.close();
}

bool DataBase :: createConnection()
{
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName("Bill.sqlite");
		
		bool ok = db.open();
		if(ok)
				return true;
		else
				return false;
}

void DataBase :: createDatabase()
{
		QSqlQuery* query = new QSqlQuery;
		bool ok;
		query->exec("drop Bill.sqlite");

		ok = query->exec("create table billtable ("		//发票信息表
				"Bill_Code nchar(40) not null, "
				"Bill_NO nchar(40) not null, "
				"Bill_Provider nchar(40) not null, "
				"Bill_Customer nchar(40) not null, "
				"Bill_SecretArea nchar(180) not null, "
				"Bill_Amount money not null, "
				"Bill_Tax money not null, "
				"Bill_Date date not null, "
				"Bill_Type nchar(20) not null, "

				"Bill_RecordDate date not null, "
				"Bill_AuthDate date, "
				"Bill_AuthCounts int, "
				"Bill_AuthResult nchar(60), "

				"primary key(Bill_Code, Bill_NO))");

		/*ok = query->exec("insert into billtable (Bill_Code, Bill_NO, Bill_Provider, Bill_Customer, Bill_SecretArea, Bill_Amount, "
		"Bill_Tax, Bill_Date, Bill_Type, Bill_RecordDate) values ('1', '1', 'google', 'IBM', '123456', 100.00, 21.00, '2013-08-14', "
		"'WAITAUTH', '2013-08-20')");*/

		ok = query->exec("create table systemparams ("
				"key nchar(20) primary key , "
				"value nchar(40) not null)");

		ok = query->exec("insert into systemparams values('url', 'https://web1.nb-n-tax.gov.cn:7006/wlrz/fpwlrz.do')");
		ok = query->exec("insert into systemparams values('czydm', '001')");
		ok = query->exec("insert into systemparams values('passwd', '001')");
		ok = query->exec("insert into systemparams values('action', 'wrlz')");
		ok = query->exec("insert into systemparams values('fpid', '001')");
		ok = query->exec("insert into systemparams values('modedir', '/dir')");					//稍后再加				
}




