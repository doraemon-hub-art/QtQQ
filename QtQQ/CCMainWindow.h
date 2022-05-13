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
	void setUserName(const QString& username);			//设置用户名
	void setLevelPixmap(int level);						//设置等级
	void setHeadPixmap(const QString& headPath);		//设置头像
	void setStatusMenuIcon(const QString& statusPath);	//设置状态
	//添加应用部件(app图片路径，app部件对象名)
	QWidget* addOtherAppExtension(const QString& apppath,const QString& appName);
	void initContactTree();
private slots:
	void onAppIconClicked();
	void onItemClicked(QTreeWidgetItem* item,int column);
	void onItemExpanded(QTreeWidgetItem* item);
	void onItemCollapsed(QTreeWidgetItem* item);
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);
private:
	void initTimer();//初始化计时器
	void initControl();
	void resizeEvent(QResizeEvent*event);
	bool eventFilter(QObject* obj,QEvent* event);
	void updateSearchStyle();//更新搜索样式
	void addCompanyDeps(QTreeWidgetItem* pRootGroupItem, int DepID);
	void mousePressEvent(QMouseEvent* event);

	QString getHeadPicturePath();
private:
    Ui::CCMainWindowClass ui;
	//QMap<QTreeWidgetItem*,QString>m_groupMap;//所有分组的分组项

	bool m_isAccountLogin;
	QString m_account;//登录的用户名或QQ号
};
