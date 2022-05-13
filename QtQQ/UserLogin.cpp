#include "UserLogin.h"
#include "CCMainWindow.h"
#include <QMessageBox>


QString gLoginEmployeeID;//��½��QQ��(Ա����)


UserLogin::UserLogin(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	// �����캯������һ��
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

	// ��Ϊ�ǿյ�, ���Բ���ֱ������,Ҫ�ȵ����� ת��
	connect(ui.loginBtn, &QPushButton::clicked, this, &UserLogin::onLoginBtnClicked);

	//�������ݿ�
	if (!connectMySql()) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("�������ݿ�ʧ��"));
		close();
	}
}

bool UserLogin::connectMySql()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("xxxxxx");		//���ݿ�����
	db.setHostName("xxxxxx");	//������
	db.setUserName("xxxxxx");			//�û���
	db.setPassword("xxxxxx");	//����
	db.setPort(3306);				//�˿�
	

	if (db.open())
	{
		/*QMessageBox::information(NULL, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("�������ݿ�ɹ�"));*/
		return true;
	}
	//�жϳ���
	QSqlError error;
	error = db.lastError();
	if (error.isValid())//��������ʱisValid()����true
	{
		switch (error.type()) {
		case QSqlError::NoError:
			qDebug() << "�޴���";
			break;
		case QSqlError::ConnectionError://���Ӵ���
			qDebug() << error.text();
			break;
		case QSqlError::StatementError://������
			qDebug() << error.text();
			break;
		case QSqlError::TransactionError://�������
			qDebug() << error.text();
			break;
		default://δ֪����
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

	//����Ա����(QQ�ŵ�¼)	
	//�ؼ��ִ�д������Сд������鿴
	//��ѯ��䡪��д��sql���Ҫ��navicat�ϲ���һ��
	QString strSqlCode = QString("SELECT code FROM tab_accounts WHERE employeeID = '%1'").arg(strAccountInput);
	QSqlQuery queryEmployeeID(strSqlCode);
	queryEmployeeID.exec();//ִ��

	//ָ��������һ������ѯ�ĵ�һ������
	if (queryEmployeeID.first()) {
		//���ݿ���QQ�Ŷ�Ӧ������
		QString strCode = queryEmployeeID.value(0).toString();//�õ�����
		if (strCode == strCodeInput) {
			gLoginEmployeeID = strAccountInput;
			strAccount = strAccountInput;
			isAccount = false;
			return true;
		}else {
			return false; 
		}
	}

	//�û�����¼
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
	QString strAccount;//�û�����QQ��
	if (!verfyAccountCode(isAccountLogin,strAccount)) {
		QMessageBox::information(NULL, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("��������˺Ż���������,����������"));
		ui.editPassword->setText("");
		ui.editUserAccount->setText("");
		return;
	}

	//���µ�¼״̬Ϊ��¼

	QString strSqlStatus = QString("UPDATE tab_employees SET online = 2 WHERE \
		employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);
	queryStatus.exec();

	close();
	CCMainWindow* mainWindow = new CCMainWindow(strAccount,isAccountLogin);
	mainWindow->show();
}
