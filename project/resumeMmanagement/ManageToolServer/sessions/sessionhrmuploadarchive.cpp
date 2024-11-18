#include "sessionhrmuploadarchive.h"
#include "dbmanager.h"
#include <QVariant>
#include <QDateTime>
#include <QFile>
#include "Base/PathTool.h"
#include "sessionhrmuploadresume.h"
#include "Base/regularexpression.h"
SessionHrmUploadArchive::SessionHrmUploadArchive(AbsMsgHandle *handle, QObject *parent) : AbsSession(handle, parent)
{

}

/**
 * @brief SessionHrmUploadArchive::sessionRun   档案入库
 * 参数：
    * name：姓名 默认**
    * birthday：生日 默认********
    * tel_no：电话号码 默认***********
    * post：岗位[QT][C++][产品经理]
    * level：级别[未定：0][初级：10][中级：20][高级：30]
    * format：格式[pdf][doc][docx]
    * type：档案类型
    * src：档案来源（用户ID）
    * mark：档案标记 默认无标记
    * sign：档案签名，对应的简历文件MD5
    * timestamp：变更时间
    * opt_id：操作人
    * experience 经历段
 * 返回：无
 *
 */
void SessionHrmUploadArchive::sessionRun()
{
    if(srcParams["src"].toString().isEmpty() || srcParams["src"].toString() == "--")//默认简历来源为操作人
        srcParams["src"] = msgHandle->userID();
    else{
        int id = DBManager::instance().queryUserID(QString("name='%1'").arg(srcParams["src"].toString()));
        if(id==0){//不存在就注册
                QVariantMap params;
                params["name"] = srcParams["src"];
                params["type"] = srcParams["srctype"];
                params["email"] = QDateTime::currentMSecsSinceEpoch();
                params["tel_no"] = QDateTime::currentMSecsSinceEpoch();
                params["passwd"] = QDateTime::currentMSecsSinceEpoch();
                params["department"] = "无";
                if(DBManager::instance().newUserInfo(params)){
                    id = DBManager::instance().queryUserID(QString("name='%1'").arg(srcParams["src"].toString()));
                }
        }
        srcParams["src"] = id;

    }
    srcParams.remove("srctype");
    srcParams["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    srcParams["opt_id"] = msgHandle->userID();

//    QVariantMap ArchiveText;
//    ArchiveText.insert("sign",srcParams["sign"]);
//    QString sign=srcParams["sign"].toString();
//    QString format=srcParams["format"].toString();
//    QString Path=PathTool::cachePath()+"/"+sign+"."+format;
//    QString Text=GetTextfromPdforWord(Path,format);
//    ArchiveText.insert("text_entirety",Text);                               //获取要入库档案的 文本

    //新加 名字 生日 经历段 用来查重比对
//    QString birthday = srcParams["birthday"].toString();
//    QStringList experience = srcParams["experience"].toStringList();
//    RegularExpression *Regular=new RegularExpression;
//    QStringList list= Regular->GetRegularTimeList(Text);

//    birthday=Regular->GetTextBirthday(list);                                //这个用来比对的生日 优先使用从文本中获取的生日 如果无 使用客户端传来的
//    if(birthday==NULL)
//    {
//        birthday=srcParams["birthday"].toString();
//        birthday.replace("/","-");
//    }
//    experience=Regular->GetTextExperience(list);
//    ArchiveText.insert("name",srcParams["name"]);                           //名字暂时从客户端输入获取
//    ArchiveText.insert("birthday",birthday);                                //生日
//    ArchiveText.insert("experience",experience.join(","));                  //经历段

//    delete Regular;
//    srcParams.remove("experience");
    srcParams.remove("levelname");
    srcParams.remove("age");                                      //ArchiveInfo数据库中无该字段，仅用来传递数据，入库前删除
    QStringList experience=srcParams["experience"].toStringList();
    srcParams["experience"]=experience.join(",");                         //把经历段以,分隔符来分割成字符串存入数据库
    bool rst = DBManager::instance().newArchiveInfo(srcParams);
    if(rst){
        createResponse(true, "档案入库成功");
 //       DBManager::instance().newArchiveText(ArchiveText);       //将文本存入数据库
        archiveFileConfirm(srcParams["sign"].toString());
        DBManager::instance().insertOneWorkLog(QString::number(msgHandle->userID()),QString("档案入库:%1").arg(srcParams["name"].toString()));
    }
    else
        createResponse(false, "档案入库失败,已入库档案无需再次入库");
}

//简历文件确认，将简历从缓存目录移动到档案目录
void SessionHrmUploadArchive::archiveFileConfirm(QString sign)
{
    QString oldFile = PathTool::findArchiveFile(sign, PathTool::cachePath());
    QFileInfo info(oldFile);
    QString newFile = PathTool::archivePath()+"/"+sign+"."+info.suffix();
    qDebug()<<"archiveFileConfirm"<<oldFile<<newFile;
    qDebug()<<QFile::rename(oldFile, newFile);
}
