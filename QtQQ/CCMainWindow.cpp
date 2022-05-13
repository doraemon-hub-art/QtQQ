#include "CCMainWindow.h"
#include <QProxyStyle>
#include "RootContactItem.h"


QString gstrLoginHeadPath;
extern QString gLoginEmployeeID;

class CCMainWindowCustomProxyStyle :public QProxyStyle
{
public:
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget * widget = nullptr)const
	{
		if (element == PE_FrameFocusRect)
		{
			return;
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);
		}
	}
};	

CCMainWindow::CCMainWindow(QString account, bool isAccountLogin,QWidget *parent)
    : BasicWindow(parent),m_isAccountLogin(isAccountLogin),m_account(account)
{
    ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool);
	loadStyleSheet("CCMainWindow");

	setHeadPixmap(getHeadPicturePath());

	initControl();
	initTimer();
}

CCMainWindow::~CCMainWindow()
{

}

void CCMainWindow::initTimer()
{
	QTimer* timer = new QTimer(this);
	timer->setInterval(500);
	connect(timer, &QTimer::timeout, [this] {
		static int level = 0;
		if(level == 99){
			level = 0;
		}
		level++;
		setLevelPixmap(level);
	});
	timer->start();
}

void CCMainWindow::initControl()
{
	//树部件获取焦点时不绘制边框
	ui.treeWidget->setStyle(new CCMainWindowCustomProxyStyle);
	setLevelPixmap(0);
	//setHeadPixmap(":/Resources/MainWindow/girl.png");
	setStatusMenuIcon(":/Resources/MainWindow/StatusSucceeded.png");

	QHBoxLayout* appupLayout = new QHBoxLayout;//设置水平布局
	appupLayout->setContentsMargins(0, 0, 0, 0);
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_7.png","app_7"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_2.png","app_2"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_3.png","app_3"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_4.png","app_4"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_5.png","app_5"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_6.png","app_6"));
	appupLayout->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/skin.png","app_skin"));
	appupLayout->addStretch();
	appupLayout->setSpacing(2);
	ui.appWidget->setLayout(appupLayout);

	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_10.png","app_10"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_8.png","app_8"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_11.png","app_11"));
	ui.bottomLayout_up->addWidget(addOtherAppExtension(":/Resources/MainWindow/app/app_9.png","app_9"));
	ui.bottomLayout_up->addStretch();



	initContactTree();

	//个性签名安装事件过滤器-当前窗体进行监视
	ui.lineEdit->installEventFilter(this);
	//搜索框-当前窗体进行监视
	ui.searchLineEdit->installEventFilter(this);


	connect(ui.sysmin, SIGNAL(clicked(bool)), this, SLOT(onShowHide(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), this, SLOT(onShowClose(bool)));
	
	connect(NotifyManager::getInstance(), &NotifyManager::signalSkinChanged, [this]() {
		updateSearchStyle();
	});

	//创建一个系统托盘
	SysTray* systray = new SysTray(this);
}

void CCMainWindow::setUserName(const QString & username)
{
	ui.nameLabel->adjustSize();
	//文本过长时进行省略
	//fontMetrics()返回QFontMerics类对象
	//文本-模式-长度
	QString name = ui.nameLabel->fontMetrics().elidedText(username, Qt::ElideRight, ui.nameLabel->width());
	ui.nameLabel->setText(name);

}

void CCMainWindow::setLevelPixmap(int level)
{
	QPixmap levelPixmap(ui.levelBtn->size());
	levelPixmap.fill(Qt::transparent);

	QPainter painter(&levelPixmap);
	painter.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));
	
	int unitNum = level % 10;//个位数
	int tenNum = level / 10;//十位数
	//十位,截取图片中的部分进行绘制
	//drawPixmap(绘制点x,绘制点y,图片，图片左上角x，图片左上角y，拷贝的宽度，拷贝的宽度)
	painter.drawPixmap(10, 4, QPixmap(":/Resources/MainWindow/levelvalue.png"),tenNum * 6,0,6,7);
	//个位
	painter.drawPixmap(16, 5, QPixmap(":/Resources/MainWindow/levelvalue.png"),unitNum * 6,0,6,7);

	ui.levelBtn->setIcon(levelPixmap);
	ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::setHeadPixmap(const QString& headPath)
{
	QPixmap pix;
	pix.load(":/Resources/MainWindow/head_mask.png");
	ui.headLabel->setPixmap(getRoundImage(QPixmap(headPath), pix, ui.headLabel->size()));
	
}

void CCMainWindow::setStatusMenuIcon(const QString& statusPath)
{
	//先画到pixmap上，然后再贴到btn上。
	QPixmap statusBtnPixmap(ui.stausBtn->size());
	statusBtnPixmap.fill(Qt::transparent);

	QPainter painter(&statusBtnPixmap);
	painter.drawPixmap(4, 4, QPixmap(statusPath));
	
	ui.stausBtn->setIcon(statusBtnPixmap);
	ui.stausBtn->setIconSize(ui.stausBtn->size());

}

QWidget* CCMainWindow::addOtherAppExtension(const QString& apppath, const QString& appName)
{
	QPushButton* btn = new QPushButton(this);
	btn->setFixedSize(20, 20);

	QPixmap pixmap(btn->size());
	pixmap.fill(Qt::transparent);

	//先画到一个pixmap上，在画到btn上
	QPainter painter(&pixmap);
	QPixmap appPixmap(apppath);
	painter.drawPixmap((btn->width() - appPixmap.width())/2,
		(btn->height()-appPixmap.height())/2,
		appPixmap);
	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
	btn->setObjectName(appName); //设置对象名
	btn->setProperty("hasborder",true);

	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onAppIconClicked);
	return btn;
}

void CCMainWindow::initContactTree()
{
	//展开与收缩时的信号
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
	connect(ui.treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*, int)));

	

	//根结点
	QTreeWidgetItem* pRootGroupItem = new QTreeWidgetItem;
	pRootGroupItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	pRootGroupItem->setData(0,Qt::UserRole,0);//根项数据设为0

	RootContactItem* pItemName = new RootContactItem(true, ui.treeWidget);

	//获取公司部门ID(公司群号)
	QSqlQuery queryComDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
		.arg(QString::fromLocal8Bit("公司群")));
	queryComDepID.exec();
	queryComDepID.first();//指向结果集的第一条
	int CompDepID = queryComDepID.value(0).toInt();

	//获取QQ登陆者所在的部门ID(部门群号)
	QSqlQuery querySelfDepID(QString("SELECT departmentID FROM tab_employees WHERE employeeID = %1")
		.arg(gLoginEmployeeID));
	querySelfDepID.exec();
	querySelfDepID.first();//指向结果集的第一条
	int SelfDepID = querySelfDepID.value(0).toInt();

	//QStringList sCompDeps;//公司部门
	//sCompDeps << QString::fromLocal8Bit("公司群");
	//sCompDeps << QString::fromLocal8Bit("人事部");
	//sCompDeps << QString::fromLocal8Bit("研发部");
	//sCompDeps << QString::fromLocal8Bit("市场部");


	//for (int nIndex = 0; nIndex < sCompDeps.length(); nIndex++)
	//{
	//	addCompanyDeps(pRootGroupItem, sCompDeps.at(nIndex));
	//}

	//链接到数据库之后只需要添加两次
	//一个大家都有的公司群
	//还有一个你的特定部门
	//说是QQ，其实可以说是一个公司里面的一个小的通讯系统。
	//初始化公司群及登录者所在的群
	addCompanyDeps(pRootGroupItem,CompDepID);
	addCompanyDeps(pRootGroupItem,SelfDepID);

	QString strGroupName = QString::fromLocal8Bit("群组");
	pItemName->setText(strGroupName);


	//插入分组结点
	ui.treeWidget->addTopLevelItem(pRootGroupItem);//添加顶级项
	ui.treeWidget->setItemWidget(pRootGroupItem, 0, pItemName);

}

void CCMainWindow::resizeEvent(QResizeEvent*event)
{
	setUserName(QString::fromLocal8Bit("快乐的威猛先生"));
	BasicWindow::resizeEvent(event);
}

bool CCMainWindow::eventFilter(QObject* obj, QEvent* event)
{
	//判断当前被监视的对象是不是搜索框
	if (ui.searchLineEdit == obj)
	{
		//键盘焦点事件
		if (event->type() == QEvent::FocusIn)
		{
			ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgb(255,255,255);border-bottom:1px solid rgba(%1,%2,%3,100)} \
																				QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/main_search_deldown.png)} \
																				QPushButton#searchBtn:hover{border-image:url(:/Resources/MainWindow/search/main_search_delhighlight.png)} \
																				QPushButton#searchBtn:pressed{border-image:url(:/Resources/MainWindow/search/main_search_delhighdown.png)}")
				.arg(m_colorBackGround.red())
				.arg(m_colorBackGround.green())
				.arg(m_colorBackGround.blue()));
			return true;
		}
		else if (event->type() == QEvent::FocusOut)
		{
			updateSearchStyle();
		}
	}
return false;
}

void CCMainWindow::updateSearchStyle()
{
	// 进行还原
	ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{background-color:rgba(%1,%2,%3,50);border-bottom:1px solid rgba(%1,%2,%3,30)}\
																		QPushButton#searchBtn{border-image:url(:/Resources/MainWindow/search/search_icon.png)}")
		.arg(m_colorBackGround.red())
		.arg(m_colorBackGround.green())
		.arg(m_colorBackGround.blue()));
}

void CCMainWindow::addCompanyDeps(QTreeWidgetItem* pRootGroupItem, 
	int DepID)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem;

	QPixmap pix; 
	pix.load(":/Resources/MainWindow/head_mask.png");

	//添加子结点
	pChild->setData(0,Qt::UserRole,1);//子项数据设为  1
	pChild->setData(0,Qt::UserRole + 1,DepID);


	//获取公司部门头像
	QPixmap groupPix;
	QSqlQuery queryPicture(QString("SELECT picture FROM tab_department WHERE departmentID = %1")
		.arg(DepID));
	queryPicture.exec();
	queryPicture.next();
	groupPix.load(queryPicture.value(0).toString());
	//获取部门名称
	QString strDepName;
	QSqlQuery querDepName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1")
	.arg(DepID));
	querDepName.exec();
	querDepName.first();
	strDepName = querDepName.value(0).toString();



	ContactItem* pContactItem = new ContactItem(ui.treeWidget);
	pContactItem->setHeadPixmap(getRoundImage(groupPix,pix,pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strDepName);

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild, 0, pContactItem);

	//m_groupMap.insert(pChild,sDeps);
}

void CCMainWindow::mousePressEvent(QMouseEvent* event)
{
	if (qApp->widgetAt(event->pos()) != ui.searchLineEdit && ui.searchLineEdit->hasFocus()){
		ui.searchLineEdit->clearFocus();//清除焦点
	}
	else if (qApp->widgetAt(event->pos()) != ui.lineEdit && ui.lineEdit->hasFocus())
	{
		ui.lineEdit->clearFocus();
	}
	BasicWindow::mousePressEvent(event);//其它事件处理

}

QString CCMainWindow::getHeadPicturePath() {
	QString strPicturePath;
	
	//QQ号登录
	if (!m_isAccountLogin) {
		QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees \
		WHERE employeeID = '%1'").arg(gLoginEmployeeID));
		queryPicture.exec();
		queryPicture.next();

		strPicturePath = queryPicture.value(0).toString();
	} else {
		//用户名登录

		QSqlQuery querEmployeeID(QString("SELECT employeeID FROM tab_accounts \
		WHERE account = '%1'").arg(m_account));
		querEmployeeID.exec();
		querEmployeeID.next();

		int employeeID = querEmployeeID.value(0).toInt();

		QSqlQuery queryPicture(QString("SELECT picture FROM tab_employees \
		WHERE employeeID = %1").arg(employeeID));
		queryPicture.exec();
		queryPicture.next();
		strPicturePath = queryPicture.value(0).toString();



	}
	gstrLoginHeadPath = strPicturePath;
	return strPicturePath;
}

void CCMainWindow::onAppIconClicked() 
{
	//判断信号发送者是否是app_skin
	if (sender()->objectName() == "app_skin") 
	{ 
		SkinWindow* skinWindow = new SkinWindow;
		skinWindow->show();
	}
}

void CCMainWindow::onItemClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)//如果不是子项
	{
		item->setExpanded(!item->isExpanded());//未展开则展开子项

	}
}

void CCMainWindow::onItemExpanded(QTreeWidgetItem* item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		//dynamic_cast 将基类对象指针(或引用)转换到继承类指针
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item,0));
		if(prootItem){
			prootItem->setExpanded(true);
		}
	}
}

//收缩
void CCMainWindow::onItemCollapsed(QTreeWidgetItem* item)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (!bIsChild)
	{
		//dynamic_cast 将基类对象指针(或引用)转换到继承类指针
		RootContactItem* prootItem = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (prootItem) {
			prootItem->setExpanded(false);
		}
	}
}

void CCMainWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	//先判断一下双击的是人还是群
	bool bIsChild = item->data(0,Qt::UserRole).toBool();
	if (bIsChild)
	{
		WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString());
		////QString strGroup = m_groupMap.value(item);
		//if (strGroup == QString::fromLocal8Bit("公司群"))
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0,Qt::UserRole + 1).toString(),COMPANY);
		//}
		//else if (strGroup == QString::fromLocal8Bit("人事部"))
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(),PERSONELGROUP);
		//}
		//else if (strGroup == QString::fromLocal8Bit("市场部"))
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(),MARKETGROUP);
		//}
		//else if (strGroup == QString::fromLocal8Bit("研发部"))
		//{
		//	WindowManager::getInstance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toString(),DEVELOPMENTGROUP);
		//}

	}
	
}
