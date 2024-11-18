#include "settings.h"

Settings::Settings()
{

}

void Settings::setValue(QString key, QVariant value)
{
    QSettings setting;
    setting.setValue(key, value);
}

QVariant Settings::getValue(QString key, QVariant defaluteV)
{
    QSettings setting;
    return setting.value(key,defaluteV);
}
