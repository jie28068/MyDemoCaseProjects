#ifndef ARCHIVEINFO_H
#define ARCHIVEINFO_H

#include <QWidget>
#include <QVariantMap>
#include <QLineEdit>
#include "Base/mapobject.h"
#include "Model/mttablemodel.h"
#include "QDataWidgetMapper"
#include "computationtime.h"
#include <Base/gettexttel.h>
#include <HRMTool/uploadresume.h>
namespace Ui {
class ArchiveInfo;
}
class QComboBox;
class ArchiveInfo : public QWidget, public MapObject
{
    Q_OBJECT
    Q_PROPERTY(OptFlag optFlag READ optFlag WRITE setOptFlag NOTIFY optFlagChanged)
//    Q_PROPERTY(QString optID READ optID WRITE setOptID)
//    Q_PROPERTY(QString sign READ sign WRITE setSign)
//    Q_PROPERTY(QString archiveFile READ archiveFile WRITE setArchiveFile)
//    Q_PROPERTY(QString name READ name WRITE setName)//姓名
//    Q_PROPERTY(QString birthday READ birthday WRITE setBirthday)//生日
//    Q_PROPERTY(QString tel_no READ tel_no WRITE setTel_no)//电话
//    Q_PROPERTY(QString post READ post WRITE setPost)//岗位
//    Q_PROPERTY(int level READ level WRITE setLevel)//级别
//    Q_PROPERTY(QString type READ type WRITE setType)//档案类型
//    Q_PROPERTY(QString src READ src WRITE setSrc)//档案来源
//    Q_PROPERTY(QString sign READ sign WRITE setSign)//档案签名

public:
    explicit ArchiveInfo(QWidget *parent = nullptr);
    ~ArchiveInfo();
    enum OptMode{
        OPT_COMMIT=1,
        OPT_UPDATE=2
    };
//    Q_ENUM(OptMode)
    Q_DECLARE_FLAGS(OptFlag, OptMode)
    Q_FLAG(OptFlag)

    void propertyInit(QString sign);
    void setModel(MTTableModel &archiveModel);
    void setData(QList<QVariantMap> listData);
    void setDate(QString birthday);
    void setAge(QString birthday);
    QVariantMap getArchiveInfo();

//    const QString &name() const;
//    void setName(const QString &newName);

//    const QString &birthday() const;
//    void setBirthday(const QString &newBirthday);

//    const QString &tel_no() const;
//    void setTel_no(const QString &newTel_no);

//    const QString &post() const;
//    void setPost(const QString &newPost);

//    int level() const;
//    void setLevel(int newLevel);

//    const QString &type() const;
//    void setType(const QString &newType);

//    const QString &src() const;
//    void setSrc(const QString &newSrc);

//    const QString &sign() const;
//    void setSign(const QString &newSign);


    const ArchiveInfo::OptFlag &optFlag() const;
    void setOptFlag(const OptFlag &newOptFlag);

public slots:
    void setCurIndex(int);
    void recvResult(bool success, QString msg);

private slots:
    void on_btn_archive_update_clicked();

    void on_btn_archive_commit_clicked();

    void on_src_clicked();

    void on_level_currentIndexChanged(int index);

    void on_birthday_editingFinished();

signals:
    void archiveInfoUpdate(QVariantMap info);
    void archiveInfoCommit(QVariantMap info);

    void optFlagChanged();

private:
    Ui::ArchiveInfo *ui;
    QDataWidgetMapper dataMapper;
    MTTableModel model;
    QStringList headerList;
    QComboBox *level;
    QString getEditText(QLineEdit* edit);
//    QString m_name;
//    QString m_birthday;
//    QString m_tel_no;
//    QString m_post;
//    int m_level;
//    QString m_type;
//    QString m_src;
    //    QString m_sign;
    OptFlag m_optFlag;
};

#endif // ARCHIVEINFO_H
