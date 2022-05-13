#include "CustomMenu.h"
#include "CommonUtils.h"
CustomMenu::CustomMenu(QWidget *parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
	CommonUtils::loadStyleSheet(this,"Menu");
};

CustomMenu::~CustomMenu()
{
}

void CustomMenu::addCustomMenu(const QString& text, const QString& icon, const QString& name)
{
	// 先创建一个动作按钮
	// 通过 图标 和 文本 的形式，添加
	QAction* pAction = addAction(QIcon(icon), name);
	m_menuActionMap.insert(text, pAction);
}
QAction* CustomMenu::getAction(const QString& text)
{
	return m_menuActionMap[text];
}
