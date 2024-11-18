#ifndef ARCHIVEMODEL_H
#define ARCHIVEMODEL_H

#include <QVariantMap>
#include "absmtmodel.h"
#include "datamap.h"

class ArchiveModel : public AbsMTModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString birthday READ birthday WRITE setBirthday)
    Q_PROPERTY(QString tel_no READ tel_no WRITE setTel_no)
    Q_PROPERTY(QString post READ post WRITE setpost )
    Q_PROPERTY(int level READ level WRITE setLevel )
    Q_PROPERTY(QString type READ type WRITE setType )
    Q_PROPERTY(int src READ src WRITE setSrc)
    Q_PROPERTY(QString mark READ mark WRITE setMark)
    Q_PROPERTY(QString sign READ sign WRITE setSign )
    Q_PROPERTY(qint64 timestamp READ timestamp WRITE setTimestamp)
    Q_PROPERTY(int opt_id READ opt_id WRITE setOpt_id)
public:
    ArchiveModel();

    const QString &name() const;
    void setName(const QString &newName);

    const QString &birthday() const;
    void setBirthday(const QString &newBirthday);

    const QString &tel_no() const;
    void setTel_no(const QString &newTel_no);

    const QString &post() const;
    void setpost(const QString &newPost);

    int level() const;
    void setLevel(int newLevel);

    const QString &type() const;
    void setType(const QString &newType);

    int src() const;
    void setSrc(int newSrc);

    const QString &mark() const;
    void setMark(const QString &newMark);

    const QString &sign() const;
    void setSign(const QString &newSign);

    qint64 timestamp() const;
    void setTimestamp(qint64 newTimestamp);

    int opt_id() const;
    void setOpt_id(int newOpt_id);

private:
    QString m_name;
    QString m_birthday;
    QString m_tel_no;
    QString m_post;
    int m_level;
    QString m_type;
    int m_src;
    QString m_mark;
    QString m_sign;
    qint64 m_timestamp;
    int m_opt_id;
};

#endif // ARCHIVEMODEL_H
