#pragma once

#include <QTextEdit>
#include <QMovie>
#include <QUrl>

class QMsgTextEdit : public QTextEdit
{
	Q_OBJECT


public:
	QMsgTextEdit(QWidget *parent = nullptr);
	~QMsgTextEdit();

private slots:
	void onEmotionImageFrameChange(int frame);//动画改变时， 响应的槽函数

public:
	void addEmotionUrl(int emotionNum);
	void deleteAllEmotionImage();

private:
	QList<QString>m_listEmotionUrl;
	QMap<QMovie*, QString>m_emootionMap;

};
