#include "TalkWindow.h"
#include "WindowManager.h"

extern QString gLoginEmployeeID;

TalkWindow::TalkWindow(QWidget *parent, const QString& uid)
	: QWidget(parent), m_talkId(uid)
{
	ui.setupUi(this);

	//����˫���ĵ�һ����ϵ�˽��д��ڵĹ��죬���ӵ�����ӳ����
	WindowManager::getInstance()->addWindowName(m_talkId,this);
	setAttribute(Qt::WA_DeleteOnClose);
	initGroupTalkStatus();
	initControl();
}

TalkWindow::~TalkWindow()
{
	WindowManager::getInstance()->deleteWindowName(m_talkId);
}

void TalkWindow::addEmotionImage(int emotionNum)
{
	ui.textEdit->setFocus();
	ui.textEdit->addEmotionUrl(emotionNum);
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setText(name);
}

QString TalkWindow::getTalkId() {
	return m_talkId;
}

void TalkWindow::onSendBtnClicked(bool)
{
	if (ui.textEdit->toPlainText().isEmpty()) {

		QToolTip::showText(this->mapToGlobal(QPoint(630,660)),
			QString::fromLocal8Bit("���͵���Ϣ����Ϊ��!"),this,QRect(0,0,120,100),2000);
		return;
	}
	//������ͱ�����ٷ������ֳ��ִ���
	QString html = ui.textEdit->document()->toHtml();

	//�ı�html���û����������������
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QString text = ui.textEdit->toPlainText();
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1",text);
			file.close();
		}
		else {
			QMessageBox::information(this,QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("�ļ� ,msgFont.txt�����ڣ�"));
			return;
		}
		
		if (!html.contains(fontHtml)) {
			html.replace(text,fontHtml);
			//�������ٰ�ֵ������
			ui.textEdit->document()->toHtml() = html;
		}
	}
	//������ֺͱ���
	ui.textEdit->clear();
	ui.textEdit->deleteAllEmotionImage();

	ui.msgWidget->appendMsg(html);//����Ϣ����������Ϣ





}

void TalkWindow::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	bool bIsChild = item->data(0, Qt::UserRole).toBool();
	if (bIsChild) {

		QString talkId = item->data(0, Qt::UserRole + 1).toString();
		//�������Լ�-�����ӶԻ���
		if (talkId == gLoginEmployeeID) {
			return;
		}
		//��ȡ��������
		QString strPeopleName = m_groupPeopleMap.value(item);
		WindowManager::getInstance()->addNewTalkWindow(item->data(0,Qt::UserRole + 1).toString());

	}
}
 


void TalkWindow::initControl()
{
	QList<int> rightWidgetSize;
	rightWidgetSize << 600 << 138;
	ui.bodySplitter->setSizes(rightWidgetSize);

	ui.textEdit->setFontPointSize(10);
	ui.textEdit->setFocus();

	connect(ui.sysmin, SIGNAL(clicked(bool)), parent(), SLOT(onShowMin(bool)));
	connect(ui.sysclose, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));
	connect(ui.closeBtn, SIGNAL(clicked(bool)), parent(), SLOT(onShowClose(bool)));

	connect(ui.faceBtn, SIGNAL(clicked(bool)), parent(), SLOT(onEmotionBtnClicked(bool)));

	connect(ui.sendBtn, SIGNAL(clicked(bool)), this,SLOT(onSendBtnClicked(bool)));

	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
		this,SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));
	


	//�ж���Ⱥ�Ļ���˽��
	if (m_isGroupTalk) {
		initTalkWindow();
	}
	else {//����
		initPtoPTalk();
	}
	
}

void TalkWindow::initGroupTalkStatus()
{
	QSqlQueryModel sqlDepModel;
	QString strSql = QString("SELECT * FROM tab_department WHERE departmentID = %1")
		.arg(m_talkId);
	sqlDepModel.setQuery(strSql);
	
	int rows = sqlDepModel.rowCount();
	if (rows == 0) {
		//��������
		m_isGroupTalk = false;

	}
	else {
		m_isGroupTalk = true;
	}

}

//��ʼ��Ⱥ��
void TalkWindow::initTalkWindow()
{
	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	pRootItem->setData(0, Qt::UserRole, 0);
	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);
	ui.treeWidget->setFixedHeight(646);
	
	//��ǰ�����Ⱥ����������
	QString strGroupName;
	QSqlQuery queryGroupName(QString("SELECT department_name FROM tab_department WHERE departmentID = %1")
		.arg(m_talkId));
	queryGroupName.exec();
	if (queryGroupName.next()) {
		strGroupName = queryGroupName.value(0).toString();
	}
	
	
	
	QSqlQueryModel queryEmployeeModel;
	if (getComDepID() == m_talkId.toInt()) {
		queryEmployeeModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
	}else {
		queryEmployeeModel.setQuery(QString("SELECT employeeID FROM tab_employees WHERE status = 1 AND departmentID = %1")
		.arg(m_talkId));
	}

	int nEmployeeNum = queryEmployeeModel.rowCount();//��ȡԱ������



	QString qsGroupName = QString::fromLocal8Bit("%1 %2/%3")
		.arg(strGroupName)
		.arg(0)
		.arg(nEmployeeNum);
	pItemName->setText(qsGroupName);
	
	//���������
	ui.treeWidget->addTopLevelItem(pRootItem);
	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);
	
	//չ��
	pRootItem->setExpanded(true);
	
	for (int i = 0; i < nEmployeeNum; i++)
	{
		QModelIndex modelIndex = queryEmployeeModel.index(i,0);
		int employeeID = queryEmployeeModel.data(modelIndex).toInt();
		
		//����ǵ�½���Լ�
		/*if (employeeID == gLoginEmployeeID.toInt()) {
			continue;
		}*/

		//�����ӽ��
		addPeopInfo(pRootItem,employeeID);
	}
}

int TalkWindow::getComDepID()
{
	QSqlQuery queryDepID(QString("SELECT departmentID FROM tab_department WHERE department_name = '%1'")
	.arg(QString::fromLocal8Bit("��˾Ⱥ")));
	queryDepID.exec();
	queryDepID.next();

	return queryDepID.value(0).toInt();
}

//void TalkWindow::initCompanyTalk()
//{
//	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
//	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
//	pRootItem->setData(0,Qt::UserRole,0);
//	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);
//	ui.treeWidget->setFixedHeight(646);
//	int nEmployeeNum = 50;
//	QString qsGroupName = QString::fromLocal8Bit("��˾Ⱥ %1/%2").arg(0).arg(nEmployeeNum);
//	pItemName->setText(qsGroupName);
//
//	//���������
//	ui.treeWidget->addTopLevelItem(pRootItem);
//	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);
//	
//	//չ��
//	pRootItem->setExpanded(true);
//	
//	for (int i = 0; i < nEmployeeNum; i++)
//	{
//		addPeopInfo(pRootItem);
//	}
//}
//
//void TalkWindow::initPersonelTalk()
//{
//	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
//	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
//	pRootItem->setData(0, Qt::UserRole, 0);
//	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);
//	ui.treeWidget->setFixedHeight(646);
//	int nEmployeeNum = 5;
//	QString qsGroupName = QString::fromLocal8Bit("���²� %1/%2").arg(0).arg(nEmployeeNum);
//	pItemName->setText(qsGroupName);
//
//	//���������
//	ui.treeWidget->addTopLevelItem(pRootItem);
//	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);
//
//	//չ��
//	pRootItem->setExpanded(true);
//
//	for (int i = 0; i < nEmployeeNum; i++)
//	{
//		addPeopInfo(pRootItem);
//	}
//}
//
//void TalkWindow::initMarketTalk()
//{
//	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
//	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
//	pRootItem->setData(0, Qt::UserRole, 0);
//	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);
//	ui.treeWidget->setFixedHeight(646);
//	int nEmployeeNum = 8;
//	QString qsGroupName = QString::fromLocal8Bit("�г��� %1/%2").arg(0).arg(nEmployeeNum);
//	pItemName->setText(qsGroupName);
//
//	//���������
//	ui.treeWidget->addTopLevelItem(pRootItem);
//	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);
//
//	//չ��
//	pRootItem->setExpanded(true);
//
//	for (int i = 0; i < nEmployeeNum; i++)
//	{
//		addPeopInfo(pRootItem);
//	}
//}
//
//void TalkWindow::initDevelopTalk()
//{
//	QTreeWidgetItem* pRootItem = new QTreeWidgetItem();
//	pRootItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
//	pRootItem->setData(0, Qt::UserRole, 0);
//	RootContactItem* pItemName = new RootContactItem(false, ui.treeWidget);
//	ui.treeWidget->setFixedHeight(646);
//	int nEmployeeNum = 32;
//	QString qsGroupName = QString::fromLocal8Bit("�з��� %1/%2").arg(0).arg(nEmployeeNum);
//	pItemName->setText(qsGroupName);
//
//	//���������
//	ui.treeWidget->addTopLevelItem(pRootItem);
//	ui.treeWidget->setItemWidget(pRootItem, 0, pItemName);
//
//	//չ��
//	pRootItem->setExpanded(true);
//
//	for (int i = 0; i < nEmployeeNum; i++)
//	{
//		addPeopInfo(pRootItem);
//	}
//}

void TalkWindow::initPtoPTalk()
{
	QPixmap pixSkin;
	pixSkin.load(":/Resources/MainWindow/skin.png");

	ui.widget->setFixedSize(pixSkin.size());

	QLabel* skinLabel = new QLabel(ui.widget);
	skinLabel->setPixmap(pixSkin);
	skinLabel->setFixedSize(ui.widget->size());

}

void TalkWindow::addPeopInfo(QTreeWidgetItem* pRootGroupItem, int employeeID)
{
	QTreeWidgetItem* pChild = new QTreeWidgetItem();

	//�����ӽ��
	pChild->setData(0, Qt::UserRole, 1);
	pChild->setData(0, Qt::UserRole + 1,employeeID);
	ContactItem* pContactItem = new ContactItem(ui.treeWidget);



	//��ȡ����ǩ����ͷ��
	QString strName; 
	QString strSign; 
	QString strPicturePath; 

	QSqlQueryModel queryInfoModel;
	queryInfoModel.setQuery(QString("SELECT employee_name,employee_sign,\
			picture FROM tab_employees WHERE employeeID = %1")
			.arg(employeeID));
	
	QModelIndex nameIndex;
	QModelIndex signIndex;
	QModelIndex pictureIndex;
	nameIndex = queryInfoModel.index(0, 0);//����
	signIndex = queryInfoModel.index(0, 1);
	pictureIndex = queryInfoModel.index(0, 2);

	strName = queryInfoModel.data(nameIndex).toString();
	strSign = queryInfoModel.data(signIndex).toString();
	strPicturePath = queryInfoModel.data(pictureIndex).toString();


	QPixmap pix1;
	pix1.load(":/Resources/MainWindow/head_mask.png");

	QImage imageHead;
	imageHead.load(strPicturePath);



	pContactItem->setHeadPixmap(CommonUtils::getRoundImage(QPixmap::fromImage(imageHead),
		pix1,pContactItem->getHeadLabelSize()));
	pContactItem->setUserName(strName);
	pContactItem->setSignName(strSign);

	pRootGroupItem->addChild(pChild);
	ui.treeWidget->setItemWidget(pChild,0,pContactItem);

	QString str = pContactItem->getUserName();
	m_groupPeopleMap.insert(pChild,str);
}