#include <QCoreApplication>
#include <QFile>
//#include "AsynqQueryModel.h"
//#include "ConnectionManager.h"

#include "MainWindow.h"
#include <QDebug>
#include <QApplication>
#include "DeviceLayer/DevicesCommunicationService.h"

using namespace Database;


void debugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    QByteArray txt = QString("").toLocal8Bit();
    // Switch structure left to be converted to write into the file in the future
    switch ( type )
    {
    case QtDebugMsg:
        //txt += QString("{Debug} \t\t %1").arg(msg);
        txt += msg;
        break;
    case QtWarningMsg:
        txt += QString("%1 {Warning:} \t %2").arg(Tools::jalaliNow()).arg(msg);
        break;
    case QtCriticalMsg:
        txt += QString("%1 {Critical:} \t %1").arg(Tools::jalaliNow()).arg(msg);
        break;
    case QtFatalMsg:
        txt += QString("%1 {Fatal:} \t %1").arg(Tools::jalaliNow()).arg(msg);
        abort(); // deliberately core dump
    }
    QFile outFile("Debug.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream textStream(&outFile);
    textStream << txt << endl;

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //qInstallMessageHandler(debugMessageOutput);

    /*ConnectionManager *databaseConnection = ConnectionManager::instance();

    databaseConnection->setType("QODBC");
    databaseConnection->setDatabaseName(QString("DRIVER={SQL Server Native Client 11.0};SERVER=lpc:.\\SQL2017;Database=EmginDB;Trusted_Connection=Yes;"));
    databaseConnection->setUserName("sa");
    databaseConnection->setPassword("123456");

    //if(databaseConnection->open())
    //    qDebug()<<"Database opened.";
    try
    {
        AsyncQuery *query = new AsyncQuery();
        QObject::connect (query, &AsyncQuery::execDone, [=](const AsyncQueryResult &result)
        {
            if (!result.isValid()) {
                qDebug() << "SqlError" << result.error().text();
            } else {
                int columns = result.headRecord().count();
                for (int row = 0; row < result.count(); row++) {
                    for (int col = 0; col < columns; col++) {
                        qDebug() << result.value(row, col).toString();
                    }
                }
            }
        });
        query->startExec("SELECT top 10 * FROM Command");
    } catch (int ab)
    {

    }*/
    MainWindow w;
    w.show();

    return a.exec();
}
