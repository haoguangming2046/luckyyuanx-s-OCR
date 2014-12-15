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
		tabWidget->addTab(sysUserInfo, tr("�û���Ϣ"));
		paramWidget = new ParamWidget;		
		tabWidget->addTab(paramWidget, tr("��������"));		
		
		QVBoxLayout* vmainlayout = new QVBoxLayout;
		vmainlayout->addWidget(tabWidget);
		QLabel* label = new QLabel(tr("Ϊ��֤������Ч���������������"));
		vmainlayout->addWidget(label);

		this->setLayout(vmainlayout);

		//connect(paramWidget, SIGNAL(process()), this, SLOT(processImage()));
}

ParamWidget* SysconfPage ::getParamPtr()
{
		return paramWidget;
}
