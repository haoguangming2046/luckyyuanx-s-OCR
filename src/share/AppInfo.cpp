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
 * 返回绝对路径
 */
std::string AppInfo::appDir()
{
	QFileInfo fileInfo(QString::fromLocal8Bit(mAppPath.c_str()));
	return fileInfo.dir().absolutePath().toLocal8Bit().constData();
}

/**
 * 将本地字符串转换成UTF8格式
 */
std::string AppInfo::toUTF8(std::string localString )
{
	QString locatText = QString::fromLocal8Bit(localString.c_str());
	std::string unicodeString = locatText.toAscii().constData();
	return unicodeString;
}

/**
 * 将UTF8格式字符串转换成本地格式
 */
std::string AppInfo::fromUTF8( std::string unicodeString )
{
	QString unicodeText = QString::fromStdString(unicodeString);
	std::string localString = unicodeText.toLocal8Bit().constData();
	return localString;
}
