#pragma once

#include "ui_SkinWindow.h"
#include "basicwindow.h"
#include "QClickLabel.h"
#include "NotifyManager.h"
class SkinWindow : public BasicWindow
{
	Q_OBJECT

public:
	SkinWindow(QWidget *parent = Q_NULLPTR);
	~SkinWindow();
public:
	void initControl();
public slots:
	void onShowClose();
private:
	Ui::SkinWindow ui;
};
