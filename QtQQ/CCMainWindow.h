#pragma once

#include "basicwindow.h"
#include "ui_CCMainWindow.h"
#include <QPainter>
#include "SkinWindow.h"
#include <QTimer>
#include "SysTray.h"
#include <QHBoxLayout>
#include <QEvent>
#include "NotifyManager.h"
#include "WindowManager.h"
#include "TalkWindowShell.h"
#include <QApplication>
#include <QSqlQuery>


#include "ContactItem.h"
#include <QTreeWidgetItem>
class CCMainWindow : public BasicWindow
{
    Q_OBJECT

public:
	CCMainWindow(QString account,bool isAccountLogin, QWidget *parent = Q_NULLPTR);
	~CCMainWindow();
public:					
	void setUserName(const QString& username);			//�����û���
	void setLevelPixmap(int level);						//���õȼ�
	void setHeadPixmap(const QString& headPath);		//����ͷ��
	void setStatusMenuIcon(const QString& statusPath);	//����״̬
	//���Ӧ�ò���(appͼƬ·����app����������)
	QWidget* addOtherAppExtension(const QString& apppath,const QString& appName);
	void initContactTree();
private slots:
	void onAppIconClicked();
	void onItemClicked(QTreeWidgetItem* item,int column);
	void onItemExpanded(QTreeWidgetItem* item);
	void onItemCollapsed(QTreeWidgetItem* item);
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);
private:
	void initTimer();//��ʼ����ʱ��
	void initControl();
	void resizeEvent(QResizeEvent*event);
	bool eventFilter(QObject* obj,QEvent* event);
	void updateSearchStyle();//����������ʽ
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);
	void mousePressEvent(QMouseEvent* event);

	QString getHeadPicturePath();
private:
    Ui::CCMainWindowClass ui;
	//QMap<QTreeWidgetItem*,QString>m_groupMap;//���з���ķ�����

	bool m_isAccountLogin;
	QString m_account;//��¼���û�����QQ��
};
