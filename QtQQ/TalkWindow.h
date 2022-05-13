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
	void initTalkWindow();//��ʼ��Ⱥ��
	int getComDepID();
	 //��ʼ����������
	//void initCompanyTalk();
	//void initPersonelTalk();
	//void initMarketTalk();
	//void initDevelopTalk();
	void initPtoPTalk();//��ʼ������
	void addPeopInfo(QTreeWidgetItem* pRootGroupItem,int employeeID);

private:
	Ui::TalkWindow ui;
	QString m_talkId;
	//GroupType m_groupType;
	QMap<QTreeWidgetItem*, QString>m_groupPeopleMap;//���з�����ϵ������
	bool m_isGroupTalk;//�Ƿ�Ⱥ��	

	friend class TalkWindowShell;
};
