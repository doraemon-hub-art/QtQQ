#include "EmotionWindow.h"
#include <QStyleOption>
#include <QPainter>

const int emotionColumn = 14;
const int emotionRow = 12;


EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint| Qt::SubWindow);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);

	ui.setupUi(this);

	initControl();
}

EmotionWindow::~EmotionWindow()
{
}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this,"EmotionWindow");

	for (int row = 0; row < emotionRow; row++)
	{
		for (int column = 0; column < emotionColumn; column++)
		{
			EmotionLabelItem* label = new EmotionLabelItem(this);
			label->setEmotionName(row* emotionColumn + column);
			connect(label,&EmotionLabelItem::emotionClicked,this,&EmotionWindow::addEmoton);
			ui.gridLayout->addWidget(label, row, column);
		}
	}
}

void EmotionWindow::addEmoton(int emotionNum)
{
	hide();
	emit signalEmotionWindowHide();
	emit signalEmotionItemClicked(emotionNum);
}

void EmotionWindow::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);

	QPainter painter(this);

	style()->drawPrimitive(QStyle::PE_Widget,&opt,&painter,this);

	__super::paintEvent(event);//进行其他的绘图操作
}
