 #pragma once

#include <QObject>


#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include <QSqlQueryModel>
#include "TalkWindowShell.h"

class TalkWindowShell;//����.h�ļ��໥��������Ҫ����һ�¡�

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
	QString getCreateingTalkId();//��ȡ���ڴ��������촰�ڵ�id	


private:

	TalkWindowShell* m_talkwindowshell;
	QMap<QString, QWidget*>m_windowMap;//����򿪵Ĵ���
	QString m_strCreatingTalkId = "";//���ڴ��������촰��

};
