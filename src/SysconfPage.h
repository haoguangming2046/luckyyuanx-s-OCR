#ifndef SYSCONFPAGE_H_
#define  SYSCONFPAGE_H_

#include <QtGui>
#include "ParamWidget.h"
#include "SysUserInfo.h"
class ParamWidget;

class SysconfPage : public QWidget
{
		Q_OBJECT

public:
		SysconfPage();
		~SysconfPage();

		ParamWidget* getParamPtr();

private:
		void buildUI();
		ParamWidget* paramWidget;
		SysUserInfo* sysUserInfo;
};
#endif