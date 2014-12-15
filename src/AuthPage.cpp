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
		model->setHeaderData(Bill_Code, Qt::Horizontal, tr("��Ʊ����"));
		model->setHeaderData(Bill_NO, Qt::Horizontal, tr("��Ʊ����"));
		model->setHeaderData(Bill_Provider, Qt::Horizontal, tr("����˰��"));
		model->setHeaderData(Bill_Customer, Qt::Horizontal, tr("����˰��"));
		model->setHeaderData(Bill_SecreArea, Qt::Horizontal, tr("������"));
		model->setHeaderData(Bill_Amount, Qt::Horizontal, tr("���"));
		model->setHeaderData(Bill_Tax, Qt::Horizontal, tr("˰��"));
		model->setHeaderData(Bill_Date, Qt::Horizontal, tr("��Ʊ����"));
		model->setHeaderData(Bill_Type, Qt::Horizontal, tr("��Ʊ����"));
		model->setHeaderData(Bill_RecordDate, Qt::Horizontal, tr("¼������"));
		model->setHeaderData(Bill_AuthDate, Qt::Horizontal, tr("��֤����"));
		model->setHeaderData(Bill_AuthCounts, Qt::Horizontal, tr("��֤����"));
		model->setHeaderData(Bill_AuthResult, Qt::Horizontal, tr("��֤���"));

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
		QLabel* labelBillCode = new QLabel(tr("��Ʊ����"));
		leditBillCode = new QLineEdit;
		QLabel* labelBillNO = new QLabel(tr("��Ʊ����"));
		leditBillNOStart = new QLineEdit;
		chkZhi = new QCheckBox(tr("��"));
		leditBillNOEnd = new QLineEdit();
		leditBillNOEnd->setEnabled(false);

		rbtnAll = new QRadioButton(tr("δ��֤/��֤δͨ����Ʊ"));
		rbtnAll->setChecked(true);
		rbtnNotPass = new QRadioButton(tr("��֤δͨ����Ʊ"));
		rbtnWaitAuth = new QRadioButton(tr("δ��֤��Ʊ"));

		chkBillRecordDate = new QCheckBox(tr("��Ʊ¼��ʱ��"));
		deditRecordDateStart = new QDateEdit;
		deditRecordDateStart->setDate(QDate::currentDate());
		deditRecordDateStart->setEnabled(false);
		QLabel* labelTemp = new QLabel(tr("--->>"));
		deditRecordDateEnd = new QDateEdit;
		deditRecordDateEnd->setEnabled(false);
		deditRecordDateEnd->setDate(QDate::currentDate());
		btnQuery = new QPushButton(tr("��ѯ��¼"));

		QueryView = new QTableView;
		btnStartAuth = new QPushButton(tr("��ʼ��֤"));
		chkSelectAll = new QCheckBox(tr("ȫѡ"));
		chkAuthAsRows = new QCheckBox(tr("������֤"));
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

		completerBillCode->setCompletionMode(QCompleter::InlineCompletion);//������ƥ��Inline
		completerBillNOStart->setCompletionMode(QCompleter::InlineCompletion);
		completerBillNOEnd->setCompletionMode(QCompleter::InlineCompletion);
		//completer->setCompletionMode(QCompleter::PopupCompletion); //ƥ���Ե���ʽ��ʾ
		leditBillCode->setCompleter(completerBillCode);
		leditBillNOStart->setCompleter(completerBillNOStart);
		leditBillNOEnd->setCompleter(completerBillNOEnd);

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
		//�����Ա
		networkAccessManager = new QNetworkAccessManager(this);
		connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
		networkRequest->setUrl(strurl);
		networkRequest->setHeader(QNetworkRequest::ContentTypeHeader,QString("application/x-www-form-urlencoded").toUtf8());
		//����SSL
		sslConfig->setPeerVerifyMode(QSslSocket::VerifyNone);
		sslConfig->setProtocol(QSsl::TlsV1);
		networkRequest->setSslConfiguration(*sslConfig);

		rbtnAll->setEnabled(false);
		rbtnNotPass->setChecked(true);
}

QSqlQuery AuthPage::staticQuery = NULL;//��̬��Ա�����������ѯ�õ���Query
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
				QMessageBox::information(this, tr("��ʾ"), tr("��֤URL�����ã�����ϵͳ���ã�"));
				return;
		}
		QItemSelectionModel* selectionModel = QueryView->selectionModel();
		//QueryView->set
		QModelIndexList indexList = selectionModel->selectedIndexes();
		QModelIndex index;
		int ncount = indexList.count();//debug
		if(indexList.count() == 0)
		{
				QMessageBox::information(this, tr("��ʾ"), tr("��ѡ����Ҫ��֤����"));
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
				staticRecord = record;//���浱ǰrecord��replyʹ��
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
		//QMessageBox::information(this, tr("��ʾ"), strSecret);
		QVector<QString> res;
		int n = strSecret.size();//debug
		cstr = new char[strSecret.size()+1];
		strcpy(cstr,strSecret.toStdString().c_str());
		//QMessageBox::information(this, tr("��ʾ"), cstr);
		QStringList strlist = strSecret.split('\n');
		p = strtok(cstr, c);
		while(p != NULL)
		{
				QMessageBox::information(this, tr("��ʾ"), p);
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
				   "nsrsbh=" + record.value(Bill_Customer).toString()															//��˰��ʶ���
			  +  "&czydm=" + strczydm														//����Ա����
			  + "&passwd=" + strpasswd													//����
			  + "&fpdm=" + record.value(Bill_Code).toString()						//��Ʊ����
			  + "&fphm=" + record.value(Bill_NO).toString()						//��Ʊ����
			  + "&kpyear=" + stryear			//��Ʊ��
			  + "&kpmonth=" + strmonth	//��Ʊ��
			  + "&kpday=" + strday				//��Ʊ��
			  + "&gsbh=" + record.value(Bill_Customer).toString()				//����ʶ���
		      + "&miwen1=" + utf8->fromUnicode(QString::fromUtf8(strmiwen1.toUtf8())).toPercentEncoding()														//���ģ�
		      + "&miwen2=" + utf8->fromUnicode(QString::fromUtf8(strmiwen2.toUtf8())).toPercentEncoding()														//���ģ�
		      + "&miwen3=" + utf8->fromUnicode(QString::fromUtf8(strmiwen3.toUtf8())).toPercentEncoding()														//���ģ�
		      + "&miwen4=" + utf8->fromUnicode(QString::fromUtf8(strmiwen4.toUtf8())).toPercentEncoding()														//���ģ�
		      + "&jine=" + record.value(Bill_Amount).toString()					//���
		      + "&shuie=" +record.value(Bill_Tax).toString()							//˰��
		      + "&xsbh=" + record.value(Bill_Provider).toString()					//����ʶ���
			  + "&fpid=" + strfpid																//��Ʊid
			  + "&action=" + straction														//action
		;
				//QMessageBox::information(this, tr("warmming"), strpost);
			  return strpost;
}

void AuthPage ::on_btnQuery_clicked()
{
		query = new QSqlQuery;
		QString strSql;
		//�����жϷ�Ʊ�����Ƿ�Ϊ��ֵ
		QString strBillNO = leditBillNOStart->text();				
		//��ȡ�ؼ�����
		strBillCode = leditBillCode->text();
		nBillNOStart = leditBillNOStart->text();						//ע��nBillNOStart, nBillNOEnd���ͣ�QString
		nBillNOEnd = leditBillNOEnd->text();
	
		dRecordDateStart = QDate::fromString(deditRecordDateStart->text(), "yyyy/MM/dd");
		dRecordDateEnd = QDate::fromString(deditRecordDateEnd->text(), "yyyy/MM/dd");
		//��δ��֤����֤δͨ���ķ�Ʊ�в�ѯ
		if(rbtnAll->isChecked())														
		{
		
		}
		//��δ��֤�ķ�Ʊ�в�ѯ	
		else if(rbtnWaitAuth->isChecked())														
		{
				strBillType = "WAITAUTH";
				if(chkZhi->isChecked())//ѡ�з�Ʊ����Ͳ�Ϊ��
				{
						if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())		//�����롢���롢���ڲ�ѯ + ����
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
						else if(!strBillCode.isEmpty() && !chkBillRecordDate->isChecked())		//�����롢�����ѯ + ����
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//�����롢���ڲ�ѯ +������
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
						else																									//�������ѯ			+����
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
						if(!strBillCode.isEmpty() && !strBillNO.isEmpty())										//�����롢�����ѯ	+���͡����������Ψһ
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
						}
						else if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())				//�����롢���ڲ�ѯ��+����
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//�����ڲ�ѯ����+����
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(!strBillCode.isEmpty())															//�������ѯ					+����
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
		//����֤δͨ���ķ�Ʊ�в�ѯ
		else																					
		{
				strBillType = "NOTPASS";
				if(chkZhi->isChecked())//ѡ�з�Ʊ����Ͳ�Ϊ��
				{
						if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())		//�����롢���롢���ڲ�ѯ + ����
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
						else if(!strBillCode.isEmpty() && !chkBillRecordDate->isChecked())		//�����롢�����ѯ + ����
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
								query->addBindValue(nBillNOEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//�����롢���ڲ�ѯ +������
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
						else																									//�������ѯ			+����
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
						if(!strBillCode.isEmpty() && !strBillNO.isEmpty())										//�����롢�����ѯ	+���͡����������Ψһ
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and Bill_NO = ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(nBillNOStart);
						}
						else if(!strBillCode.isEmpty() && chkBillRecordDate->isChecked())				//�����롢���ڲ�ѯ��+����
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_Code = ? and "
										"Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(strBillCode);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(strBillCode.isEmpty() && chkBillRecordDate->isChecked())			//�����ڲ�ѯ����+����
						{
								strSql = "select * from billtable where Bill_Type = ? and Bill_RecordDate between ? and ?";
								query->prepare(strSql);
								query->addBindValue(strBillType);
								query->addBindValue(dRecordDateStart);
								query->addBindValue(dRecordDateEnd);
						}
						else if(!strBillCode.isEmpty())															//�������ѯ					+����
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
		
		staticQuery = *query;//��̬��Ա��ֵ
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

//������MySqlQueryModel��ʵ��
MySqlQueryModel::MySqlQueryModel()
{
		
}

Qt::ItemFlags MySqlQueryModel :: flags(const QModelIndex &index) const
{
		Qt::ItemFlags flags = QSqlQueryModel::flags(index);  
		int ncolumn = index.column();
		if (ncolumn >= 0 && ncolumn < 8) //����0��7���ֶοɱ༭ 
				flags |= Qt::ItemIsEditable;  
		return flags;  
}

bool MySqlQueryModel ::setData(const QModelIndex &index, const QVariant &value, int role )
{
		int nColumn = index.column();//������
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
				return qVariantFromValue(QColor(Qt::red)); //��һ�����Ե�������ɫΪ��ɫ   
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
		setHeaderData(Bill_Code, Qt::Horizontal, tr("��Ʊ����"));
		setHeaderData(Bill_NO, Qt::Horizontal, tr("��Ʊ����"));
		setHeaderData(Bill_Provider, Qt::Horizontal, tr("����˰��"));
		setHeaderData(Bill_Customer, Qt::Horizontal, tr("����˰��"));
		setHeaderData(Bill_SecreArea, Qt::Horizontal, tr("������"));
		setHeaderData(Bill_Amount, Qt::Horizontal, tr("���"));
		setHeaderData(Bill_Tax, Qt::Horizontal, tr("˰��"));
		setHeaderData(Bill_Date, Qt::Horizontal, tr("��Ʊ����"));
		setHeaderData(Bill_Type, Qt::Horizontal, tr("��Ʊ����"));
		setHeaderData(Bill_RecordDate, Qt::Horizontal, tr("¼������"));
		setHeaderData(Bill_AuthDate, Qt::Horizontal, tr("��֤����"));
		setHeaderData(Bill_AuthCounts, Qt::Horizontal, tr("��֤����"));
		setHeaderData(Bill_AuthResult, Qt::Horizontal, tr("��֤���"));
}