#ifndef TOOLS_H
#define TOOLS_H
#include <QString>
#include <QCalendar>
#include <QDateTime>
#include <QMetaEnum>

class Tools
{
public:
    Tools();
    static QString jalaliToday(const char spliter='/');
    static QString jalaliNow(const char dateSpliter='/', const char timeSpliter=':');

    template<typename QEnum>
    static QString enumToString (const QEnum value)
    {
        return QString(QMetaEnum::fromType<QEnum>().valueToKey(value));
    }

};

#endif // TOOLS_H
