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


//改变默认的部件风格
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
			//去掉windows中部件默认的边框或虚线框，部件获取焦点时直接返回，不进行绘制
			return;
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}

};


enum GroupType {
	COMPANY = 0,//公司群
	PERSONELGROUP,//人事部
	DEVELOPMENTGROUP,//研发部
	MARKETGROUP,//市场部
	PTOP,//私聊
};

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = Q_NULLPTR);
	~TalkWindowShell();

public:
	//添加新的聊天窗口
	void addTalkWindow(TalkWindow* talkWindow,TalkWindowItem* talkWindowItem, const QString& uid);
		
	//设置当前聊天窗口
	void setCurrentWidget(QWidget* widget); 

	const QMap<QListWidgetItem*, QWidget*>&getTalkWindowItemMap()const;

private:
	//初始化部件
	void initControl();
	//初始化tcpsocket
	void initTcpSocket();
	void initUdpSocket();//初始化UDP
	QStringList& getEmployeesID(QStringList& employeesIDList);//获取所有员工QQ号
	bool createJSFile(QStringList &employeesList);
	void handleReceiveMsg(int senderEmployeeID,int msgType,QString strMsg);//处理接收信息
	
public slots:
	//表情按钮点击后执行的槽函数
	void onEmotionBtnClicked(bool);
	//客户端发送Tcp数据(数据，数据类型，文件)
	void updateSendTcpMsg(QString& strData,int &msgType,QString fileName = "");

private slots:
	//左侧列表点击后执行的槽函数
	void onTalkWindowItemClicked(QListWidgetItem* item );
	//表情被选中
	void onEmotionItemClicked(int emotionNum);

	void processPendingData();//处理udp广播收到的数据
private:
	Ui::TalkWindowClass ui;
	//打开聊天窗口
	QMap<QListWidgetItem*, QWidget*>m_talkWindowItemMap;
	//表情窗口
	EmotionWindow* m_emotionWindow;

private:
	QTcpSocket* m_tcpClientSocket;//tcp客户端
	QUdpSocket* m_udpReceiver;//udp接收端
};
