#include "SysconfPage.h"
#include "ParamWidget.h"

SysconfPage ::SysconfPage()
{
		buildUI();
}

SysconfPage ::~SysconfPage()
{

}

void SysconfPage :: buildUI()
{
		QTabWidget* tabWidget = new	QTabWidget;
		sysUserInfo = new SysUserInfo;
		tabWidget->addTab(sysUserInfo, tr("用户信息"));
		paramWidget = new ParamWidget;		
		tabWidget->addTab(paramWidget, tr("参数设置"));		
		
		QVBoxLayout* vmainlayout = new QVBoxLayout;
		vmainlayout->addWidget(tabWidget);
		QLabel* label = new QLabel(tr("为保证设置有效，请重新启动软件"));
		vmainlayout->addWidget(label);

		this->setLayout(vmainlayout);

		//connect(paramWidget, SIGNAL(process()), this, SLOT(processImage()));
}

ParamWidget* SysconfPage ::getParamPtr()
{
		return paramWidget;
}
