#include "RootContactItem.h"

RootContactItem::RootContactItem(bool hasArrow,QWidget *parent)
	: QLabel(parent)
	,m_rotation(0)
	,m_hasArrow(hasArrow)
{ 
	
	setFixedHeight(32);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	//��ʼ�����Զ���
	m_animation = new QPropertyAnimation(this,"rotation");
	m_animation->setDuration(50);//���õ��ʶ�����ʱ���� 50ms
	m_animation->setEasingCurve(QEasingCurve::InQuad);//����������������t^2��0����

}

RootContactItem::~RootContactItem()
{

}

void RootContactItem::setText(const QString& title)
{
	m_titleText = title;
	update();
}

void RootContactItem::setExpanded(bool expand)
{
	//�ж��Ƿ�չ��
	if (expand)
	{
		m_animation->setEndValue(90);//�������Զ����Ľ���ֵ
	}
	else
	{
		m_animation->setEndValue(0);
	}
	m_animation->start();
}

int RootContactItem::rotation()
{
	return m_rotation;
}

void RootContactItem::setRotation(int rotation)
{
	m_rotation = rotation;
	update();//�����ػ����
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::TextAntialiasing, true);//�ı������

	QFont font;
	font.setPointSize(10);
	painter.setFont(font);

	painter.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter,m_titleText);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.save();//���滭������

	//����м�ͷ
	if (m_hasArrow)
	{
		QPixmap pixmap;
		pixmap.load(":/Resources/MainWindow/arrow.png");

		QPixmap tmpPixmap(pixmap.size());
		tmpPixmap.fill(Qt::transparent);

		QPainter p(&tmpPixmap);
		p.setRenderHint(QPainter::SmoothPixmapTransform,true);
		
		//����ϵƫ�ƣ�x����ƫ�ƣ�y����ƫ��
		p.translate(pixmap.width()/2,pixmap.height()/2);
		//��ת����ϵ��˳ʱ��
		p.rotate(m_rotation);
		p.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2,pixmap);

		painter.drawPixmap(6, (height()-pixmap.height())/2,tmpPixmap);
		painter.restore();//�ָ���������
	}
	QLabel::paintEvent(event);
}