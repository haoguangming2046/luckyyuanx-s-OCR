#include "AppInfo.h"
#include <QtCore/QtCore>

AppInfo* AppInfo::mInstance = 0;

AppInfo* AppInfo::instance()
{
	if(mInstance == 0)
	{
		mInstance = new AppInfo;
	}
	return mInstance;
}

AppInfo::AppInfo()
{

}

void AppInfo::setAppPath(const std::string& appPath)
{
	mAppPath = appPath;
}

const std::string& AppInfo::appPath()
{
	return mAppPath;
}

/**
 * ���ؾ���·��
 */
std::string AppInfo::appDir()
{
	QFileInfo fileInfo(QString::fromLocal8Bit(mAppPath.c_str()));
	return fileInfo.dir().absolutePath().toLocal8Bit().constData();
}

/**
 * �������ַ���ת����UTF8��ʽ
 */
std::string AppInfo::toUTF8(std::string localString )
{
	QString locatText = QString::fromLocal8Bit(localString.c_str());
	std::string unicodeString = locatText.toAscii().constData();
	return unicodeString;
}

/**
 * ��UTF8��ʽ�ַ���ת���ɱ��ظ�ʽ
 */
std::string AppInfo::fromUTF8( std::string unicodeString )
{
	QString unicodeText = QString::fromStdString(unicodeString);
	std::string localString = unicodeText.toLocal8Bit().constData();
	return localString;
}
