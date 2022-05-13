#include "TalkWindowShell.h"
#include <QListWidget>
#include "EmotionWindow.h"
#include "TalkWindow.h"
#include <QMessageBox>
#include <QFile>

//��½�ߵ�QQ��
extern QString gLoginEmployeeID;

const int gUdpPort = 6666;

QString gfileName;//�ļ�����
QString gfileData;//�ļ�����



TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	initControl();

	initTcpSocket();
	initUdpSocket();

	
	QFile file(":/Resources/MainWindow/MsgHtml/msgtmpl.js");

	if (!file.size()) {
		QStringList employeeIDList;
		getEmployeesID(employeeIDList);

		if (!createJSFile(employeeIDList)){
			QMessageBox::information(this, 
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("����JS�ļ�����ʧ��!"));
		}
	}
}

TalkWindowShell::~TalkWindowShell()
{
	delete m_emotionWindow;
	m_emotionWindow = nullptr;
}



void TalkWindowShell::addTalkWindow(TalkWindow* talkWindow, TalkWindowItem* talkWindowItem,const QString& uid)
{
	ui.rightStackedWidget->addWidget(talkWindow);
	connect(m_emotionWindow, SIGNAL(signalEmotionWindowHide()),talkWindow,SLOT(onSetEmotionBtnStatus()));
	
	QListWidgetItem* aItem = new QListWidgetItem(ui.listWidget);
	m_talkWindowItemMap.insert(aItem, talkWindow);
	
	aItem->setSelected(true);




	//�ж���Ⱥ�Ļ��ǵ���
	QSqlQueryModel sqlDepModel;

	QString strQuery = QString("SELECT picture FROM tab_department WHERE departmentID = %1")
		.arg(uid);
	sqlDepModel.setQuery(strQuery);
	int rows = sqlDepModel.rowCount();
	if (rows == 0) {//����
		strQuery = QString("SELECT picture FROM tab_employees WHERE employeeID = %1")
			.arg(uid);
		sqlDepModel.setQuery(strQuery);
	}
	QModelIndex index;
	index = sqlDepModel.index(0, 0);//�У���
	
	QImage img;
	img.load(sqlDepModel.data(index).toString());

	talkWindowItem->setHeadPixmap(QPixmap::fromImage(img));//����ͷ��
	ui.listWidget->addItem(aItem);
	ui.listWidget->setItemWidget(aItem, talkWindowItem);

	onTalkWindowItemClicked(aItem);

	connect(talkWindowItem, &TalkWindowItem::signalCloseClicked, [talkWindowItem, talkWindow, aItem, this]() {
		m_talkWindowItemMap.remove(aItem);
		talkWindow->close();
		ui.listWidget->takeItem(ui.listWidget->row(aItem));
		delete talkWindowItem;
		ui.rightStackedWidget->removeWidget(talkWindow);
		if (ui.rightStackedWidget->count() < 1)
		{
			close();
		}

	});
}

void TalkWindowShell::setCurrentWidget(QWidget* widget)
{
	ui.rightStackedWidget->setCurrentWidget(widget); 
}

const QMap<QListWidgetItem*, QWidget*>& TalkWindowShell::getTalkWindowItemMap() const
{
	return m_talkWindowItemMap;
}

void TalkWindowShell::initControl()
{
	loadStyleSheet("TalkWindow");
	setWindowTitle(QString::fromLocal8Bit("���촰��"));
	
	m_emotionWindow = new EmotionWindow;
	m_emotionWindow->hide();//���ر��鴰��

	QList<int>leftWidgetSize;
	leftWidgetSize << 154 << width() - 154;
	ui.splitter->setSizes(leftWidgetSize); //���������óߴ�


	ui.listWidget->setStyle(new CustomProxyStyle(this));

	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onTalkWindowItemClicked);
	connect(m_emotionWindow,SIGNAL(signalEmotionItemClicked(int)),this,SLOT(onEmotionItemClicked(int)));

}

void TalkWindowShell::initTcpSocket() {
	m_tcpClientSocket = new QTcpSocket(this);
	m_tcpClientSocket->connectToHost("127.0.0.1",gtcpPort);
}

void TalkWindowShell::initUdpSocket() {
	m_udpReceiver = new QUdpSocket(this);
	for (quint16 port = gUdpPort; port < gUdpPort + 200; port++) {
		if (m_udpReceiver->bind(port, QUdpSocket::ShareAddress)) {
			break;
		}
	}
	connect(m_udpReceiver, &QUdpSocket::readyRead, this, &TalkWindowShell::processPendingData);
}

QStringList& TalkWindowShell::getEmployeesID(QStringList& employeesIDList) {
	QSqlQueryModel queryModel;
	queryModel.setQuery("SELECT employeeID FROM tab_employees WHERE status = 1");
	int employeesNum = queryModel.rowCount();//����ģ�͵�������(Ա���ĸ���)
	QModelIndex index;
	for (int i = 0; i < employeesNum; i++) {
		index = queryModel.index(i, 0);//��һ���ֶΣ������еĲ�����0(��0��ʼ)��
		employeesIDList<<queryModel.data(index).toString();
	}

	return employeesIDList;
}

bool TalkWindowShell::createJSFile(QStringList &employeesList) {
	//��ȡ�ļ�����txt
	QString strFileTxt = ":/Resources/MainWindow/MsgHtml/msgtmpl.txt";
	QFile fileRead(strFileTxt);
	QString strFile;
	
	if (fileRead.open(QIODevice::ReadOnly)) {
		strFile = fileRead.readAll();
		fileRead.close();
	} else {
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("��ȡmsgtml.txtʧ�ܣ�"));
		return false;
	}

	//�滻(external0,appendHtml0�����Լ�����Ϣʹ��)
	QFile fileWrite(":/Resources/MainWindow/MsgHtml/msgtmpl.js");
	if (fileWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		//���¿�ֵ
		QString strSourceInitNull = "var external = null";
		//���³�ʼ��
		QString strSourceInit = "external = channel.objects.external;";
		//����newWebChannel
		QString strSourceNew =
			"new QWebChannel(qt.webChannelTransport,\
			function(channel){\
			external = channel.objects.external;\
		}\
		);\
		";


		//����׷��recvHtml,�ű�����˫�����޷�ֱ�ӽ����ַ�����ֵ�����ö��ļ���ʽ��
		QString strSourceRecvHtml;
		QFile fileRecvHtml(":/Resources/MainWindow/MsgHtml/recvHtml.txt");
		if (fileRecvHtml.open(QIODevice::ReadOnly)) {
			strSourceRecvHtml = fileRecvHtml.readAll();
			fileRecvHtml.close();
		} else {
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("��ȡrecvHtml.txtʧ��"));
			return false;
		}
		//�����滻��Ľű�
		QString strReplaceInitNull;
		QString strReplaceInit;
		QString strReplaceNew;
		QString strReplaceRecvHtml;

		for (int i = 0; i < employeesList.length(); i++) {
			//�༭�滻��Ŀ�ֵ
			QString strInitNull = strSourceInitNull;
			strInitNull.replace("external", QString("external_%1")
				.arg(employeesList.at(i)));
			strReplaceInitNull += strInitNull;
			strReplaceInitNull += "\n";

			//�༭�滻��ĳ�ʼֵ
			QString strInit = strSourceInit;
			strInit.replace("external", QString("external_%1")
				.arg(employeesList.at(i)));

			strReplaceInit += strInit;
			strReplaceInit += "\n";

			//�༭�滻���newWebChannel
			QString strNew = strSourceNew;
			strNew.replace("external", QString("external_%1")
				.arg(employeesList.at(i)));
			strReplaceNew += strNew;
			strReplaceNew += "\n";
			
			//�༭�滻���recvHtml
			QString strRecvHtml = strSourceRecvHtml;
			strRecvHtml.replace("external", QString("external_%1")
				.arg(employeesList.at(i)));
			strRecvHtml.replace("recvHtml",QString("recvHtml_%1")
				.arg(employeesList.at(i))); 
			strReplaceRecvHtml += strRecvHtml;
			strReplaceRecvHtml += "\n";
		}
		strFile.replace(strSourceInitNull,strReplaceInitNull);
		strFile.replace(strSourceInit, strReplaceInit);
		strFile.replace(strSourceNew, strReplaceNew);
		strFile.replace(strSourceRecvHtml,strReplaceRecvHtml);

		QTextStream stream(&fileWrite);
		stream << strFile;
		fileWrite.close();

		return  true;
	} else {
		QMessageBox::information(this,
			QString::fromLocal8Bit("��ʾ"),
			QString::fromLocal8Bit("дrecvHtml.jsʧ��"));
		return false;
	}
}

void TalkWindowShell::handleReceiveMsg(int senderEmployeeID, int msgType, QString strMsg) {
	QMsgTextEdit msgTextEdit;
	msgTextEdit.setText(strMsg);
	
	if(msgType == 1){//������Ϣ
		msgTextEdit.document()->toHtml();
	} else if (msgType == 0){//������Ϣ
		const int emotionWidth = 3;
		int emotionNum = strMsg.length() / emotionWidth;

		for (int i = 0; i < emotionWidth; i++) {
			msgTextEdit.addEmotionUrl(strMsg.mid(i*3,emotionWidth).toInt());
		}
	}


	//����Сbug����1.������Ϣʱ��Ϊ����ԭ����ܷ���ʧ��
	//2.���ͱ���ʱС��3���и��ӵı��飬ò�ơ�


	QString html = msgTextEdit.document()->toHtml();

	//�ı�html���û���������������
	if (!html.contains(".png") && !html.contains("</span>")) {
		QString fontHtml;
		QFile file(":/Resources/MainWindow/MsgHtml/msgFont.txt");
		if (file.open(QIODevice::ReadOnly)) {
			fontHtml = file.readAll();
			fontHtml.replace("%1", strMsg);
			file.close();
		} else {
			QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("�ļ� ,msgFont.txt�����ڣ�"));
			return;
		}
		if (!html.contains(fontHtml)) {
			html.replace(strMsg, fontHtml);
		}
	}

	TalkWindow* talkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	talkWindow->ui.msgWidget->appendMsg(html,QString::number(senderEmployeeID));
}

void TalkWindowShell::onEmotionBtnClicked(bool)
{
	m_emotionWindow->setVisible(!m_emotionWindow->isVisible());
	QPoint emotionPoint = this->mapToGlobal(QPoint(0, 0));//����ǰ�ؼ������λ��ת��Ϊ��Ļ�ľ���λ��

	emotionPoint.setX(emotionPoint.x() + 170);
	emotionPoint.setY(emotionPoint.y() + 220);
	m_emotionWindow->move(emotionPoint);
}


void TalkWindowShell::updateSendTcpMsg(QString& strData, 
	int &msgType, QString fileName /*= " "*/) {

	//��ȡ��ǰ����촰��
	TalkWindow* curTalkWindow =dynamic_cast<TalkWindow*>
		(ui.rightStackedWidget->currentWidget());
	QString talkId = curTalkWindow->getTalkId();

	QString strGroupFlag;
	QString strSend;
	if (talkId.length() == 4) { //ȺQQ�ĳ���
		strGroupFlag = "1";
	} else {
		strGroupFlag = "0";
	}

	int nstrDataLength = strData.length();//��������ݳ���
	int dataLength = QString::number(nstrDataLength).length();//��������ȵĳ���(��λ��)
	//const int nstrDataLength = dataLength;
	QString strDataLength;

	if (msgType == 1) {//�����ı���Ϣ
		//�ı���Ϣ�ĳ���Լ��Ϊ5λ
		if (dataLength == 1) {
			strDataLength = "0000" + QString::number(nstrDataLength);
		} else if (dataLength == 2) {
			strDataLength = "000" + QString::number(nstrDataLength);
		} else if (dataLength == 3) {
			strDataLength = "00" + QString::number(nstrDataLength);
		} else if (dataLength == 4) {
			strDataLength = "0" + QString::number(nstrDataLength);
		} else if (dataLength == 5) {
			strDataLength = QString::number(nstrDataLength);
		} else {
			QMessageBox::information(this,
				QString::fromLocal8Bit("��ʾ"),
				QString::fromLocal8Bit("����������ݳ���!"));
		}
		strSend = strGroupFlag + gLoginEmployeeID + talkId +
			"1" + strDataLength + strData;
	} else if (msgType == 0) {//������Ϣ
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "0" + strData;
	} else if (msgType == 2) {//�ļ���Ϣ
		QByteArray bt = strData.toUtf8();//��ȡ���ֽ�����
		QString strLength = QString::number(bt.length());
		strSend = strGroupFlag + gLoginEmployeeID + talkId + "2" +
			strLength + "bytes" + fileName + "data_begin" + strData;
	}
	QByteArray dataBt;
	dataBt.resize(strSend.length());
	dataBt = strSend.toUtf8();
	m_tcpClientSocket->write(dataBt);
}

void TalkWindowShell::onTalkWindowItemClicked(QListWidgetItem* item)
{
	QWidget* talkwindowWidget = m_talkWindowItemMap.find(item).value();
	ui.rightStackedWidget->setCurrentWidget(talkwindowWidget);
}

void TalkWindowShell::onEmotionItemClicked(int emotionNum)
{
	TalkWindow* currTalkWindow = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (currTalkWindow) {
		currTalkWindow->addEmotionImage(emotionNum);
	}
}

/*
���ݰ���ʽ:
�ı����ݰ���ʽ: Ⱥ�ı�־ + ������Ա��QQ�� + ����ϢԱ��QQ��(ȺQQ��) + ��Ϣ����1 + ���ݳ���+ ����
�������ݰ���ʽ: Ⱥ�ı�־ + ������Ա��QQ�� + ����ϢԱ��QQ��(ȺQQ��) + ��Ϣ����0 + ������� +Images��������
xxx������������

����:

1100042000100005hello
1100012001100005hello
1100012001100005hello
1100042000100002���

��ʾQQ10001��Ⱥ2001�����ı���Ϣ��������5������Ϊhellow

Ⱥ�ı�־ ������ ����Ⱥ �������� ���� ��������
1		 10001	 2001    1      00005 hello

*/
void TalkWindowShell::processPendingData() {
	//�˿����Ƿ���δ���������
	qDebug() <<QString::fromLocal8Bit("��δ�����������") << endl;
	while (m_udpReceiver->hasPendingDatagrams()) {
		const static int groupFlagWidth = 1;//Ⱥ�ı�־ռλ
		const static int groupWidth = 4;	//ȺQQ�ſ��
		const static int employeeWidth = 5;//Ա��QQ�ſ��
		const static int msgTypeWidth = 1;//��Ϣ���Ϳ��
		const static int msgLengthWidth = 5;//�ı���Ϣ���ȵĿ��
		const static int pictureWidth = 3;//����ͼƬ��Ϣ�Ŀ��


		//��ȡudp����
		QByteArray btData;
		btData.resize(m_udpReceiver->pendingDatagramSize());
		m_udpReceiver->readDatagram(btData.data(), btData.size());


		QString strData = btData.data();
		qDebug() << QString::fromLocal8Bit("�յ�����:")<<strData << endl;
		QString strWindowID;//���촰��ID,Ⱥ������Ⱥ�ţ���������Ա��QQ��
		QString strSendEmployeeID, strReceiveEmployeeID;//���ͼ����ն˵�QQ��
		
		QString strMsg;//����
		int msgLen;//���ݳ���
		int msgType;//��������

		//λ��,����
		strSendEmployeeID = strData.mid(groupFlagWidth,employeeWidth);//�����ߵ�QQ��
		
		//�Լ�������Ϣ��������
		if (strSendEmployeeID == gLoginEmployeeID) {
			return;
		}

		//�ж�Ⱥ�ı�־λ
		if (btData[0] == '1') {//Ⱥ��
			//ȺQQ��
			strWindowID = strData.mid(groupFlagWidth + employeeWidth, groupWidth);
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + groupWidth];

			if (cMsgType== '1') {//�ı���Ϣ
				msgType = 1;
				msgLen = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth
					,msgLengthWidth).toInt();//��Ϣ����
				strMsg = strData.mid(groupFlagWidth + employeeWidth + groupWidth + msgTypeWidth
					+  msgLengthWidth,msgLen);//��Ϣ
				qDebug() << msgLen << endl;

			} else if (cMsgType == '0') {//������Ϣ
				msgType = 0;
				int posImages = strData.indexOf("images");
				strMsg = strData.right(strData.length()-posImages-QString("images").length());

			}else if(cMsgType == '2'){//�ļ���Ϣ
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int posData_begin = strData.indexOf("data_begin");
				//�ļ�����
				QString fileName = strData.mid(posBytes + bytesWidth,
					posData_begin-posBytes-bytesWidth);
				gfileName = fileName;

				//�ļ�����
				int dataLengthWidth;
				int posData = posData_begin + QString("data_begin").length();
				strMsg = strData.mid(posData);
				gfileData = strMsg;

				//����employeeID��ȡ����������
				QString sender; 
				int employeeID = strSendEmployeeID.toInt();
				QSqlQuery querySenderName(QString("SELECT employee_name FROM tab_employees WHRER \
					employeeID = %1")
					.arg(employeeID));
				querySenderName.exec();

				if (querySenderName.first()) {
					sender = querySenderName.value(0).toString();
				}

			

			}
		} else {//˽��
			strReceiveEmployeeID = strData.mid(groupWidth + employeeWidth,employeeWidth);
			strWindowID = strSendEmployeeID;//������QQ��
			qDebug() << strWindowID;

			//���Ƿ����ҵ���Ϣ��������
			if (strReceiveEmployeeID != gLoginEmployeeID) {
				return;
			}


			//��ȡ��Ϣ����
			QChar cMsgType = btData[groupFlagWidth + employeeWidth + employeeWidth];
			if(cMsgType == '1'){//�ı���Ϣ
				msgType = 1;

				//�ı���Ϣ����
				msgLen = strData.mid(groupFlagWidth + employeeWidth + employeeWidth +
					msgTypeWidth,msgLengthWidth).toInt();

				//�ı���Ϣ
				strMsg = strData.mid(groupFlagWidth + employeeWidth + employeeWidth +
					msgTypeWidth + msgLengthWidth,msgLen);  
			} else if (cMsgType == '0') {//������Ϣ   
				msgType = 0;
				int posImages = strData.indexOf("images");
				int imagesWidth = QString("images").length();
				strMsg = strData.mid(posImages + imagesWidth);

			} else if (cMsgType == '2') {//�ļ���Ϣ
				msgType = 2;
				int bytesWidth = QString("bytes").length();
				int posBytes = strData.indexOf("bytes");
				int data_beginWidth = QString("data_begin").length();
				int posData_begin = strData.indexOf("data_begin");

				//�ļ�����
				QString fileName = strData.mid(posBytes + bytesWidth,posData_begin - posBytes-
				bytesWidth);
				gfileName = fileName;

				//�ı�����
				strMsg = strData.mid(posData_begin + data_beginWidth);
				gfileData = strMsg;
			}
		}
		//�����촰����Ϊ��Ĵ���
		QWidget* widget = WindowManager::getInstance()->findWindowName(strWindowID);
		if (widget) {//����
			this->setCurrentWidget(widget);

			//ͬ��������촰���б�
			QListWidgetItem* item = m_talkWindowItemMap.key(widget);
			item->setSelected(true);
		}else{//���촰��δ��
			return;
		}
		
		//�ļ���Ϣ��������
		if (msgType != 2){
			int sendEmployeeID = strSendEmployeeID.toInt();
			qDebug() << QString::fromLocal8Bit("��������������") << strMsg << endl;
			handleReceiveMsg(sendEmployeeID, msgType, strMsg);
			
		}

	}
}
