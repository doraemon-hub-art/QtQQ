#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>


QString gLoginEmployeeID;//登陆者QQ号(员工号)


UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	// 将构造函数完善一下
	setAttribute(Qt::WA_DeleteOnClose);
	initTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");

	loadStyleSheet("UserLogin");
	initControl();
}

UserLogin::~UserLogin()
{
}

void UserLogin::initControl()
{
	QLabel* headLabel = new QLabel(this);
	headLabel->setFixedSize(68, 68);

	QPixmap pix(":/Resources/MainWindow/head_mask.png");
	headLabel->setPixmap(getRoundImage(QPixmap(":/Resources/MainWindow/app/logo.ico"),
		pix, headLabel->size()));
	headLabel->move(width() / 2 - 34, ui.titleWidget->height() - 34);

	// 因为是空的, 所以不能直接设置,要先调函数 转换
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	//链接数据库
	if (!connectMySql()) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("链接数据库失败"));
		close();
	}
}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("xxxxxx");		//数据库名称
	db.setHostName("xxxxxx");	//主机名
	db.setUserName("xxxxxx");			//用户名
	db.setPassword("xxxxxx");	//密码
	db.setPort(3306);				//端口
	

	if (db.open())
	{
		/*QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("链接数据库成功"));*/
		return true;
	}
	//判断出错
	QSqlError error;
	error = db.lastError();
	if (error.isValid())//发生错误时isValid()返回true
	{
		switch (error.type()) {
		case QSqlError::NoError:
			qDebug() << "无错误";
			break;
		case QSqlError::ConnectionError://连接错语
			qDebug() << error.text();
			break;
		case QSqlError::StatementError://语句错语
			qDebug() << error.text();
			break;
		case QSqlError::TransactionError://事务错误
			qDebug() << error.text();
			break;
		default://未知错误
			qDebug() << error.text();
			break;
		}
	}
	return false;

}

bool UserLogin::verfyAccountCode(bool &isAccount, QString &strAccount)
{
	QString strAccountInput = ui.editUserAccount->text();
	QString strCodeInput = ui.editPassword->text();

	//输入员工号(QQ号登录)	
	//关键字大写，其他小写，方便查看
	//查询语句——写完sql语句要在navicat上测试一下
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = '%1'").arg(strAccountInput);
	QSqlQuery queryEmployeeID(strSqlCode);
	queryEmployeeID.exec();//执行

	//指向结果集第一条，查询的第一个内容
	if (queryEmployeeID.first()) {
		//数据库中QQ号对应的密码
		QString strCode = queryEmployeeID.value(0).toString();//拿到密码
		if (strCode == strCodeInput) {
			gLoginEmployeeID = strAccountInput;
			strAccount = strAccountInput;
			isAccount = false;
			return true;
		}else {
			return false; 
		}
	}

	//用户名登录
	strSqlCode = QString("SELECT code,employeeID FROM tab_accounts WHERE account = '%1'")
		.arg(strAccountInput);
	QSqlQuery queryAccount(strSqlCode);
	queryAccount.exec();
	if (queryAccount.first()) {
		QString strCode = queryAccount.value(0).toString();
		if (strCode == strCodeInput) {
			gLoginEmployeeID = queryAccount.value(1).toString();
			strAccount = strAccountInput;
			isAccount = true;
			return true;
		}else {
			return false;
		}
	}

	return false;

}

void UserLogin::onLoginBtnClicked()
{
	bool isAccountLogin;
	QString strAccount;//用户名或QQ号
	if (!verfyAccountCode(isAccountLogin,strAccount)) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("提示"),
			QString::fromLocal8Bit("您输入的账号或密码有误,请重新输入"));
		ui.editPassword->setText("");
		ui.editUserAccount->setText("");
		return;
	}

	//更新登录状态为登录

	QString strSqlStatus = QString("UPDATE tab_employees SET online = 2 WHERE \
		employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);
	queryStatus.exec();

	close();
	CCMainWindow* mainWindow = new CCMainWindow(strAccount,isAccountLogin);
	mainWindow->show();
}
