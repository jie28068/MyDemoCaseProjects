#ifndef IMPORTBLOCKPROPERTY_H
#define IMPORTBLOCKPROPERTY_H

#include "ProjectManagerServer/IProjectManagerServer.h"
#include "server/CodeManagerServer/ICodeManagerServer.h"
#include "ui_importblockproperty.h"
#include <QString>
#include <QWidget>
using namespace Kcc::ProjectManager;

class ImportBlockProperty : public QWidget
{
    Q_OBJECT

public:
    ImportBlockProperty(QWidget *parent = nullptr);
    ~ImportBlockProperty();

public:
    inline QString getFileName() { return m_fileName; }
    inline QString getFilePath() { return m_strFilePath; }
    inline QString getBlockName() { return m_blockName; }

private slots:
    void onLoadFile();
    void onTextChanged(const QString &str);

private:
    Ui::importblock ui;
    QString m_blockName;
    QString m_strFilePath;
    QString m_fileName;
};

class ImportSimulinkBlock : public QObject
{
    Q_OBJECT

public:
    ImportSimulinkBlock(PIProjectManagerServer pProjectServer, QString blockName);

    void initPortInfo(PControlBlock pBlock);
    void createBlockByCallback(const QString &strInfo);
    static bool importCallback(void *pParam, const QString &strInfo, Kcc::CodeManager::CompileCode code);

signals:
    void refreshView();

private:
    PIProjectManagerServer m_pProjectServer; // 项目管理服务接口
    QString m_blockName;
};

#endif // !IMPORTBLOCKPROPERTY_H