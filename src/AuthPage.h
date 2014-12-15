#ifndef AUTHPAGE_H_
#define AUTHPAGE_H_

#include <QtGui>
class QSqlQuery;
#include <QSqlQueryModel>
#include <QVector>

class QCompleter;
class MySqlQueryModel;
//处理网络
class QNetworkAccessManager;
class QNetworkReply;
class QSslConfiguration;
class QNetworkRequest;
class QSqlTableModel;

class AuthPage : public QWidget
{
		Q_OBJECT

public:
		AuthPage();
		~AuthPage();	

private slots:
		void on_btnQuery_clicked();
		void on_btnStartAuth_clicked();
		void on_chkBillRecordDate_clicked();
		void on_chkZhi_clicked();
		void on_chkSelectAll_clicked();

		void finishedSlot(QNetworkReply *reply);

private:
		void buildUI();
		void buildModel(QSqlQueryModel* model, QTableView* view);
		QString createPostData(const QSqlRecord& record);//构建发送的post数据报文
		QVector<QString> stringSplit(QString& strSecret, char *c);//拆分密码区
		bool updateAuthResult(const QString& strResult, const QString & strType);

		//QItemSelectionModel* selectionModel;
		//QMap<int, int> rowMap;
		//UI controls
		QLineEdit* leditBillCode;
		QLineEdit* leditBillNOStart;
		QLineEdit* leditBillNOEnd;
		QCheckBox* chkZhi;

		QRadioButton* rbtnNotPass;
		QRadioButton* rbtnWaitAuth;
		QRadioButton* rbtnAll;
		QCheckBox* chkBillRecordDate;
		QDateEdit* deditRecordDateStart;
		QDateEdit* deditRecordDateEnd;

		QPushButton* btnQuery;
		QPushButton* btnStartAuth;
		QCheckBox* chkSelectAll;
		QCheckBox* chkAuthAsRows;

		QTableView* QueryView;
		QTableView* authResultView;
			
		//data variants about query
		MySqlQueryModel* queryModel;
		QSqlQueryModel* authResultModel;
		QSqlQuery* query;
		QString strurl, strfpid, strczydm, strpasswd, straction, strnsrsbh;//保存认证需要的几个数据段值
		
		QString strBillCode, strBillType;
		QString nBillNOStart;
		QString nBillNOEnd;
		QDate dRecordDateStart;
		QDate dRecordDateEnd;
		//variants about network
		QNetworkAccessManager* networkAccessManager;
		QSslConfiguration* sslConfig;
		QNetworkRequest* networkRequest;
		QNetworkReply* networkReply;
public:
	    static QSqlQuery staticQuery;//静态成员，用来保存查询得到的Query
		static QSqlRecord staticRecord;//静态成员，用来保存当前的认证项，，，，，，，，，方法存疑？
};

class MySqlQueryModel : public QSqlQueryModel
{
public:
		MySqlQueryModel();
		//重载三个虚函数
		Qt::ItemFlags flags(const QModelIndex &index)  const;
		bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */);
		QVariant data(const QModelIndex &item, int role = Qt::DisplayRole );
		
private:
		bool setBillCode(const QString &strBillCode, const QString & strBillNO, const QString & strValue);
		bool setBillNO(const QString &strBillCode, const QString & strBillNO, const QString & strValue);
		bool setProvider(const QString &strBillCode, const QString & strBillNO, const QString & strValue);
		bool setCustomer(const QString &strBillCode, const QString & strBillNO, const QString & strValue);
		bool setSecretArea(const QString &strBillCode, const QString & strBillNO, const QString & strValue);
		bool setAmount(const QString &strBillCode, const QString & strBillNO, float fValue);
		bool setBillTax(const QString &strBillCode, const QString & strBillNO,  float fValue);
		bool setBillDate(const QString &strBillCode, const QString & strBillNO, const QDate & dValue);
public:
		void refresh();
		//QSqlQuery query;
};

#endif