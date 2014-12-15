#ifndef SYSUSERINFO_H_
#define SYSUSERINFO_H_

#include <QtGui/QWidget>

class QLabel;
class QPushButton;
class QLineEdit;

class SysUserInfo : public QWidget
{
		Q_OBJECT

public:
		SysUserInfo();
		~SysUserInfo();

		private slots:
				void on_btnSaved_clicked();
				void on_btnModechoose_clicked();

private:
		void buildUI();
		QLineEdit* leditUrl;
		QLineEdit* leditUserCode;
		QLineEdit* leditPasswd;
		QLineEdit* leditfpid;
		QLineEdit* leditAction;
		QLineEdit* leditBillModeDir;
		QPushButton* btnSave;
		QPushButton* btnModechoose;
};
#endif