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

//改变皮肤
void BasicWindow::onSignalSkinChanged(const QColor &color)
{
	m_colorBackGround = color;
	loadStyleSheet(m_styleName);//加载样式表
}

//设置标题栏
void BasicWindow::setTitleBarTitle(const QString& title, const QString& icon)
{
	_titleBar->setTitleIcon(icon);
	_titleBar->setTitleContent(title);
}

//初始化标题栏
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

//加载样式表
void BasicWindow::loadStyleSheet(const QString& sheetName)
{
	m_styleName = sheetName;
	QFile file(":/Resources/QSS/" + sheetName + ".css");
	file.open(QFile::ReadOnly);

	if (file.isOpen())
	{
		setStyleSheet("");
		QString qsstyleSheet = QLatin1String(file.readAll());			// 数据内容全部读取出来, 保存到 qstyleSheet字符串里

		// 获取当前用户的皮肤RGB值
		QString r = QString::number(m_colorBackGround.red());
		QString g = QString::number(m_colorBackGround.green());
		QString b = QString::number(m_colorBackGround.blue());

		// titleskin 标题皮肤属性为 true
		// 底部的皮肤 bottomskin 为 true
		// rgba 的 a ,是透明度
		qsstyleSheet += QString("QWidget[titleskin=true]\
												{background-color:rgb(%1,%2,%3);\
												border-top-left-radius:4px;}\
												QWidget[bottomskin=true]\
												{border-top:1px solid rgba(%1,%2,%3,100);\
												background-color:rgba(%1,%2,%3,50);\
												border-bottom-left-radius:4px;\
												border-bottom-right-radius:4px;}")
			.arg(r).arg(g).arg(b);
		// 设置样式表
		setStyleSheet(qsstyleSheet);
	}

	file.close();

}

//初始化背景
void BasicWindow::initBackGroundColor()
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//子类化部件时，需要重写绘图事件设置背景图
void BasicWindow::paintEvent(QPaintEvent* event)
{
	initBackGroundColor();
	QDialog::paintEvent(event);
}

//头像转圆头像
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
	//保存转换后的图像
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
	//更新登录状态为离线
	QString strSqlStatus = QString("UPDATE tab_employees SET online = 1 WHERE \
		employeeID = %1").arg(gLoginEmployeeID);
	QSqlQuery queryStatus(strSqlStatus);
	queryStatus.exec();

	QApplication::quit();
}

//鼠标移动事件
void BasicWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (m_mousePressed && (event->buttons() && Qt::LeftButton))//按下左键
	{
		move(event->globalPos() - m_mousePoint);
		event->accept();
	}
}

//鼠标按下事件
void BasicWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{ 
		m_mousePressed = true;
		m_mousePoint = event->globalPos() - pos();
		event->accept();
	}
}

//鼠标释放事件
void BasicWindow::mouseReleaseEvent(QMouseEvent* event)
{
	m_mousePressed = false;
}

//最小化按钮
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

//最大化还原按钮
void BasicWindow::onButtonRestoreClicked()
{
	QPoint windowPos;
	QSize windowSize;
	_titleBar->getRestoreInfo(windowPos, windowSize);//获取窗体位置以及大小的信息
	setGeometry(QRect(windowPos, windowSize));//设置集合属性
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
