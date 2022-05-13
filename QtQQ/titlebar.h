#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QStyleOption>
#include <QApplication>
#include <QMouseEvent>
#include <QDesktopWidget>
#include "CommonUtils.h"
#include "NotifyManager.h"


//���������Ͻǰ�ť���
enum ButtonType
{
	MIN_BUTTON = 0,//��С�����رհ�ť
	MIN_MAX_BUTTON,//��С������󻯼��رհ�ť
	ONLY_CLOSE_BUTTON//ֻ�йرհ�ť
};
//�Զ��������
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget *parent);
	~TitleBar();
	void setTitleIcon(const QString &filePath);			//���ñ�����ͼ��
	void setTitleContent(const QString& titleContent);	//���ñ���������
	void setTitleWidth(int width);					//���ñ��������
	void setButtonType(ButtonType buttonType);		//���ñ�������ť����
	//���桢��ȡ�������ǰ���ڵ�λ�úͼ���С
	void saveRestoreInfo(const QPoint &point, const QSize &size);
	void getRestoreInfo(QPoint &point, QSize &size);
private:
	void paintEvent(QPaintEvent* event);			//��ͼ�¼�
	void mouseDoubleClickEvent(QMouseEvent *event);//���˫���¼�
	void mousePressEvent(QMouseEvent *event);		 //��갴���¼�
	void mouseMoveEvent(QMouseEvent* event);	     //����ƶ��¼�
	void mouseReleaseEvent(QMouseEvent* event);	 //����ɿ��¼�

	void initControl();								 //��ʼ���ؼ�
	void initConnections();							 //��ʼ���ź���۵�����
	void loadStyleSheet(const QString &sheetName); //������ʽ��
signals:
	//��������ť���ʱ�������ź�
	void signalButtonMinClicked();					 //��С����ť
	void signalButtonRestoreClicked();				 //��󻯻�ԭ��ť
	void signalButtonMaxClicked();	         		 //��󻯰�ť
	void signalButtonCloseClicked();				 //�رհ�ť
private slots:
	//��ť��Ӧ�Ĳۡ���ע��ͬ��
	void onButtonMinClicked();						 //��С��
	void onButtonRestoreClicked();					 //��󻯻�ԭ
	void onButtonMaxClicked();						 //���
	void onButtonCloseClicked();					 //�ر�
private:
	QLabel* m_pIcon;								 //������ͼ��
	QLabel* m_pTitleContent;						 //����������
	QPushButton* m_pButtonMin;						 //��С����ť
	QPushButton* m_pButtonRestore;					 //��󻯻�ԭ��ť
	QPushButton* m_pButtonMax;					     //��󻯰�ť
	QPushButton* m_pButtonClose;					 //�رհ�ť
	//��󻯻�ԭ��ť�����������ڱ�����С��ǰ�����λ���Լ���С
	QPoint m_restorePos;
	QSize m_restoreSize;
	//�ƶ����ڵı���
	bool m_isPressed;
	QPoint m_startMovePos;
	QString m_titleContent;							//����������
	ButtonType m_buttonType;						//���������Ͻǰ�ť����


};
