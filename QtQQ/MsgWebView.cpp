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
	//������qrc:/*.html
	if (url.scheme() == QString("qrc"))//�ж�url����
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

	//external0�����Լ�������Ϣ�������ҳ����
	m_channel->registerObject("external0", m_msgHtmlObj);

	

	TalkWindowShell* talkWindowShell = WindowManager::getInstance()->
		getTalkWindowShell();
	connect(this, &MsgWebView::signalSendMsg, talkWindowShell, 
		&TalkWindowShell::updateSendTcpMsg);

	//��ȡ��ǰ�����������촰�ڵ�ID(QQ��)
	QString strTalkId = WindowManager::getInstance()->getCreateingTalkId();
	qDebug()<<QString::fromLocal8Bit("��ǰ���ڹ����Ĵ���:")<<strTalkId<<endl;

	QSqlQueryModel queryEmployeeModel;
	QString strEmployeeID, strPicturePath;
	QString strExternal;
	bool isGroupTalk = false;

	//��ȡ��˾ȺID
	queryEmployeeModel.setQuery(QString("SELECT departmentID FROM tab_department WHERE \
		department_name = '%1'").
		arg(QStringLiteral("��˾Ⱥ")));

	QModelIndex companyIndex = queryEmployeeModel.index(0, 0);
	QString strCompanyID = queryEmployeeModel.data(companyIndex).toString();

	if(strTalkId == strCompanyID){//��˾Ⱥ��
		isGroupTalk = true;
		queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees WHERE \
		status = 1"));
	} else {
		//�ж�����ͨȺ�Ļ���˽��
		if (strTalkId.length() == 4) {//����Ⱥ��
			isGroupTalk = true;
			queryEmployeeModel.setQuery(QString("SELECT employeeID,picture FROM tab_employees \
				WHERE status = 1 AND departmentID = %1")
				.arg(strTalkId));

			
		} else {//˽��
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
	
	//����Ⱥ��
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
	//��ʼ������Ϣҳ��
	this->load(QUrl("qrc:/Resources/MainWindow/MsgHtml/msgTmpl.html"));
}

MsgWebView::~MsgWebView()
{
}

void MsgWebView::appendMsg(const QString& html, QString strObj)
{
	QJsonObject msgObj;
	QString qsMsg;
	const QList<QStringList>msgList = parseHtml(html);//����html

	int msgType = 1;//��Ϣ���ͣ�0�Ǳ��� 1���ı� 2�ļ�
	QString strData;//�����ſ��Ϊ3
	int imageNum = 0;

	bool isImageMsg = false;


	for (int i = 0; i < msgList.size(); i++) {

		if (msgList.at(i).at(0) == "img") {
			QString imagePath = msgList.at(i).at(1);
			QPixmap pixmap;

			//��ȡ�������Ƶ�λ��
			QString strEmotionPath = "qrc:/Resources/MainWindow/emotion/";
			int pos = strEmotionPath.size();//����·������
			isImageMsg = true;

			//��ȡ��������
			QString strEmotionName = imagePath.mid(pos);
			strEmotionName.replace(".png","");

			//���ݱ������Ƶĳ��ȣ��������ñ������ݣ�����3λ��23-023
			int emotionNameL = strEmotionName.length();
			//�жϳ���
			if (emotionNameL == 1) {
				strData = strData + "00" + strEmotionName;
			} else if (emotionNameL == 2) {
				strData = strData + "0" + strEmotionName;
			} else if (emotionNameL == 3) {
				strData = strData + strEmotionName;
			}

			msgType = 0;//������Ϣ
			imageNum++;

			if (imagePath.left(3) == "qrc") {
				pixmap.load(imagePath.mid(3));//ȥ������·���е�qrc
			}
			else {
				pixmap.load(imagePath);
			}


			//����ͼƬhtml��ʽ�ı����
			QString imgPath = QString("<img src=\"%1\" width=\"%2\" height=\"%3\"/>") 
				.arg(imagePath).arg(pixmap.width()).arg(pixmap.height());
		
			qsMsg += imgPath;
		}
		else if (msgList.at(i).at(0) == "text") {
			qsMsg += msgList.at(i).at(1);
			strData = qsMsg;
		}
	}

	//���뵽Json�����У��Ǽ�ֵ��
	msgObj.insert("MSG",qsMsg);

	const QString& Msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	//����Ϣ
	if (strObj == "0") {
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(Msg));

		if (isImageMsg) {
			strData = QString::number(imageNum) + "images" + strData;
		}
		emit signalSendMsg(strData, msgType);

	} else {   //����Ϣ
		this->page()->runJavaScript(QString("recvHtml_%1(%2)")
			.arg(strObj)
			.arg(Msg));
	}
	
}

QList<QStringList> MsgWebView::parseHtml(const QString& html)
{
	//����������html��ʽ��ת����QT����ļ�
	QDomDocument doc;
	doc.setContent(html);
	const QDomElement& root = doc.documentElement();//���Ԫ��
	const QDomNode& node = root.firstChildElement("body");
	return parseDocNode(node);
}

//�������
QList<QStringList> MsgWebView::parseDocNode(const QDomNode& node)
{
	QList<QStringList>attribute;//���ս������������ַ�������
	const QDomNodeList& list = node.childNodes();//���������ӽڵ�


	for (int i = 0; i < list.count(); i++) {
		//������ȡ��ǰ�����еĽ��
		const QDomNode& node = list.at(i);

		//�ٶԽ����н������ж��Ƿ�ΪԪ��
		if (node.isElement()) {
			//ת��Ԫ��
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


