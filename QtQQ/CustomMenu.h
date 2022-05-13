#pragma once

#include <QMenu>
#include <QIcon>
#include <QMap>
#include <QAction>
class CustomMenu : public QMenu
{
	Q_OBJECT

public:
	CustomMenu(QWidget *parent = nullptr);
	~CustomMenu();
public:
	void addCustomMenu(const QString& text, const QString& icon, const QString& name);
	QAction* getAction(const QString& text);
private:
	QMap<QString, QAction*>m_menuActionMap;
};
