#include "WindowManager.h"


//单利模式，创建全局静态对象。
Q_GLOBAL_STATIC(WindowManager, theInstance);

WindowManager::WindowManager():QObject(nullptr),m_talkwindowshell(nullptr)
{

}

WindowManager::~WindowManager()
{

}

QWidget* WindowManager::findWindowName(const QString& qsWindowName)
{
	//根据映射查看
	if (m_windowMap.contains(qsWindowName)) {
		return m_windowMap.value(qsWindowName);
	}
	return nullptr;
}

void WindowManager::deleteWindowName(const QString& qsWindowName)
{
	m_windowMap.remove(qsWindowName);//根据
}

void WindowManager::addWindowName(const QString& qsWindownName, QWidget* qWidget)
{
	if (!m_windowMap.contains(qsWindownName)) {
		m_windowMap.insert(qsWindownName,qWidget); 
	}
}

WindowManager* WindowManager::getInstance()
{
	return theInstance();//返回当前实例
}

TalkWindowShell* WindowManager::getTalkWindowShell() {
	return m_talkwindowshell;
}

void WindowManager::addNewTalkWindow(const QString& uid)
{
	if (m_talkwindowshell == nullptr) {
		m_talkwindowshell = new TalkWindowShell;
		connect(m_talkwindowshell, &TalkWindowShell::destroyed,[this](QObject*) {
			m_talkwindowshell = nullptr;
		});
	}
	//判断当前窗口是否已经打开
	QWidget* widget = findWindowName(uid);
	if (!widget)
	{
		m_strCreatingTalkId = uid;
		TalkWindow* talkwindow = new TalkWindow(m_talkwindowshell,uid);
		TalkWindowItem* talkwindowItem = new TalkWindowItem(talkwindow);
		
		m_strCreatingTalkId = "";

		//判断是群聊还是单聊
		QSqlQueryModel sqlDepModel;
		QString strSql = QString("SELECT department_name,sign FROM tab_department WHERE departmentID  = %1")
			.arg(uid);
		sqlDepModel.setQuery(strSql);
		int rows = sqlDepModel.rowCount();
		QString strWindowName;
		QString strMsgLabel;

		//单聊
		if (rows == 0) {
			QString sql = QString("SELECT employee_name,employee_sign FROM tab_employees WHERE employeeID = %1")
				.arg(uid);
			sqlDepModel.setQuery(sql);
		}
		QModelIndex indexDepIndex;
		QModelIndex  signIndex;
		indexDepIndex = sqlDepModel.index(0, 0);//0行0列
		signIndex = sqlDepModel.index(0, 1);//0行1列
		strWindowName = sqlDepModel.data(signIndex).toString();
		strMsgLabel = sqlDepModel.data(indexDepIndex).toString();

		talkwindow->setWindowName(strWindowName);//窗口名称
		talkwindowItem->setMsgLabelContent(strMsgLabel);//左侧联系人文本显示
		m_talkwindowshell->addTalkWindow(talkwindow, talkwindowItem, uid);

	}
	else
	{
		//更新左右列表

		//左侧聊天列表设为被选中
		QListWidgetItem* item = m_talkwindowshell->getTalkWindowItemMap().key(widget);
		item->setSelected(true);

		//设置右侧当前聊天窗口
		m_talkwindowshell->setCurrentWidget(widget);
	}
	m_talkwindowshell->show();
	m_talkwindowshell->activateWindow();

}

QString WindowManager::getCreateingTalkId() {
	return m_strCreatingTalkId;
}

