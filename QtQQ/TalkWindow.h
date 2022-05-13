#pragma once

#include <QWidget>
#include "ui_TalkWindow.h"
#include "TalkWindowShell.h"
#include "RootContactItem.h"
#include <QWidgetItem>
#include "CommonUtils.h"
#include "ContactItem.h"
#include <QToolTip>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QSqlQuery>


class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(QWidget *parent,const QString& uid);
	~TalkWindow();

public:
	void addEmotionImage(int emotionNum);
	void setWindowName(const QString& name);
	QString getTalkId();

private slots:
	void onSendBtnClicked(bool);
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);


private:
	void initControl();
	void initGroupTalkStatus();
	void initTalkWindow();//初始化群聊
	int getComDepID();
	 //初始化各种聊天
	//void initCompanyTalk();
	//void initPersonelTalk();
	//void initMarketTalk();
	//void initDevelopTalk();
	void initPtoPTalk();//初始化单聊
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem,int employeeID);

private:
	Ui::TalkWindow ui;
	QString m_talkId;
	//GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;//所有分组联系人姓名
	bool m_isGroupTalk;//是否群聊	

	friend class TalkWindowShell;
};
