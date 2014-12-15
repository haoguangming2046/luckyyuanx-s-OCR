#include "AuthPage.h"
#include "DataBase.h"
#include "Globel.h"

#include <QSqlQuery>
//#include <QSqlQueryView>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QStringList>
#include <QCompleter>
#include <QSqlRecord>
//network need
#include <QNetworkAccessManager>
#include <QNetworkReply>  
#include <QNetworkRequest>  
#include <QSslSocket>
#include <QSslConfiguration>

AuthPage ::AuthPage()
{
		buildUI();
		//buildModel();
}

AuthPage :: ~AuthPage()
{

}

void AuthPage::buildModel(QSqlQueryModel* model, QTableView* view)
{
		model->setHeaderData(Bill_Code, Qt::Horizontal, tr("发票代码"));
		model->setHeaderData(Bill_NO, Qt::Horizontal, tr("发票号码"));
		model->setHeaderData(Bill_Provider, Qt::Horizontal, tr("销方税号"));
		model->setHeaderData(Bill_Customer, Qt::Horizontal, tr("购方税号"));
		model->setHeaderData(Bill_SecreArea, Qt::Horizontal, tr("密码区"));
		model->setHeaderData(Bill_Amount, Qt::Horizontal, tr("金额"));
		model->setHeaderData(Bill_Tax, Qt::Horizontal, tr("税额"));
		model->setHeaderData(Bill_Date, Qt::Horizontal, tr("开票日期"));
		model->setHeaderData(Bill_Type, Qt::Horizontal, tr("发票类型"));
		model->setHeaderData(Bill_RecordDate, Qt::Horizontal, tr("录入日期"));
		model->setHeaderData(Bill_AuthDate, Qt::Horizontal, tr("认证日期"));
		model->setHeaderData(Bill_AuthCounts, Qt::Horizontal, tr("认证次数"));
		model->setHeaderData(Bill_AuthResult, Qt::Horizontal, tr("认证结果"));

		view->setModel(model);
		view->setSelectionBehavior(QAbstractItemView::SelectRows);
		view->resizeColumnToContents(4);
		//QueryView->setEditTriggers(QAbstractItemView::AllEditTriggers);

		QHeaderView* header = view->horizontalHeader();
		header->setStretchLastSection(true);
		//header->setVisible(true);
}

void AuthPage :: buildUI()
{
		//build data variants
		QueryView = new QTableView;
		authResultView = new QTableView;
		queryModel = new MySqlQueryModel;
		authResultModel = new QSqlQueryModel;
		sslConfig = new QSslConfiguration;
		networkRequest = new QNetworkRequest;
		//build UI controls
		QLabel* labelBillCode = new QLabel(tr("发票代码"));
		leditBillCode = new QLineEdit;
		QLabel* labelBillNO = new QLabel(tr("发票号码"));
		leditBillNOStart = new QLineEdit;
		chkZhi = new QCheckBox(tr("至"));
		leditBillNOEnd = new QLineEdit();
		leditBillNOEnd->setEnabled(false);

		rbtnAll = new QRadioButton(tr("未认证/认证未通过发票"));
		rbtnAll->setChecked(true);
		rbtnNotPass = new QRadioButton(tr("认证未通过发票"));
		rbtnWaitAuth = new QRadioButton(tr("未认证发票"));

		chkBillRecordDate = new QCheckBox(tr("发票录入时间"));
		deditRecordDateStart = new QDateEdit;
		deditRecordDateStart->setDate(QDate::currentDate());
		deditRecordDateStart->setEnabled(false);
		QLabel* labelTemp = new QLabel(tr("--->>"));
		deditRecordDateEnd = new QDateEdit;
		deditRecordDateEnd->setEnabled(false);
		deditRecordDateEnd->setDate(QDate::currentDate());
		btnQuery = new QPushButton(tr("查询记录"));

		QueryView = new QTableView;
		btnStartAuth = new QPushButton(tr("开始认证"));
		chkSelectAll = new QCheckBox(tr("全选"));
		chkAuthAsRows = new QCheckBox(tr("逐行认证"));
		chkAuthAsRows->setEnabled(false);
		chkAuthAsRows->setChecked(true);

		//layout
		QHBoxLayout* hlayout1 = new QHBoxLayout;
		hlayout1->addWidget(labelBillCode);
		hlayout1->addWidget(leditBillCode);
		hlayout1->addWidget(labelBillNO);
		hlayout1->addWidget(leditBillNOStart);
		hlayout1->addWidget(chkZhi);
		hlayout1->addWidget(leditBillNOEnd);
		hlayout1->addStretch();

		QHBoxLayout* hlayout2 = new QHBoxLayout;
		hlayout2->addWidget(rbtnAll);
		hlayout2->addWidget(rbtnWaitAuth);
		hlayout2->addWidget(rbtnNotPass);	
		hlayout2->addStretch();

		QHBoxLayout* hlayout3 = new QHBoxLayout;
		hlayout3->addWidget(chkBillRecordDate);
		hlayout3->addWidget(deditRecordDateStart);
		hlayout3->addWidget(labelTemp);
		hlayout3->addWidget(deditRecordDateEnd);
		hlayout3->addStretch();
		hlayout3->addWidget(btnQuery);

		QVBoxLayout* toplayout = new QVBoxLayout;
		toplayout->addLayout(hlayout1);
		toplayout->addLayout(hlayout2);
		toplayout->addLayout(hlayout3);

		QVBoxLayout* vlayout = new QVBoxLayout;
		vlayout->addWidget(btnStartAuth);
		vlayout->addWidget(chkSelectAll);
		vlayout	->addWidget(chkAuthAsRows);
		vlayout->addStretch();

		QHBoxLayout* middlelayout = new QHBoxLayout;
		middlelayout->addWidget(QueryView);
		middlelayout->addLayout(vlayout);				

		QVBoxLayout* mainlayout = new QVBoxLayout;
		mainlayout->addLayout(toplayout);
		mainlayout->addLayout(middlelayout);
		mainlayout->addWidget(authResultView);

		setLayout(mainlayout);
		//DataBase::createDatabase();

		//connect
		connect(chkZhi, SIGNAL(toggled(bool)), this, SLOT(on_chkZhi_clicked()));
		connect(chkBillRecordDate, SIGNAL(toggled(bool)), this, SLOT(on_chkBillRecordDate_clicked()));
		connect(chkSelectAll, SIGNAL(toggled(bool)), this, SLOT(on_chkSelectAll_clicked()));
		connect(btnQuery, SIGNAL(clicked()), this, SLOT(on_btnQuery_clicked()));
		connect(btnStartAuth, SIGNAL(clicked()), this, SLOT(on_btnStartAuth_clicked()));
		//QCompleter and QStringList
		QStringList BillCodeList, BillNOList;
		QSqlQuery* queryForStringList = new QSqlQuery;
		bool ok = queryForStringList->exec("select * from billtable");
		while(queryForStringList->next())
		{ 
				BillCodeList<<queryForStringList->value(0).toString();
				BillNOList<<queryForStringList->value(1).toString();
		}
		QCompleter* completerBillCode = new QCompleter(BillCodeList); 
		QCompleter* completerBillNOStart = new QCompleter(BillNOList);
		QCompleter* completerBillNOEnd = new QCompleter(BillNOList);

		completerBillCode->setCompletionMode(QCompleter::InlineCompletion);//完整的匹配Inline
		completerBillNOStart->setCompletionMode(QCompleter::InlineCompletion);
		completerBillNOEnd->setCompletionMode(QCompleter::InlineCompletion);
		//completer->setCompletionMode(QCompleter::PopupCompletion); //匹配以弹出式显示
		leditBillCode->setCompleter(completerBillCode);
		leditBillNOStart->setCompleter(completerBillNOStart);
		leditBillNOEnd->setCompleter(completerBillNOEnd);

		//获取系统设置中的fpid, action, czydm, passwd
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
		//网络成员
		networkAccessManager = new QNetworkAccessManager(this);
		connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
		networkRequest->setUrl(strurl);
		networkRequest->setHeader(QNetworkRequest::ContentTypeHeader,QString("application/x-www-form-urlencoded").toUtf8());
		//配置SSL
		sslConfig->setPeerVerifyMode(QSslSocket::VerifyNone);
		sslConfig->setProtocol(QSsl::TlsV1);
		networkRequest->setSslConfiguration(*sslConfig);

		rbtnAll->setEnabled(false);
		rbtnNotPass->setChecked(true);
}

QSqlQuery AuthPage::staticQuery = NULL;//静态成员，用来保存查询得到的Query
QSqlRecord AuthPage::staticRecord;

bool AuthPage ::updateAuthResult(const QString & strResult, const QString & strType)
{
		QSqlQuery query;
		int n = staticRecord.value(11).toInt();
		QString strSql = "update billtable set Bill_Type = ?, Bill_AuthDate = ?,  Bill_AuthCounts = ?, Bill_AuthResult = ? "
				"where Bill_Code = ? and Bill_NO = ? ";
		query.prepare(strSql);
		query.addBindValue(strType);
		query.addBindValue(QDate::currentDate());
		query.addBindValue(staticRecord.value(11).toInt() + 1);
		query.addBindValue(strResult);
		query.addBindValue(staticRecord.value(0).toString());
		query.addBindValue(staticRecord.value(1).toString());

		return query.exec();
}
#include <QSqlTableModel>
//build slot
void AuthPage ::finishedSlot(QNetworkReply *reply)
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
						updateAuthResult(strAuthResult, strType);
				}
				else
				{
						QString strType = "NOTPASS";
						updateAuthResult(strAuthResult, strType);
				}
				QSqlQuery query;
				query.prepare("select * from billtable where Bill_Code = ? and Bill_NO = ?");
				query.addBindValue(staticRecord.value(0).toString());
				query.addBindValue(staticRecord.value(1).toString());
				QString strstatic = staticRecord.value(0).toString();
				query.exec();
				authResultModel->setQuery(query);
				buildModel(authResultModel, authResultView);
		}  		
		else 
		{ 
				QByteArray bytes = reply->readAll(); 
				QString string = QString::fromUtf8(bytes);
				QMessageBox::information(this, "Warmming Error", string);
		} 
		queryModel->refresh();
		reply->deleteLater();  
}

void AuthPage ::on_btnStartAuth_clicked()
{
		if(!networkRequest->url().isValid())
		{
				QMessageBox::information(this, tr("提示"), tr("认证URL不可用，请检查系统设置！"));
				return;
		}
		QItemSelectionModel* selectionModel = QueryView->selectionModel();
		//QueryView->set
		QModelIndexList indexList = selectionModel->selectedIndexes();
		QModelIndex index;
		int ncount = indexList.count();//debug
		if(indexList.count() == 0)
		{
				QMessageBox::information(this, tr("提示"), tr("请选中需要认证的行"));
				return;
		}
		QMap<int, int> rowMap;
		foreach(index, indexList) 
		{
				int nrow = index.row();//debug
				rowMap.insert(index.row(), 0);				
		}
		QMapIterator<int, int> rowMapIterator(rowMap);
		QSqlRecord record;
		QString strData;
		
		while(rowMapIterator.hasNext())
		{
				rowMapIterator.next();
				int nkey = rowMapIterator.key();//debug
				record = queryModel->record(rowMapIterator.key());
				staticRecord = record;//保存当前record供reply使用
				QString strno = staticRecord.value(1).toString();
				strData = createPostData(record);
				networkRequest->setHeader(QNetworkRequest::ContentLengthHeader, strData.length());
				networkReply = networkAccessManager->post(*networkRequest, strData.toUtf8());
				QString value = record.value(1).toString();
		}	
		
}

QVector<QString> AuthPage::stringSplit(QString& strSecret, char *c)
{
		char *cstr, *p;
		//QMessageBox::information(this, tr("提示"), strSecret);
		QVector<QString> res;
		int n = strSecret.size();//debug
		cstr = new char[strSecret.size()+1];
		strcpy(cstr,strSecret.toStdString().c_str());
		//QMessageBox::information(this, tr("提示"), cstr);
		QStringList strlist = strSecret.split('\n');
		p = strtok(cstr, c);
		while(p != NULL)
		{
				QMessageBox::information(this, tr("提示"), p);
				res.push_back(p);
				p = strtok(NULL,c);
		}
		return res;
}

QString AuthPage::createPostData(const QSqlRecord& record)
{
		QStringList strlist = record.value(Bill_SecreArea).toString().split('\n');		
		QTextCodec *utf8 = QTextCodec::codecForName("utf-8");		
		
		QString strdate = record.value(Bill_Date).toString();
		QStringList strTimeList = strdate.split('-');
		QString stryear = strTimeList.at(0);
		QString strmonth = strTimeList.at(1);
		QString strday = strTimeList.at(2);
		
		QString strmiwen1 = strlist.at(0);
		QString strmiwen2 = strlist.at(1);
		QString strmiwen3 = strlist.at(2);
		QString strmiwen4 = strlist.at(3);
		QString strpost = 
				   "nsrsbh=" + record.value(Bill_Customer).toString()															//纳税人识别号
			  +  "&czydm=" + strczydm														//操作员代码
			  + "&passwd=" + strpasswd													//口令
			  + "&fpdm=" + record.value(Bill_Code).toString()						//发票代码
			  + "&fphm=" + record.value(Bill_NO).toString()						//发票号码
			  + "&kpyear=" + stryear			//开票年
			  + "&kpmonth=" + strmonth	//开票月
			  + "&kpday=" + strday				//开票日
			  + "&gsbh=" + record.value(Bill_Customer).toString()				//购方识别号
		      + "&miwen1=" + utf8->fromUnicode(QString::fromUtf8(strmiwen1.toUtf8())).toPercentEncoding()														//密文１
		      + "&miwen2=" + utf8->fromUnicode(QString::fromUtf8(strmiwen2.toUtf8())).toPercentEncoding()														//密文２
		      + "&miwen3=" + utf8->fromUnicode(QString::fromUtf8(strmiwen3.toUtf8())).toPercentEncoding()														//密文３
		      + "&miwen4=" + utf8->fromUnicode(QString::fromUtf8(strmiwen4.toUtf8())).toPercentEncoding()														//密文４
		      + "&jine=" + record.value(Bill_Amount).toString()					//金额
		      + "&shuie=" +record.value(Bill_Tax).toString()							//税额
		      + "&xsbh=" + record.value(Bill_Provider).toString()					//销方识别号
			  + "&fpid=" + strfpid																//发票id
			  + "&action=" + straction														//action
		;
				//QMessageBox::information(this, tr("warmming"), strpost);
			  return strpost;
}

void AuthPage ::on_btnQuery_clicked()
{
		query = new QSqlQuery;
		QString strSql;
		//用来判断发票号码是否为空值
		QString strBillNO = leditBillNOStart->text();				
		//获取控件数据
		strBillCode = leditBillCode->text();
		nBillNOStart = leditBillNOStart->text();						//注意nBillNOStart, nBillNOEnd类型：QString
		nBillNOEnd = leditBillNOEnd->text();
	
		dRecordDateStart = QDate::fromString(deditRecordDateStart->text(), "yyyy/MM/dd");
		dRecordDateEnd = QDate::fromString(deditRecordDateEnd->text(), "yyyy/MM/dd");
		//在未认证与认证未通过的发票中查询
		if(rbtnAll->isChecked())														
		{
		
		}
		//在未认证的发票中查询	
		else if(rbtnWaitAuth->isChecked())														
		{
				strBillType = "WAITAUTH";
				if(chkZhi->isChecked())//选中发票号码就不为空
				{
						if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())		//按号码、代码、日期查询 + 类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ?  and Bill_NO between ? and ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(!strBillCode.isEmpty() && !chkBillRecordDate->isChecked())		//按号码、代码查询 + 类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//按号码、日期查询 +　类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_NO between ? and ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else																									//按号码查询			+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_NO between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
						}
				}	
				else
				{
						if(!strBillCode.isEmpty() && !strBillNO.isEmpty())										//按代码、号码查询	+类型　　　　结果唯一
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
						}
						else if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())				//按代码、日期查询　+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//按日期查询　　+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(!strBillCode.isEmpty())															//按代码查询					+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
						}
						else 
						{
								strSql = "select * from billtable where Bill_Type = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
						}
				}
		}
		//在认证未通过的发票中查询
		else																					
		{
				strBillType = "NOTPASS";
				if(chkZhi->isChecked())//选中发票号码就不为空
				{
						if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())		//按号码、代码、日期查询 + 类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ?  and Bill_NO between ? and ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(!strBillCode.isEmpty() && !chkBillRecordDate->isChecked())		//按号码、代码查询 + 类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//按号码、日期查询 +　类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_NO between ? and ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else																									//按号码查询			+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_NO between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
						}
				}	
				else
				{
						if(!strBillCode.isEmpty() && !strBillNO.isEmpty())										//按代码、号码查询	+类型　　　　结果唯一
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
						}
						else if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())				//按代码、日期查询　+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//按日期查询　　+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(!strBillCode.isEmpty())															//按代码查询					+类型
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
						}
						else 
						{
								strSql = "select * from billtable where Bill_Type = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
						}
				}
		}
		
		staticQuery = *query;//静态成员赋值
		bool success = query->exec();		
		queryModel->setQuery(*query);
		buildModel(queryModel, QueryView);
		if(!success)
				QMessageBox::information(this, "Warming", "Query failed...");
}

void AuthPage ::on_chkBillRecordDate_clicked()
{
		static bool choosed = true;
		if(choosed)
		{
				deditRecordDateStart->setEnabled(true);
				deditRecordDateEnd->setEnabled(true);
		}
		else
		{
				deditRecordDateStart->setEnabled(false);
				deditRecordDateEnd->setEnabled(false);
		}
		choosed = !choosed;
}

void AuthPage ::on_chkZhi_clicked()
{
		static bool choose = true;

		if(choose)
				leditBillNOEnd->setEnabled(true);
		else
				leditBillNOEnd->setEnabled(false);

		choose = !choose;
}

void AuthPage ::on_chkSelectAll_clicked()
{
		static bool isselect = true;
		if(isselect)
		{
				QueryView->selectAll();
		}
		else
				QueryView->clearSelection();

		isselect = !isselect;
}

//下面是MySqlQueryModel的实现
MySqlQueryModel::MySqlQueryModel()
{
		
}

Qt::ItemFlags MySqlQueryModel :: flags(const QModelIndex &index) const
{
		Qt::ItemFlags flags = QSqlQueryModel::flags(index);  
		int ncolumn = index.column();
		if (ncolumn >= 0 && ncolumn < 8) //设置0－7的字段可编辑 
				flags |= Qt::ItemIsEditable;  
		return flags;  
}

bool MySqlQueryModel ::setData(const QModelIndex &index, const QVariant &value, int role )
{
		int nColumn = index.column();//属性列
		if(nColumn < 0 || nColumn > 7)
				return false;

		QModelIndex primaryKeyIndex1 = QSqlQueryModel::index(index.row(), 0); 
		QModelIndex primaryKeyIndex2 = QSqlQueryModel::index(index.row(), 1);
		QString strBillCode = data(primaryKeyIndex1).toString(); 
		QString strBillNO = data(primaryKeyIndex2).toString();

		clear();  
		bool isOk; 
		switch(nColumn)
		{
		case 0:
				isOk = setBillCode(strBillCode, strBillNO, value.toString());break;
		case 1:
				isOk = setBillNO(strBillCode, strBillNO, value.toString()); break;
		case 2:
				isOk = setProvider(strBillCode, strBillNO, value.toString()); break;
		case 3:
				isOk = setCustomer(strBillCode, strBillNO, value.toString()); break;
		case 4:
				isOk = setSecretArea(strBillCode, strBillNO, value.toString()); break;
		case 5:
				isOk = setAmount(strBillCode, strBillNO, value.toFloat()); break;
		case 6:
				isOk = setBillTax(strBillCode, strBillNO, value.toFloat()); break;
		case 7:
				isOk = setBillDate(strBillCode, strBillNO, value.toDate()); break;
		}
		refresh();
		return isOk;
}

QVariant MySqlQueryModel :: data(const QModelIndex &index, int role )
{
		QVariant value = QSqlQueryModel::data(index, role);  
		if (role == Qt::TextColorRole && index.column() == 0)  
				return qVariantFromValue(QColor(Qt::red)); //第一个属性的字体颜色为红色   
		return value;  
}

bool MySqlQueryModel:: setBillCode(const QString &strBillCode, const QString & strBillNO, const QString& strValue)
{
		QSqlQuery query;  
		query.prepare("update billtable set Bill_Code = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(strValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();  
}

bool MySqlQueryModel::setBillNO(const QString &strBillCode, const QString & strBillNO, const QString & strValue)
{
		QSqlQuery query;
		query.prepare("update billtable set Bill_NO = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(strValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();
}

bool MySqlQueryModel::setProvider(const QString &strBillCode, const QString & strBillNO, const QString & strValue)
{
		QSqlQuery query;
		query.prepare("update billtable set Bill_Provider = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(strValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();
}

bool MySqlQueryModel::setCustomer(const QString &strBillCode, const QString & strBillNO, const QString & strValue)
{
		QSqlQuery query;
		query.prepare("update billtable set Bill_Customer = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(strValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();
}

bool MySqlQueryModel::setSecretArea(const QString &strBillCode, const QString & strBillNO, const QString & strValue)
{
		QSqlQuery query;
		query.prepare("update billtable set Bill_SecretArea = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(strValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();
}

bool MySqlQueryModel::setAmount(const QString &strBillCode, const QString & strBillNO, float fValue)
{
		QSqlQuery query;
		query.prepare("update billtable set Bill_Amount = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(fValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();
}

bool MySqlQueryModel::setBillTax(const QString &strBillCode, const QString & strBillNO, float fValue)
{
		QSqlQuery query;
		query.prepare("update billtable set Bill_Tax = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(fValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();
}

bool MySqlQueryModel::setBillDate(const QString &strBillCode, const QString & strBillNO, const QDate & dValue)
{
		QSqlQuery query;
		query.prepare("update billtable set Bill_Date = ? where Bill_Code = ? and Bill_NO = ?");  
		query.addBindValue(dValue);  
		query.addBindValue(strBillCode); 
		query.addBindValue(strBillNO);

		return query.exec();
}

void MySqlQueryModel :: refresh()
{
		QSqlQuery query = AuthPage::staticQuery;
		query.exec();
		setQuery(query);
		setHeaderData(Bill_Code, Qt::Horizontal, tr("发票代码"));
		setHeaderData(Bill_NO, Qt::Horizontal, tr("发票号码"));
		setHeaderData(Bill_Provider, Qt::Horizontal, tr("销方税号"));
		setHeaderData(Bill_Customer, Qt::Horizontal, tr("购方税号"));
		setHeaderData(Bill_SecreArea, Qt::Horizontal, tr("密码区"));
		setHeaderData(Bill_Amount, Qt::Horizontal, tr("金额"));
		setHeaderData(Bill_Tax, Qt::Horizontal, tr("税额"));
		setHeaderData(Bill_Date, Qt::Horizontal, tr("开票日期"));
		setHeaderData(Bill_Type, Qt::Horizontal, tr("发票类型"));
		setHeaderData(Bill_RecordDate, Qt::Horizontal, tr("录入日期"));
		setHeaderData(Bill_AuthDate, Qt::Horizontal, tr("认证日期"));
		setHeaderData(Bill_AuthCounts, Qt::Horizontal, tr("认证次数"));
		setHeaderData(Bill_AuthResult, Qt::Horizontal, tr("认证结果"));
}