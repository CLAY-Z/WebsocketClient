#include "WebsocketClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WebsocketClient w;
	w.show();
	return a.exec();
}
