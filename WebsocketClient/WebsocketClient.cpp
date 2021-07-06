#include "WebsocketClient.h"
#include "ui_WebsocketClient.h"

WebsocketClient::WebsocketClient(QWidget *parent)
	: QDialog(parent),
	ui(new Ui::WebsocketClientClass)
{
	ui->setupUi(this);
	ctrl_timer = new QTimer(this);

	ui->iplineedit->setText("192.168.1.20");
	ui->portspinbox->setValue(9002);

	connect(ui->linkbutton, SIGNAL(clicked(bool)), this, SLOT(connectToServer()));
	connect(ui->disconnectbutton, SIGNAL(clicked(bool)), this, SLOT(stopClicked()));
	connect(ui->sendbutton, SIGNAL(clicked(bool)), this, SLOT(onSendButtonClicked()));
	connect(ui->cleanbutton, SIGNAL(clicked(bool)), this, SLOT(onCleanButtonClicked()));
	connect(ui->readbutton, SIGNAL(clicked(bool)), this, SLOT(onReadButtonClicked()));

	connect(ui->frontbutton, SIGNAL(pressed()), this, SLOT(onFrontButtonPressed()));
	connect(ui->backbutton, SIGNAL(pressed()), this, SLOT(onBackButtonPressed()));
	connect(ui->frontbutton, SIGNAL(released()), this, SLOT(onFrontButtonReleased()));
	connect(ui->backbutton, SIGNAL(released()), this, SLOT(onBackButtonReleased()));

	connect(&m_websocket, SIGNAL(connected()), this, SLOT(onconnected()));
	connect(&m_websocket, SIGNAL(disconnected()), this, SLOT(closeConnection()));
	connect(&m_websocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));

	connect(ctrl_timer, SIGNAL(timeout()), this, SLOT(sendctrlmessage()));

	ctrl_timer->start(1000);
}

WebsocketClient::~WebsocketClient() {
	m_websocket.errorString();
	m_websocket.close();
	ctrl_timer->stop();
	delete ui;
}

//连接服务器
void WebsocketClient::connectToServer() {
	QString path = QString("ws://%1:%2").arg(ui->iplineedit->text()).arg(ui->portspinbox->text());
	m_url = QUrl(path);
	m_websocket.open(m_url);
}

//断开服务器
void WebsocketClient::stopClicked() {
	m_websocket.close();
}

//连接上之后
void WebsocketClient::onconnected() {
	qDebug() << "hello websocket";
	ui->statuslabel->setText("Connected Succesufully");
	ui->linkbutton->setEnabled(false);
	ui->disconnectbutton->setEnabled(true);
	ui->sendmessagetextedit->setEnabled(true);
	ui->sendbutton->setEnabled(true);
	ui->receivemessagetextedit->setEnabled(true);
	ui->cleanbutton->setEnabled(true);
}

//断开之后
void WebsocketClient::closeConnection() {
	ui->statuslabel->setText("Disconnected");
	ui->linkbutton->setEnabled(true);
	ui->disconnectbutton->setEnabled(false);
	ui->sendmessagetextedit->setEnabled(false);
	ui->sendbutton->setEnabled(false);
	ui->receivemessagetextedit->setEnabled(true);
	ui->cleanbutton->setEnabled(false);
}

//收到消息
void WebsocketClient::onTextMessageReceived(const QString &message) {
	QString time = current_date_time->currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
	ui->receivemessagetextedit->append(time + '\n' + message);
}

//发送消息
void WebsocketClient::onSendButtonClicked() {
	QString msg = ui->sendmessagetextedit->toPlainText();
	//msg: "{\"version\":1.0,\n\"type\":0,\n\"fields\":{\"db_name\":zhaoxiaohe,\n\"table_name\":shuaige,\n\"field_name\": }}"
	qDebug() << "send data:" + msg;
	m_websocket.sendTextMessage(msg);
}

//清除内容
void WebsocketClient::onCleanButtonClicked() {
	ui->receivemessagetextedit->clear();
	ui->sendmessagetextedit->clear();
}

//构建json
QByteArray WebsocketClient::strtojson(QString act) {
	timeStamp = QDateTime::currentDateTime().toTime_t();
	QString str = QString("{\"type\":\"set_move_act\", \"id\":%1,\"ts\":%2,\"cmd\":\"%3\"}")
		.arg(CardId).arg(timeStamp).arg(act);
	//qDebug()<<str;
	return str.toUtf8();
}

//读取json
void WebsocketClient::onReadButtonClicked() {
	QFile file(file_path);
	if (!file.open(QIODevice::ReadOnly)) {
		ui->sendmessagetextedit->append("文件打开失败");
		return;
	}

	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
	if (parseError.error != QJsonParseError::NoError) {
		ui->sendmessagetextedit->append("文件读取失败");
		ui->sendmessagetextedit->append(parseError.errorString());
		return;
	}

	if (document.isObject()) {
		QJsonObject object = document.object();
		if (object.contains("version")) {
			QJsonValue version = object.value("version");
			ui->sendmessagetextedit->append("{\"version\":" + version.toString() + ",");
		}
		
		if (object.contains("type")) {
			QJsonValue type = object.value("type");
			ui->sendmessagetextedit->append("\"type\":" + QString::number(type.toInt()) + ",");
		}

		if (object.contains("fields")) {
			QJsonValue fields = object.value("fields");
			QJsonObject field = fields.toObject();

			QJsonValue db_name = field.value("db_name");
			QJsonValue table_name = field.value("table_name");
			QJsonValue field_name = field.value("field_name");

			ui->sendmessagetextedit->append("\"fields\":{\"db_name\":" + db_name.toString() + ",");
			ui->sendmessagetextedit->append("\"table_name\":" + table_name.toString() + ",");
			ui->sendmessagetextedit->append("\"field_name\":" + field_name.toString() + "},");
		}

		if (object.contains("cmd")) {
			QJsonValue cmd = object.value("cmd");
			ui->sendmessagetextedit->append("\"cmd\":" + cmd.toString() + "}");
		}
	}

	file.close();
}

//前进
void WebsocketClient::onFrontButtonPressed() {
	move_flag = true;
	move_act = "move_front";
}

//停止
void WebsocketClient::onFrontButtonReleased() {
	move_flag = false;
	move_act = "move_stop";
	m_websocket.sendTextMessage(strtojson(move_act));
}

//前进
void WebsocketClient::onBackButtonPressed() {
	move_flag = true;
	move_act = "move_back";
}

//停止
void WebsocketClient::onBackButtonReleased() {
	move_flag = false;
	move_act = "move_stop";
	m_websocket.sendTextMessage(strtojson(move_act));
}

//定时发送数据至服务端
void WebsocketClient::sendctrlmessage() {
	if (move_flag == true) {
		//QString str = ui->sendmessagetextedit->toPlainText();
		m_websocket.sendTextMessage(strtojson(move_act));
	}
}