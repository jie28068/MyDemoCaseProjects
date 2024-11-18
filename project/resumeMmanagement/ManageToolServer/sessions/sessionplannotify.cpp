#include "sessionplannotify.h"

SessionPlanNotify::SessionPlanNotify(AbsMsgHandle* handle, QObject *parent) : AbsSession(handle, parent)
{

}

/**
 * @brief SessionPlanNotify::sessionRun
 */
void SessionPlanNotify::sessionRun()
{
    QVariantList plan_id_list = srcParams["plan_id_list"].toList();
    QVariantList user_id_list = srcParams["user_id_list"].toList();

}
