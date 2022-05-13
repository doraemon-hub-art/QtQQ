#pragma once

#include <QWidget>
#include "ui_TalkWindowShell.h"
#include "basicwindow.h"
#include <QMap>
#include "EmotionWindow.h"
#include "TalkWindowItem.h"
#include <QSqlQueryModel>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSqlQuery>
#include "WindowManager.h"


class TalkWindow;
class TalkWindowItem;
class QListWidgetItem;
class EmotionWindow;
  
const int gtcpPort = 8888;


//�ı�Ĭ�ϵĲ������
class CustomProxyStyle :public QProxyStyle
{
public:
	CustomProxyStyle(QObject* parent)
	{
		setParent(parent);
	}
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0)const
	{
		if (PE_FrameFocusRect == element)
		{
			//ȥ��windows�в���Ĭ�ϵı߿�����߿򣬲�����ȡ����ʱֱ�ӷ��أ������л���
			return;
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}

};


enum GroupType {
	COMPANY = 0,//��˾Ⱥ
	PERSONELGROUP,//���²�
	DEVELOPMENTGROUP,//�з���
	MARKETGROUP,//�г���
	PTOP,//˽��
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	//����µ����촰��
	void addTalkWindow(TalkWindow* talkWindow,TalkWindowItem* talkWindowItem, const QString& uid);
		
	//���õ�ǰ���촰��
	void setCurrentWidget(QWidget* widget); 

	const QMap<QListWidgetItem*, QWidget*>&getTalkWindowItemMap()const;

private:
	//��ʼ������
	void initControl();
	//��ʼ��tcpsocket
	void initTcpSocket();
	void initUdpSocket();//��ʼ��UDP
	QStringList& getEmployeesID(QStringList& employeesIDList);//��ȡ����Ա��QQ��
	bool createJSFile(QStringList &employeesList);
	void handleReceiveMsg(int senderEmployeeID,int msgType,QString strMsg);//���������Ϣ
	
public slots:
	//���鰴ť�����ִ�еĲۺ���
	void onEmotionBtnClicked(bool);
	//�ͻ��˷���Tcp����(���ݣ��������ͣ��ļ�)
	void updateSendTcpMsg(QString& strData,int &msgType,QString fileName = "");

private slots:
	//����б�����ִ�еĲۺ���
	void onTalkWindowItemClicked(QListWidgetItem* item );
	//���鱻ѡ��
	void onEmotionItemClicked(int emotionNum);

	void processPendingData();//����udp�㲥�յ�������
private:
	Ui::TalkWindowClass ui;
	//�����촰��
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;
	//���鴰��
	EmotionWindow* m_emotionWindow;

private:
	QTcpSocket* m_tcpClientSocket;//tcp�ͻ���
	QUdpSocket* m_udpReceiver;//udp���ն�
};
