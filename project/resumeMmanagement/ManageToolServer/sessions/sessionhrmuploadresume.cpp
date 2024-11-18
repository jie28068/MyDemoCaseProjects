#include "sessionhrmuploadresume.h"
#include "dbmanager.h"
#include <QTimer>
#include <QTime>


#define EXPERIENCE_MATCH 2      //经历段进行比较时 匹配到的相同时间段个数超过这个值认为简历可能重复

SessionHrmUploadResume::SessionHrmUploadResume(AbsMsgHandle *handle, QObject *parent): AbsSession(handle, parent)
{
    connect(handle, &AbsMsgHandle::streamRst, this, &SessionHrmUploadResume::streamMsg);
}

/**
 * @brief SessionHrmUploadResume::sessionRun   简历上传请求
 * [sign]：文件签名
 * [size]：文件大小
 */
void SessionHrmUploadResume::sessionRun()
{
    QString sign = srcParams["sign"].toString();
    quint64 size = srcParams["size"].toULongLong();
    QString format=srcParams["format"].toString();
    QString birthday = srcParams["birthday"] .toString();
    QStringList experience = srcParams["experience"].toStringList();
    Uploadbirthday = birthday;
    Uploadexperience = experience;
//    UploadResumeformat=format;
//    UploadResumesign=sign;

    if(sign.isEmpty())
        createResponse(false, "文件签名为空");
    else if(size == 0 || size > 100000000)
        createResponse(false, "非法的文件大小");
    else{
        bool signCheckPass = DBManager::instance().archiveSignCheck(sign);
        if(signCheckPass){
            msgHandle->waitForStream(sign, format, size);
            createResponse(true, "简历上传请求成功,正在上传文件...");

            //compare test
//            QTimer::singleShot(5000, this, &SessionHrmUploadResume::resumeCompareTest);
        }
        else
            createResponse(false, "完全重复的简历文件");
    }
}

void SessionHrmUploadResume::resumeCompareTest()    //该函数无用
{
    QVariantList variantList = DBManager::instance().QueryResumeTextList();
    QVariantList retList;
    int i=0;
    responseParams.clear();

    foreach(QVariant resume, variantList)
    {
        QVariantMap map = resume.toMap();
        map["similarity"] = QString("%1%%").arg((i++)*10);
        retList<<map;
        responseParams["similarity_list"] = retList;
    }
    createResponse(false, "简历可能重复");
    sessionResponse();
}

void SessionHrmUploadResume::streamMsg(bool success, QString msg)
{
    if(success)
    {
        createResponse(success, "文件上传成功，正在查重...");
        sessionResponse();

        QTimer::singleShot(200, this, &SessionHrmUploadResume::contrastResume);
    }
    else
    {
        createResponse(success, msg);
        sessionResponse();
    }

}

void SessionHrmUploadResume::contrastResume()
{
    responseParams.clear();

    QVariantList templist;
    /**
     *  这里流程 从上传的文本中获取 姓、生日、工作经历时间信息，与数据库中做对比
     *  if有超过简历相似度门限的简历 则提示有相似简历
     *  else 无相似简历  可以入库简历    返回对应相似的数据库数据
    **/
    //上传简历路径
    //QString UploadResumePath=PathTool::cachePath()+"/"+UploadResumesign+"."+UploadResumeformat;
    //qDebug()<<"文件路径:"<<UploadResumePath;
    //获取上传简历的文本
    //QString UploadResumeText=GetTextfromPdforWord(UploadResumePath,UploadResumeformat);

    //提取文本中 姓名、生日、经历段等数据
//    RegularExpression *Regular=new RegularExpression;
//    QString name;        //暂时无法获取
//    QStringList list = Regular->GetRegularTimeList(UploadResumeText);
//    QString birthday = Regular->GetTextBirthday(list);
//    QStringList experience = Regular->GetTextExperience(list);

    QVariantList maplist=DBManager::instance().QueryResumeTextList();  //返回数据库ArchiveInfo列表
    qDebug()<<"Archive Number:"<<maplist.count();
    qDebug()<<"Uploadbirthday info:"<<Uploadbirthday;
    qDebug()<<"Uploadexperience info:"<<Uploadexperience;
    if(!Uploadbirthday.isEmpty())     //生日不为空，先进行生日初筛
    {
        qDebug()<<"生日不为空";
        QVariantList similarlist;
         for(int i=0;i<maplist.count();i++)
         {
               QVariantMap map = maplist.at(i).toMap();
               QString archivebirthday = map.value("birthday").toString();
               qDebug()<<"archivebirthday"<<i<<":"<<archivebirthday;
               if(compareBirthday(Uploadbirthday,archivebirthday))
               {
                  similarlist.append(map);              //生日相同 把map存入相似列表  用于后续再判断
                  qDebug()<<"匹配到生日相同的简历,将继续进行经历段比较";
               }
         }
         if(similarlist.isEmpty())
         {
             qDebug()<<"库中无简历生日与之相同,查重通过";
         }
         else  //有生日相同的数据 进行经历比较
         {
               for(int i=0;i<similarlist.count();i++)
               {
                   qDebug()<<"compare:";
                   QVariantMap map = similarlist.at(i).toMap();
                   QStringList archiveexperience = map.value("experience").toString().split(",");    //数据库中 experience字段 字符串之间通过","隔开
                   if(compareExperience(Uploadexperience,archiveexperience))    //经历有相同段
                   {
                       map.insert("Reason","生日匹配\n经历匹配");
                       templist.append(map);     //将相同的简历map追加至临时list，用于返回
                   }
               }
         }

    }
    else     //生日为空，直接比较经历
    {
        qDebug()<<"生日为空";
        for(int i=0;i<maplist.count();i++)
        {
            qDebug()<<"compare:";
            QVariantMap map = maplist.at(i).toMap();
            QStringList archiveexperience = map.value("experience").toString().split(",");
            if(compareExperience(Uploadexperience,archiveexperience))     //经历有相同段
            {
                map.insert("Reason","经历匹配");
                templist.append(map);
            }
        }

    }

    if(!templist.isEmpty())
        responseParams.insert("similarity_list",templist);

    if(responseParams.contains("similarity_list"))
        createResponse(false, "简历可能重复");
    else
        createResponse(true, "简历查重通过，可以编辑或入库");
    sessionResponse();
}

bool SessionHrmUploadResume::compareName(QString name, QString archivename)
{
    if((name.isEmpty())||(archivename.isEmpty()))
    {
        qDebug()<<"上传简历或数据库该行数据name有空,无法进行name比较";
        return false;
    }
    //name比较
}

bool SessionHrmUploadResume::compareBirthday(QString birthday, QString archivebirthday)
{
    if((birthday.isEmpty())||(archivebirthday.isEmpty()))
    {
        qDebug()<<"上传简历或数据库该行数据birthday有空,无法进行birthday比较";
        return false;
    }
    //birthday比较
    if(birthday==archivebirthday)    //  字符串完全匹配  xxxx-xx-xx与xxxx-xx-xx  xxxx-xx与xxxx-xx
    {
        //qDebug()<<"简历可能重复,重复原因:生日可能相同";
        return true;
    }
    else                            //   如果不完全匹配 是否需要考虑这种情况:1999-07-04    1999-07
    {
        if(birthday.contains(archivebirthday)||archivebirthday.contains(birthday))
        {
            //qDebug()<<"简历可能重复,重复原因:生日可能相同";
            return true;
        }
    }
    //qDebug()<<"生日匹配通过";
    return false;
}

bool SessionHrmUploadResume::compareExperience(QStringList experience,QStringList archiveexperience)
{
    if((experience.isEmpty())||(archiveexperience.isEmpty()))     //格式  默认  xxxx-xx-xxxx-xx
    {
        qDebug()<<"上传简历或数据库该行数据experience有空,无法进行experience比较";
        return false;
    }
    //experience比较
    int num=0;
    foreach(QString s,experience)
    {
        foreach(QString ss,archiveexperience)
        {
           if(s==ss)
           {
               num++;
               qDebug()<<"含有相同的经历段:"<<s;
           }
        }
    }
    if(num>=EXPERIENCE_MATCH)
    {
        qDebug()<<"简历可能重复,重复原因:经历段有相似";
        return true;
    }
    qDebug()<<"经历段匹配通过";
    return false;
}

bool compare_char_(char c1, char c2)
{
    return c1 == c2;
}

size_t ins_(char c)
{
    return 1;
}

size_t del_(char c)
{
    return 1;
}

size_t sub_(char c1, char c2)
{
    return compare_char_(c1, c2) ? 0 : 2;
}


size_t compare_(const std::string& ref_s, const std::string& ref_l)
{
    size_t len_s = ref_s.length();
    size_t len_l = ref_l.length();

    size_t** distance = new size_t*[len_s + 1];
    for(size_t i = 0; i < len_s + 1; ++i)
    {
        distance[i] = new size_t[len_l + 1];
    }

    distance[0][0] = 0;

    for(size_t i = 1; i < len_s + 1; ++i)
    {
        distance[i][0] = distance[i - 1][0] + del_(ref_s.at(i - 1));
    }

    for(size_t i = 1; i < len_l + 1; ++i)
    {
        distance[0][i] = distance[0][i - 1] + ins_(ref_l.at(i - 1));
    }

    for(size_t i = 1; i < len_s + 1; ++i)
    {
        for(size_t j = 1; j < len_l + 1; ++j)
        {
            size_t ins = distance[i][j - 1] + ins_(ref_l.at(j - 1));
            size_t del = distance[i - 1][j] + del_(ref_s.at(i - 1));
            size_t sub = distance[i - 1][j - 1] + sub_(ref_s.at(i - 1), ref_l.at(j - 1));

            distance[i][j] = std::min(std::min(ins, del), sub);
        }
    }

    //myadd
    float Ret=distance[len_s][len_l];
    delete distance;
    //
    return Ret;
}

float SessionHrmUploadResume::textCompare(QString &UploadText, QString &ArchiveText)
{
    std::string ref1=UploadText.toStdString();
    std::string ref2=ArchiveText.toStdString();
    if(ref1.empty() && ref2.empty())
    {
        qDebug()<<"[SessionHrmUploadResume::textCompare]"<<"input is empty, compare failed";
        return 1;
    }

    size_t distance = 0;
    size_t len = 0;

    if(ref1.length() < ref2.length())
    {
        distance = compare_(ref1, ref2);
        len = ref2.length();
    }
    else
    {
        distance = compare_(ref2, ref1);
        len = ref1.length();
    }

    return distance < len ? 1 - static_cast<float>(distance) / len : 0;

}
