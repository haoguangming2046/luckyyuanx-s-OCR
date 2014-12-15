#ifndef QUERYPAGE_H_
#define QUERYPAGE_H_

#include <QtGui>
#include <QSqlQuery>

class QRadioButton;
class QPushButton;
class QLabel;
class QCheckBox;
class QDateEdit;
class QSqlQueryModel;
class QSqlQuery;

class DataBase;

class QueryPage : public QWidget
{
		Q_OBJECT

public:
		QueryPage();
		~QueryPage();

public slots:
		void buildTableModel();
		void on_chkZHI_clicked();
		void on_chkRecordDate_clicked();
		void on_chkAuthDate_clicked();
		void on_pbtnQuery_clicked();
		void on_pbtnDelete_clicked();

private:
		void buildUI();
		//QRadioButton
		QRadioButton* rbtnAllBill;
		QRadioButton* rbtnAuthed;
		QRadioButton* rbtnNotPass;
		QRadioButton* rbtnWaitAuth;
		QLabel* labelQueryCounts;
		//lineedit
		QLineEdit* leditBillCode;
		QLineEdit* leditBillNOStart;
		QCheckBox* chkZHI;
		QLineEdit* leditBillNOEnd;

		QCheckBox* chkRecordDate;
		QDateEdit* deditRecordDateStart;
		QDateEdit* deditRecordDateEnd;
		QCheckBox* chkAuthDate;
		QDateEdit* deditAuthDateStart;
		QDateEdit* deditAuthDateEnd;
		QLineEdit* leditProviderNO;
		//button
		QPushButton* pbtnQuery;
		QPushButton* pbtnHighQuery;
		QPushButton* pbtnEdit;
		QPushButton* pbtnPrintList;
		QPushButton* pbtnDelete;
		//tableView
		QTableView* tableView;
		QSqlQueryModel* model;
		QSqlQuery* query;
		QSqlQuery queryForUpdate;//用来更新model
		
};
#endif