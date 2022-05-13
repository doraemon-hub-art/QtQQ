#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"
#include "CommonUtils.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent = Q_NULLPTR);
	~TalkWindowItem();

	void setHeadPixmap(const QPixmap& pixmap);
	void setMsgLabelContent(const QString& msg);
	QString getMsgLabelText();


private:
	void initControl();
	
signals:
	void signalCloseClicked();

private:
	void enterEvent(QEvent* event);//鼠标移入事件
	void leaveEvent(QEvent* event);//鼠标移出事件
	void resizeEvent(QResizeEvent* event);


private:
	Ui::TalkWindowItem ui;
};
