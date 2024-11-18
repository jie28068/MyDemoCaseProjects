#include "archivemodel.h"

ArchiveModel::ArchiveModel()
{

}

const QString &ArchiveModel::name() const
{
    return m_name;
}

void ArchiveModel::setName(const QString &newName)
{
    m_name = newName;
}

const QString &ArchiveModel::birthday() const
{
    return m_birthday;
}

void ArchiveModel::setBirthday(const QString &newBirthday)
{
    m_birthday = newBirthday;
}

const QString &ArchiveModel::tel_no() const
{
    return m_tel_no;
}

void ArchiveModel::setTel_no(const QString &newTel_no)
{
    m_tel_no = newTel_no;
}

const QString &ArchiveModel::post() const
{
    return m_post;
}

void ArchiveModel::setpost(const QString &newPost)
{
    m_post = newPost;
}

int ArchiveModel::level() const
{
    return m_level;
}

void ArchiveModel::setLevel(int newLevel)
{
    m_level = newLevel;
}

const QString &ArchiveModel::type() const
{
    return m_type;
}

void ArchiveModel::setType(const QString &newType)
{
    m_type = newType;
}

int ArchiveModel::src() const
{
    return m_src;
}

void ArchiveModel::setSrc(int newSrc)
{
    m_src = newSrc;
}

const QString &ArchiveModel::mark() const
{
    return m_mark;
}

void ArchiveModel::setMark(const QString &newMark)
{
    m_mark = newMark;
}

const QString &ArchiveModel::sign() const
{
    return m_sign;
}

void ArchiveModel::setSign(const QString &newSign)
{
    m_sign = newSign;
}

qint64 ArchiveModel::timestamp() const
{
    return m_timestamp;
}

void ArchiveModel::setTimestamp(qint64 newTimestamp)
{
    m_timestamp = newTimestamp;
}

int ArchiveModel::opt_id() const
{
    return m_opt_id;
}

void ArchiveModel::setOpt_id(int newOpt_id)
{
    m_opt_id = newOpt_id;
}
