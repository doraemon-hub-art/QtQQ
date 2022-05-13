 #pragma once

#include <QObject>


#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <QSqlQueryModel>
#include "TalkWindowShell.h"

class TalkWindowShell;//两个.h文件相互包含，需要声明一下。

class WindowManager : public QObject
{
	Q_OBJECT

public:
	WindowManager();
	~WindowManager();
	
public:

	QWidget* findWindowName(const QString& qsWindowName);
	void deleteWindowName(const QString& qsWindowName); 
	void addWindowName(const QString& qsWindownName,QWidget* qWidget);
	 
	static WindowManager* getInstance();
	TalkWindowShell* getTalkWindowShell();
	void addNewTalkWindow(const QString& uid);
	QString getCreateingTalkId();//获取正在创建的聊天窗口的id	


private:

	TalkWindowShell* m_talkwindowshell;
	QMap<QString, QWidget*>m_windowMap;//保存打开的窗口
	QString m_strCreatingTalkId = "";//正在创建的聊天窗口

};
