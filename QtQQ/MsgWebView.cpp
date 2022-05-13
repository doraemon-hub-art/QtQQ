#include "MsgWebView.h"
#include "TalkWindowShell.h"
#include "WindowManager.h"
#include <QSqlQueryModel>

extern QString gstrLoginHeadPath;

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString msgLPicPath)
{
	m_msgLPicPath = msgLPicPath;
	initHtmlTmpl();
}


void MsgHtmlObj::initHtmlTmpl()
{
	m_msgLHtmlTmpl = getMsgTmpHtml("msgleftTmpl");
	m_msgLHtmlTmpl.replace("%1", m_msgLPicPath);

	m_msgRHtmlTmpl = getMsgTmpHtml("msgrightTmpl");
	m_msgRHtmlTmpl.replace("%1", gstrLoginHeadPath);
}

QString MsgHtmlObj::getMsgTmpHtml(const QString& code)
{
	QFile file(":/Resources/MainWindow/MsgHtml/" + code + ".html");
	file.open(QFile::ReadOnly);
	QString strData;
	if (file.isOpen()) {
		strData = QLatin1String(file.readAll());
	}
	else {
		QMessageBox::information(nullptr, "Tips", "Failed to init html!");
		file.close();
		return QString("");
	}

	file.close();
	return strData;
}

bool MsgWebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	//仅接收qrc:/*.html
	if (url.scheme() == QString("qrc"))//判断url类型
	{
		return true;
	}
	return false;
}


MsgWebView::MsgWebView(QWidget *parent)
	: QWebEngineView(parent),
	m_channel(new QWebChannel(this))
{
	MsgWebPage* page = new MsgWebPage(this);
	this->setPage(page);

	
	m_msgHtmlObj = new MsgHtmlObj(this);

	//external0用于自己发送信息，这个网页对象
	m_channel->registerObject("external0", m_msgHtmlObj);

	

	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->
		getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, 
		&TalkWindowShell::updateSendTcpMsg);

	//获取当前正构建的聊天窗口的ID(QQ号)
	QString strTalkId = WindowManager::getInstance()->getCreateingTalkId();
	qDebug()<<QString::fromLocal8Bit("当前正在构建的窗口:")<<strTalkId<<endl;

	QSqlQueryModel queryEmployeeModel;
	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroupTalk = false;

	//获取公司群ID
	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE \
		department_name = '%1'").
		arg(QStringLiteral("公司群")));

	QModelIndex companyIndex = queryEmployeeModel.index(0, 0);
	QString strCompanyID = queryEmployeeModel.data(companyIndex).toString();

	if(strTalkId == strCompanyID){//公司群聊
		isGroupTalk = true;
		queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE \
		status = 1"));
	} else {
		//判断是普通群聊还是私聊
		if (strTalkId.length() == 4) {//其他群聊
			isGroupTalk = true;
			queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees \
				WHERE status = 1 AND departmentID = %1")
				.arg(strTalkId));

			
		} else {//私聊
			queryEmployeeModel.setQuery(QString("SELECT picture FROM tab_employees WHERE \
			status  = 1 AND employeeID = %1").
			arg(strTalkId));
			
			QModelIndex index = queryEmployeeModel.index(0,0);
			strPicturePath = queryEmployeeModel.data(index).toString();

			strExternal = "external_" + strTalkId;

			MsgHtmlObj *msgHtmlObj = new MsgHtmlObj(this,strPicturePath	);
			m_channel->registerObject(strExternal,msgHtmlObj);
			  
		}
	}
	
	//处理群聊
	if (isGroupTalk) {
		QModelIndex employeeModelIndex, pictureModelIndex;
		int rows = queryEmployeeModel.rowCount();
		for (int i = 0; i < rows; i++) {
			employeeModelIndex = queryEmployeeModel.index(i, 0);
			pictureModelIndex = queryEmployeeModel.index(i, 1);

			strEmployeeID = queryEmployeeModel.data(employeeModelIndex).toString();
			strPicturePath = queryEmployeeModel.data(pictureModelIndex).toString();

			strExternal = "external_" + strEmployeeID;

			MsgHtmlObj* msgHtmlObj = new MsgHtmlObj(this, strPicturePath);
			m_channel->registerObject(strExternal, msgHtmlObj);
		}
	}



	this->page()->setWebChannel(m_channel); 
	//初始化收信息页面
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{
}

void MsgWebView::appendMsg(const QString& html, QString strObj)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);//解析html

	int msgType = 1;//信息类型，0是表情 1是文本 2文件
	QString strData;//表情编号宽度为3
	int imageNum = 0;

	bool isImageMsg = false;


	for (int i = 0; i < msgList.size(); i++) {

		if (msgList.at(i).at(0) == "img") {
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;

			//获取表情名称的位置
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();//表情路径长度
			isImageMsg = true;

			//获取表情名称
			QString strEmotionName = imagePath.mid(pos);
			strEmotionName.replace(".png","");

			//根据表情名称的长度，进行设置表情数据，补足3位，23-023
			int emotionNameL = strEmotionName.length();
			//判断长度
			if (emotionNameL == 1) {
				strData = strData + "00" + strEmotionName;
			} else if (emotionNameL == 2) {
				strData = strData + "0" + strEmotionName;
			} else if (emotionNameL == 3) {
				strData = strData + strEmotionName;
			}

			msgType = 0;//表情信息
			imageNum++;

			if (imagePath.left(3) == "qrc") {
				pixmap.load(imagePath.mid(3));//去掉表情路径中的qrc
			}
			else {
				pixmap.load(imagePath);
			}


			//表情图片html格式文本组合
			QString imgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>") 
				.arg(imagePath).arg(pixmap.width()).arg(pixmap.height());
		
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text") {
			qsMsg += msgList.at(i).at(1);
			strData = qsMsg;
		}
	}

	//插入到Json对象中，是键值对
	msgObj.insert("MSG",qsMsg);

	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	//发信息
	if (strObj == "0") {
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		if (isImageMsg) {
			strData = QString::number(imageNum) + "images" + strData;
		}
		emit signalSendMsg(strData, msgType);

	} else {   //来信息
		this->page()->runJavaScript(QString("recvHtml_%1(%2)")
			.arg(strObj)
			.arg(Msg));
	}
	
}

QList<QStringList> MsgWebView::parseHtml(const QString& html)
{
	//将传进来的html格式，转换成QT结点文件
	QDomDocument doc;
	doc.setContent(html);
	const QDomElement& root = doc.documentElement();//结点元素
	const QDomNode& node = root.firstChildElement("body");
	return parseDocNode(node);
}

//解析结点
QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	QList<QStringList>attribute;//最终解析出来的是字符串链表
	const QDomNodeList& list = node.childNodes();//返回左右子节点


	for (int i = 0; i < list.count(); i++) {
		//挨个获取当前链表中的结点
		const QDomNode& node = list.at(i);

		//再对结点进行解析，判断是否为元素
		if (node.isElement()) {
			//转换元素
			const QDomElement& element = node.toElement();
			if (element.tagName() == "img") {
				QStringList attributeList;
				attributeList << "img" <<element.attribute("src");
				attribute << attributeList;
			}

			if (element.tagName() == "span") {
				QStringList attributeList;
				attributeList << "text" << element.text();
				attribute << attributeList;
			}

			if (node.hasChildNodes()) {
				attribute<<parseDocNode(node);
			}
		}
	}

	return attribute;
}


