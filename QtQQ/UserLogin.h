#pragma once

#include <QWidget>
#include "ui_UserLogin.h"
#include "basicwindow.h"
#include "CCMainWindow.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
class UserLogin : public BasicWindow
{
	Q_OBJECT

public:
	UserLogin(QWidget *parent = Q_NULLPTR);
	~UserLogin();

private slots:
	void onLoginBtnClicked();

private:
	void initControl();

	bool connectMySql();//�������ݿ�
	bool verfyAccountCode(bool &isAccount,QString &strAccount);//��֤


private:
	Ui::UserLogin ui;

};
