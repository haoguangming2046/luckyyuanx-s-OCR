#include "SysUserInfo.h"
#include "Share/AppInfo.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>

SysUserInfo :: SysUserInfo()
{
		buildUI();
}

SysUserInfo::~SysUserInfo()
{

}

void SysUserInfo::on_btnModechoose_clicked()
{
		QString fileName = QFileDialog::getOpenFileName(this,
				tr("Load Masks"), AppInfo::instance()->appDir().c_str(),
				tr("XML files (*.xml)"));
		if(fileName.isEmpty())
		{
				QMessageBox::information(this, tr("提示"), tr("请选择可用的模板，以保证系统正常动作！"));
				return ;
		}
		leditBillModeDir->setText(fileName);
}

void SysUserInfo::on_btnSaved_clicked()
{
		QSqlQuery query;
		//update url
		query.prepare("update systemparams set value = ? where key = ?");
		query.addBindValue(leditUrl->text());
		query.addBindValue("url");
		query.exec();
		//update cxydm
		query.prepare("update systemparams set value = ? where key = ?");
		query.addBindValue(leditUserCode->text());
		query.addBindValue("czydm");
		query.exec();
		//update passwd
		query.prepare("update systemparams set value = ? where key = ?");
		query.addBindValue(leditPasswd->text());
		query.addBindValue("passwd");
		query.exec();
		//update action
		query.prepare("update systemparams set value = ? where key = ?");
		query.addBindValue(leditAction->text());
		query.addBindValue("action");
		query.exec();
		//update modedir
		query.prepare("update systemparams set value = ? where key = ?");
		query.addBindValue(leditBillModeDir->text());
		query.addBindValue("modedir");
		query.exec();
}

void SysUserInfo::buildUI()
{
		QSqlQuery query;
		//build controls
		//url
		query.exec("select * from systemparams where key = 'url'");
		query.next();
		QLabel* label1 = new QLabel(tr("Url"));
		leditUrl = new QLineEdit;
		leditUrl->setText(query.value(1).toString());
		//czydm
		query.exec("select * from systemparams where key = 'czydm'");
		query.next();
		QLabel* label3 = new QLabel(tr("用户代码(czydm)"));
		leditUserCode = new QLineEdit;
		leditUserCode->setText(query.value(1).toString());
		//passwd
		query.exec("select * from systemparams where key = 'passwd'");
		query.next();
		QLabel* label4 = new QLabel(tr("用户口令(passwd)"));
		leditPasswd = new QLineEdit;
		leditPasswd->setText(query.value(1).toString());
		//fpid
		query.exec("select * from systemparams where key = 'fpid'");
		query.next();
		QLabel* labelfpid = new QLabel(tr("发票id(fpid)"));
		leditfpid = new QLineEdit;
		leditfpid->setText(query.value(1).toString());
		btnSave = new QPushButton(tr("保存"));
		//action
		query.exec("select * from systemparams where key = 'action'");
		query.next();
		QLabel* label5 = new QLabel(tr("Action"));		
		leditAction = new QLineEdit;
		leditAction->setText(query.value(1).toString());
		//modedir
		query.exec("select * from systemparams where key = 'modedir'");
		query.next();
		QLabel* label6 = new QLabel(tr("发票模板"));
		btnModechoose = new QPushButton(tr("选择模板"));
		leditBillModeDir = new QLineEdit;
		leditBillModeDir->setText(query.value(1).toString());
		//layout
		QGridLayout* glayout = new QGridLayout;
		glayout->addWidget(label1, 0, 0);
		glayout->addWidget(leditUrl, 0, 1);
		glayout->addWidget(labelfpid, 1, 0);
		glayout->addWidget(leditfpid, 1, 1);
		glayout->addWidget(label3, 2, 0);
		glayout->addWidget(leditUserCode, 2, 1);
		glayout->addWidget(label4, 3, 0);
		glayout->addWidget(leditPasswd, 3, 1);
		glayout->addWidget(label5, 4, 0);
		glayout->addWidget(leditAction, 4, 1);
		
		glayout->addWidget(label6, 6, 0);
		glayout->addWidget(leditBillModeDir, 6, 1);
		glayout->addWidget(btnModechoose, 6, 2);
		glayout->addWidget(btnSave, 7, 0);
		
		QVBoxLayout* vlayout = new QVBoxLayout;
		vlayout->addLayout(glayout);
		vlayout->addStretch();
		
		this->setLayout(vlayout);
		//connect
		connect(btnSave, SIGNAL(clicked()), this, SLOT(on_btnSaved_clicked()));
		connect(btnModechoose, SIGNAL(clicked()), this, SLOT(on_btnModechoose_clicked()));

}