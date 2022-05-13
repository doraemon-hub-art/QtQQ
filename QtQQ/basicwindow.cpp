#include "basicwindow.h"

extern QString gLoginEmployeeID;

BasicWindow::BasicWindow(QWidget *parent)
	: QDialog(parent)
{
	m_colorBackGround = CommonUtils::getDefaultSkinColor();

	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);

	connect(NotifyManager::getInstance(), SIGNAL(signalSkinChanged(const QColor&)), this, SLOT(onSignalSkinChanged(const QColor&)));
}

BasicWindow::~BasicWindow()
{

}

//�ı�Ƥ��
void BasicWindow::onSignalSkinChanged(const QColor &color)
{
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);//������ʽ��
}

//���ñ�����
void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon)
{
	_titleBar->setTitleIcon(icon);
	_titleBar->setTitleContent(title);
}

//��ʼ��������
void BasicWindow::initTitleBar(ButtonType buttontype)
{
	_titleBar = new TitleBar(this);
	_titleBar->setButtonType(buttontype);
	_titleBar->move(0, 0);

	connect(_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	connect(_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestroreClicked()));
	connect(_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
	connect(_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));

}

//������ʽ��
void BasicWindow::loadStyleSheet(const QString& sheetName)
{
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);

	if (file.isOpen())
	{
		setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());			// ��������ȫ����ȡ����, ���浽 qstyleSheet�ַ�����

		// ��ȡ��ǰ�û���Ƥ��RGBֵ
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		// titleskin ����Ƥ������Ϊ true
		// �ײ���Ƥ�� bottomskin Ϊ true
		// rgba �� a ,��͸����
		qsstyleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												border-top-left-radius:4px;}\
												QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												background-color:rgba(%1,%2,%3,50);\
												border-bottom-left-radius:4px;\
												border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);
		// ������ʽ��
		setStyleSheet(qsstyleSheet);
	}

	file.close();

}

//��ʼ������
void BasicWindow::initBackGroundColor()
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//���໯����ʱ����Ҫ��д��ͼ�¼����ñ���ͼ
void BasicWindow::paintEvent(QPaintEvent* event)
{
	initBackGroundColor();
	QDialog::paintEvent(event);
}

//ͷ��תԲͷ��
QPixmap BasicWindow::getRoundImage(const QPixmap& src, QPixmap& mask, QSize maskSize)
{
	if (maskSize == QSize(0, 0))
	{
		maskSize = mask.size();
	}
	else
	{
		mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}
	//����ת�����ͼ��
	QImage resultImage(maskSize, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&resultImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(resultImage.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.drawPixmap(0, 0, src.scaled(maskSize,Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.end();
	return QPixmap::fromImage(resultImage);
}

void BasicWindow::onShowClose(bool)
{
	close();
}

void BasicWindow::onShowMin(bool)
{
	showMinimized();
}

void BasicWindow::onShowHide(bool)
{
	hide();
}

void BasicWindow::onShowNormal(bool)
{
	show();
	activateWindow();
}

void BasicWindow::onShowQuit(bool)
{
	//���µ�¼״̬Ϊ����
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 1 WHERE \
		employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);
	queryStatus.exec();

	QApplication::quit();
}

//����ƶ��¼�
void BasicWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_mousePressed && (event->buttons() && Qt::LeftButton))//�������
	{
		move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

//��갴���¼�
void BasicWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{ 
		m_mousePressed = true;
		m_mousePoint = event->globalPos() - pos();
		event->accept();
	}
}

//����ͷ��¼�
void BasicWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_mousePressed = false;
}

//��С����ť
void BasicWindow::onButtonMinClicked()
{
	if (Qt::Tool == (windowFlags() & Qt::Tool))
	{
		hide();
	}
	else
	{
		showMinimized();
	}
}

//��󻯻�ԭ��ť
void BasicWindow::onButtonRestoreClicked()
{
	QPoint windowPos;
	QSize windowSize;
	_titleBar->getRestoreInfo(windowPos, windowSize);//��ȡ����λ���Լ���С����Ϣ
	setGeometry(QRect(windowPos, windowSize));//���ü�������
}

void BasicWindow::onButtonMaxClicked()
{
	_titleBar->saveRestoreInfo(pos(), QSize(width(), height()));
	QRect deskTopRect = QApplication::desktop()->availableGeometry();
	QRect factRect = QRect(deskTopRect.x() - 3, deskTopRect.y() - 3, deskTopRect.width() + 6, deskTopRect.height() + 6);
	setGeometry(factRect);
}

void BasicWindow::onButtonCloseClicked()
{
	close();
}
