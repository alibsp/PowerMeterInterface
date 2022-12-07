#include "Tools.h"



Tools::Tools()
{

}

QString Tools::jalaliToday(const char spliter)
{
    QCalendar calendar(QCalendar::System::Jalali);
    QDate today = QDate::currentDate();

    QCalendar::YearMonthDay ymd = calendar.partsFromDate(today);
    return QString("%1%2%3%4%5")
            .arg(ymd.year, 4, 10, QChar('0'))
            .arg(spliter)
            .arg(ymd.month, 2, 10, QChar('0'))
            .arg(spliter)
            .arg(ymd.day, 2, 10, QChar('0'));

}

QString Tools::jalaliNow(const char dateSpliter,const  char timeSpliter)
{
    QTime now = QTime::currentTime();
    return jalaliToday(dateSpliter)+" "+QString("%1%2%3%4%5")
                                     .arg(now.hour(), 2, 10, QChar('0'))
                                     .arg(timeSpliter)
                                     .arg(now.minute(), 2, 10, QChar('0'))
                                     .arg(timeSpliter)
                                     .arg(now.second(), 2, 10, QChar('0'));
}

