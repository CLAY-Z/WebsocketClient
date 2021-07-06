#pragma once

#include <QtWidgets/QDialog>
#include <QtWebSockets/qwebsocket.h>
#include <QtWebSockets/qwebsocketprotocol.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

#include <QFile>
#include <QTimer>

namespace Ui {
	class WebsocketClientClass;
}
class WebsocketClient : public QDialog
{
	Q_OBJECT

public:
	WebsocketClient(QWidget *parent = Q_NULLPTR);
	~WebsocketClient();
	QByteArray strtojson(QString cmd);

Q_SIGNALS:
	private Q_SLOTS:
	void connectToServer();
	void closeConnection();
	void onTextMessageReceived(const QString& message);

	public slots:
	void stopClicked();
	void onconnected();
	void onSendButtonClicked();
	void onCleanButtonClicked();
	void onReadButtonClicked();

	void onFrontButtonPressed();
	void onFrontButtonReleased();
	void onBackButtonPressed();
	void onBackButtonReleased();

	void sendctrlmessage();

private:
	Ui::WebsocketClientClass *ui;
	QWebSocket m_websocket;
	QUrl m_url;
	QDateTime* current_date_time;
	uint32_t timeStamp;
	QString file_path = "C:/Users/zhaoxiaohe/Desktop/test.json";

	bool move_flag;
	QString move_act;
	QTimer *ctrl_timer;
	uint8_t CardId = 1;
};
