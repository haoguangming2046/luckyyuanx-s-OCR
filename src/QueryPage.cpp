#include "QueryPage.h"
#include "Globel.h"
#include "DataBase.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQueryModel>
//#include <QSqlQuery>
#include <QSqlRecord>

QueryPage :: QueryPage()
{		
		buildUI();		
		buildTableModel();
}

QueryPage :: ~QueryPage()
{

}

void QueryPage :: buildUI()
{
		//build RadioButton
		rbtnAllBill = new QRadioButton(tr("所有的发票数据"));
		rbtnAllBill->setChecked(true);
		rbtnAuthed = new QRadioButton(tr("通过认证的发票"));
		rbtnNotPass = new QRadioButton(tr("未通过认证发票"));
		rbtnWaitAuth = new QRadioButton(tr("等待认证的发票"));
		QLabel* label1 = new QLabel(tr("共有"));
		labelQueryCounts = new QLabel(tr("0"));
		QLabel* label2 = new QLabel(tr("条发票记录"));
		//left layout
		QVBoxLayout* vlayoutleft = new QVBoxLayout;
		QVBoxLayout* vlayoutltemp1 = new QVBoxLayout;
		vlayoutltemp1->addWidget(rbtnAllBill);
		vlayoutltemp1->addWidget(rbtnAuthed);
		vlayoutltemp1->addWidget(rbtnNotPass);
		vlayoutltemp1->addWidget(rbtnWaitAuth);		

		QHBoxLayout* hlayoutltemp2 = new QHBoxLayout;
		hlayoutltemp2->addWidget(label1);
		hlayoutltemp2->addWidget(labelQueryCounts);
		hlayoutltemp2->addWidget(label2);

		vlayoutleft->addLayout(vlayoutltemp1);
		vlayoutleft->addLayout(hlayoutltemp2);

		//build Edit
		QLabel* label3 = new QLabel(tr("发票代码"));
		leditBillCode = new QLineEdit;
		QLabel* label4 = new QLabel(tr("发票号码"));
		leditBillNOStart = new QLineEdit;
		chkZHI = new QCheckBox(tr("至"));
		leditBillNOEnd = new QLineEdit;
		leditBillNOEnd->setEnabled(false);
		//hlayoutmtemp1
		QHBoxLayout* hlayoutmtemp1 = new QHBoxLayout;
		hlayoutmtemp1->addWidget(label3);
		hlayoutmtemp1->addWidget(leditBillCode);
		hlayoutmtemp1->addWidget(label4);
		hlayoutmtemp1->addWidget(leditBillNOStart);
		hlayoutmtemp1->addWidget(chkZHI);
		hlayoutmtemp1->addWidget(leditBillNOEnd);
		hlayoutmtemp1->addStretch();
		//build RecordDate
		chkRecordDate = new QCheckBox(tr("发票录入时间"));
		deditRecordDateStart = new QDateEdit;
		deditRecordDateStart->setEnabled(false);
		deditRecordDateStart->setDate(QDate::currentDate());
		QLabel* label5 = new QLabel(tr("-->>"));
		deditRecordDateEnd = new QDateEdit;
		deditRecordDateEnd->setDate(QDate::currentDate());
		deditRecordDateEnd->setEnabled(false);
		//hlayoutmtemp2
		QHBoxLayout* hlayoutmtemp2 = new QHBoxLayout;
		hlayoutmtemp2->addWidget(chkRecordDate);
		hlayoutmtemp2->addWidget(deditRecordDateStart);
		hlayoutmtemp2->addWidget(label5);
		hlayoutmtemp2->addWidget(deditRecordDateEnd);
		hlayoutmtemp2->addStretch();
		//build AuthDate
		chkAuthDate = new QCheckBox(tr("发票认证时间"));
		deditAuthDateStart = new QDateEdit;
		deditAuthDateStart->setEnabled(false);
		deditAuthDateStart->setDate(QDate::currentDate());
		QLabel* label6 = new QLabel(tr("-->>"));
		deditAuthDateEnd = new QDateEdit;
		deditAuthDateEnd->setEnabled(false);
		deditAuthDateEnd->setDate(QDate::currentDate());
		//hlayoutmtemp3
		QHBoxLayout* hlayoutmtemp3 = new QHBoxLayout;
		hlayoutmtemp3->addWidget(chkAuthDate);
		hlayoutmtemp3->addWidget(deditAuthDateStart);
		hlayoutmtemp3->addWidget(label6);
		hlayoutmtemp3->addWidget(deditAuthDateEnd);
		hlayoutmtemp3->addStretch();
		//build ProviderNO
		QLabel* label7 = new QLabel(tr("销方纳税人识别号"));
		leditProviderNO = new QLineEdit;
		//hlayoutmtemp4
		QHBoxLayout* hlayoutmtemp4 = new QHBoxLayout;
		hlayoutmtemp4->addWidget(label7);
		hlayoutmtemp4->addWidget(leditProviderNO);
		hlayoutmtemp4->addStretch();
		//build vlayoutmiddle
		QVBoxLayout* vlayoutmiddle = new QVBoxLayout;
		vlayoutmiddle->addLayout(hlayoutmtemp1);
		vlayoutmiddle->addLayout(hlayoutmtemp2);
		vlayoutmiddle->addLayout(hlayoutmtemp3);
		vlayoutmiddle->addLayout(hlayoutmtemp4);

		//build button
		pbtnQuery = new QPushButton(tr("检索记录"));
		pbtnHighQuery = new QPushButton(tr("高级检索"));
		pbtnEdit = new QPushButton(tr("查看/修改"));
		pbtnPrintList = new QPushButton(tr("打印清单"));
		pbtnDelete = new QPushButton(tr("删除记录"));
		//build vlayoutright
		QVBoxLayout* vlayoutright = new QVBoxLayout;
		vlayoutright->addWidget(pbtnQuery);
		vlayoutright->addWidget(pbtnHighQuery);
		vlayoutright->addWidget(pbtnEdit);
		vlayoutright->addWidget(pbtnPrintList);
		vlayoutright->addWidget(pbtnDelete);
		//build layoutupper
		QHBoxLayout* hlayoutupper = new QHBoxLayout;
		hlayoutupper->addLayout(vlayoutleft);
		hlayoutupper->addLayout(vlayoutmiddle);
		hlayoutupper->addLayout(vlayoutright);
		//build tabelView
		tableView = new QTableView;
		model = new QSqlQueryModel;
		query = new QSqlQuery;
		//build mainlayout
		QVBoxLayout* mainlayout = new QVBoxLayout;
		mainlayout->addLayout(hlayoutupper);
		mainlayout->addWidget(tableView);
		//mainlayout->addStretch();
		this->setLayout(mainlayout);

		//connect
		connect(pbtnQuery, SIGNAL(clicked()), this, SLOT(on_pbtnQuery_clicked()));
		connect(pbtnDelete, SIGNAL(clicked()), this, SLOT(on_pbtnDelete_clicked()));
		connect(chkZHI, SIGNAL(toggled(bool)), this, SLOT(on_chkZHI_clicked()));
		connect(chkRecordDate, SIGNAL(toggled(bool)), this, SLOT(on_chkRecordDate_clicked()));
		connect(chkAuthDate, SIGNAL(toggled(bool)), this, SLOT(on_chkAuthDate_clicked()));
		//QCompletor and QStringList
		QStringList BillCodeList, BillNOList, BillProviderList;
		QSqlQuery* queryForStringList = new QSqlQuery;
		bool ok = queryForStringList->exec("select * from billtable");
		while(queryForStringList->next())
		{ 
				BillCodeList<<queryForStringList->value(0).toString();
				BillNOList<<queryForStringList->value(1).toString();
				BillProviderList<<queryForStringList->value(2).toString();
		}
		QCompleter* completerBillCode = new QCompleter(BillCodeList); 
		QCompleter* completerBillNOStart = new QCompleter(BillNOList);
		QCompleter* completerBillNOEnd = new QCompleter(BillNOList);
		QCompleter* completerBillProvider = new QCompleter(BillProviderList);

		completerBillCode->setCompletionMode(QCompleter::InlineCompletion);//完整的匹配Inline
		completerBillNOStart->setCompletionMode(QCompleter::InlineCompletion);
		completerBillNOEnd->setCompletionMode(QCompleter::InlineCompletion);
		completerBillProvider->setCompletionMode(QCompleter::InlineCompletion);
		//completer->setCompletionMode(QCompleter::PopupCompletion); //匹配以弹出式显示
		leditBillCode->setCompleter(completerBillCode);
		leditBillNOStart->setCompleter(completerBillNOStart);
		leditBillNOEnd->setCompleter(completerBillNOEnd);
		leditProviderNO->setCompleter(completerBillProvider);


		pbtnHighQuery->setEnabled(false);
		pbtnEdit->setEnabled(false);
		pbtnHighQuery->setEnabled(false);
		pbtnPrintList->setEnabled(false);
}

void QueryPage ::buildTableModel()
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

		tableView->setModel(model);
		tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		//tableView->resizeColumnToContents(4);
		tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tableView->horizontalHeader()->setVisible(true);
		tableView->verticalHeader()->setVisible(true);
		//tableView->resizeColumnsToContents();
		//tableView->resizeRowsToContents();
		
		QHeaderView* header = tableView->horizontalHeader();
		header->setStretchLastSection(true);
		header->setVisible(true);
		QHeaderView* vheader = tableView->verticalHeader();
		vheader->setVisible(true);
}

// build slots
void QueryPage ::on_chkZHI_clicked()
{
		static bool choosed = true;
		if(choosed)
				leditBillNOEnd->setEnabled(true);
		else
				leditBillNOEnd->setEnabled(false);
		choosed = !choosed;
}

void QueryPage :: on_chkRecordDate_clicked()
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

void QueryPage :: on_chkAuthDate_clicked()
{
		static bool choosed = true;
		if(choosed)
		{
				deditAuthDateStart->setEnabled(true);
				deditAuthDateEnd->setEnabled(true);
		}
		else
		{
				deditAuthDateStart->setEnabled(false);
				deditAuthDateEnd->setEnabled(false);
		}
		choosed = !choosed;
}

void QueryPage ::on_pbtnQuery_clicked()
{
		query = new QSqlQuery;
		query->exec("select * from billtable");
		queryForUpdate = * query;
		model->setQuery(*query);
		buildTableModel();

} 

void QueryPage::on_pbtnDelete_clicked()
{
		QItemSelectionModel* selectionModel = tableView->selectionModel();
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

		query = new QSqlQuery;
		while(rowMapIterator.hasNext())
		{
				rowMapIterator.next();
				int nkey = rowMapIterator.key();//debug
				record = model->record(rowMapIterator.key());
				query->prepare("delete from billtable where Bill_Code = ? and Bill_NO = ?");
				query->addBindValue(record.value(Bill_Code).toString());
				query->addBindValue(record.value(Bill_NO).toString());
				query->exec();
		}
		queryForUpdate.exec();
		model->setQuery(queryForUpdate);
}

