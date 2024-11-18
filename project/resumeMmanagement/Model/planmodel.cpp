#include "planmodel.h"

PlanModel::PlanModel()
{

}

const QString &PlanModel::type() const
{
    return m_type;
}

void PlanModel::setType(const QString &newType)
{
    m_type = newType;
}

const QString &PlanModel::create_time() const
{
    return m_create_time;
}

void PlanModel::setCreate_time(const QString &newCreate_time)
{
    m_create_time = newCreate_time;
}

const QString &PlanModel::node_time() const
{
    return m_node_time;
}

void PlanModel::setNode_time(const QString &newNode_time)
{
    m_node_time = newNode_time;
}

const QString &PlanModel::finish_time() const
{
    return m_finish_time;
}

void PlanModel::setFinish_time(const QString &newFinish_time)
{
    m_finish_time = newFinish_time;
}

int PlanModel::related_id() const
{
    return m_related_id;
}

void PlanModel::setRelated_id(int newRelated_id)
{
    m_related_id = newRelated_id;
}

const QString &PlanModel::detail() const
{
    return m_detail;
}

void PlanModel::setDetail(const QString &newDetail)
{
    m_detail = newDetail;
}

const QString &PlanModel::note() const
{
    return m_note;
}

void PlanModel::setNote(const QString &newNote)
{
    m_note = newNote;
}

int PlanModel::role_create() const
{
    return m_role_create;
}

void PlanModel::setRole_create(int newRole_create)
{
    m_role_create = newRole_create;
}

int PlanModel::role_targets() const
{
    return m_role_targets;
}

void PlanModel::setRole_targets(int newRole_targets)
{
    m_role_targets = newRole_targets;
}

int PlanModel::role_complete() const
{
    return m_role_complete;
}

void PlanModel::setRole_complete(int newRole_complete)
{
    m_role_complete = newRole_complete;
}
