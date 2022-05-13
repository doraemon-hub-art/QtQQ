#pragma once

#include<QPixmap>
#include<QSize>
#include<QProxyStyle>
#include<QPainter>
#include<QFile>
#include<QWidget>
#include<QApplication>
#include<QSettings>



class CommonUtils
{
public:
	CommonUtils();
public:
	static QPixmap getRoundImage(const QPixmap& src, QPixmap& mask, QSize masksize = QSize(0, 0));
	static void loadStyleSheet(QWidget* widget, const QString& sheetName);
	static void setDefaultSkinColor(const QColor& color);
	static QColor getDefaultSkinColor();
};

