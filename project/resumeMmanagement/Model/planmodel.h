#ifndef PLANMODEL_H
#define PLANMODEL_H

#include "absmtmodel.h"
#include "datamap.h"

class PlanModel : public AbsMTModel
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType)
    Q_PROPERTY(QString create_time READ create_time WRITE setCreate_time)
    Q_PROPERTY(QString node_time READ node_time WRITE setNode_time)
    Q_PROPERTY(QString finish_time READ finish_time WRITE setFinish_time )
    Q_PROPERTY(int related_id READ related_id WRITE setRelated_id)
    Q_PROPERTY(QString detail READ detail WRITE setDetail)
    Q_PROPERTY(QString note READ note WRITE setNote)
    Q_PROPERTY(int role_create READ role_create WRITE setRole_create)
    Q_PROPERTY(int role_targets READ role_targets WRITE setRole_targets )
    Q_PROPERTY(int role_complete READ role_complete WRITE setRole_complete )

private:
    QString m_type;

    QString m_create_time;

    QString m_node_time;

    QString m_finish_time;

    int m_related_id;

    QString m_detail;

    QString m_note;

    int m_role_create;

    int m_role_targets;

    int m_role_complete;

public:
    PlanModel();

    const QString &type() const;
    void setType(const QString &newType);
    const QString &create_time() const;
    void setCreate_time(const QString &newCreate_time);
    const QString &node_time() const;
    void setNode_time(const QString &newNode_time);
    const QString &finish_time() const;
    void setFinish_time(const QString &newFinish_time);
    int related_id() const;
    void setRelated_id(int newRelated_id);
    const QString &detail() const;
    void setDetail(const QString &newDetail);
    const QString &note() const;
    void setNote(const QString &newNote);
    int role_create() const;
    void setRole_create(int newRole_create);
    int role_targets() const;
    void setRole_targets(int newRole_targets);
    int role_complete() const;
    void setRole_complete(int newRole_complete);
};

#endif // PLANMODEL_H
