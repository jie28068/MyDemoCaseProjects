#ifndef PROJECTMANAGERSERVER_H
#define PROJECTMANAGERSERVER_H

#include "CoreLib/ServerBase.h"
#include "ProjectManagerServer/IProjectManagerServer.h"
#include <QApplication>
#include <QDir>

using namespace Kcc;
using namespace Kcc::ProjectManager;

// 当前项目，默认存放位置
#ifdef COMPILER_PRODUCT_DESIGNER
static const QString CURRENT_PROJECT_PATH = (QDir::homePath() + "/AppData/Local/SimuLab.Designer/currentProject");
static const QString SIMUNPS_WORKSPACE_PATH = (QDir::homePath() + "/AppData/Local/SimuLab.Designer/workspace1.0");
#else
static const QString CURRENT_PROJECT_PATH = (QDir::homePath() + "/AppData/Local/SimuNPS/currentProject");
static const QString SIMUNPS_WORKSPACE_PATH = (QDir::homePath() + "/AppData/Local/SimuNPS/workspace1.0");
#endif

class ProjectManagerServer : public QObject, public ServerBase, public IProjectManagerServer
{
    Q_OBJECT
public:
    ProjectManagerServer();
    virtual ~ProjectManagerServer();

    // ServerBase
    void Init() override;
    void UnInit() override;

    // IProjectManagerServer
    // 处理所有项目
    virtual PKLProject CreatProject(const QString &dir, const QString &projName, QString &projPath);
    virtual PKLProject LoadProject(const QString &projPath);
    virtual bool DestoryProject(PKLProject project);
    virtual bool FindProject(const QString &path);
    virtual QStringList GetAllProjectPath();
    virtual void SetCurProject(PKLProject project);
    virtual PKLProject GetCurProject();
    virtual PKLProject GetProject(const QString &projectPath);
    virtual PKLProject ImportDemoProject(const QString &targetDir, const QString &filePath,
                                         const QString &targetProjectName,
                                         KLProject::ProjectMode prjModel = KLProject::ProjectMode::ProjectMode_SimuNPS);
    virtual void initProjectBoardStatus(PKLProject project);
    virtual PKLProject CopyProject(PKLProject sourceProject, const QString &targetDir,
                                   const QString &targetProjectName);
    virtual PKLProject MoveProject(PKLProject sourceProject, const QString &targetDir,
                                   const QString &targetProjectName);
    virtual PKLProject ImportProject(const QString &targetDir, const QString &filePath,
                                     KLProject::ProjectMode prjModel = KLProject::ProjectMode::ProjectMode_SimuNPS);
    virtual bool ExportProject(const QString &targetDir, const QString &filePath);
    virtual void UpdataProjectPrefix(const QString &sourcePrefixfDir, const QString &newPrefixfDir);
    virtual bool RenamProjectName(PKLProject sourceProject, const QString &newProjectName);
    virtual void LoadAllProjectFinished();
    virtual void SaveProjectAllModel(QList<PKLProject> projectList);

    // 处理当前项目
    /// @ 外部接口 ****************
    virtual bool CheckNameIsvalid(const QString &name) override;

    //////////////// 画板Model
    virtual QList<PModel> GetAllBoardModel(int modelType = -1) override;
    virtual PModel CreateBoardModel(const QString &boardName, int modelType) override;
    virtual bool DestroyBoardModel(PModel boardModel) override;
    virtual void ConfirmUserDefinedBoardModel(const QString &boardName) override;
    virtual bool SaveBoardModel(PModel boardModel, bool isLog = true) override;
    virtual bool ReloadBoardModel(PModel boardModel) override;
    virtual bool RenameBoardModel(PModel boardModel, const QString oldName,
                                  const QString newName) override; // 弃用后续删除
    virtual PModel GetBoardModelByName(const QString &boardName) override;
    virtual PModel GetBoardModelByUUID(const QString &uuid) override;
    virtual bool IsExistModel(const QString &modelName,
                              DuplicateChecks checkType = (DuplicateCheck_ToolBox | DuplicateCheck_CurProject
                                                           | DuplicateCheck_SystemBuildIn)) override;
    virtual QStringList GetActiveBoardModelNames() override;

    //////////////// Block Model
    virtual PModel CreateBlockModel(const QString &prototypeName) override;

    //////////////// 系统构造 Model
    virtual bool AddSystemCombineBoardModel(PCombineBoardModel combineModel) override;
    virtual bool DeleteSystemCombineBoardModel(PCombineBoardModel combineModel) override;
    virtual QList<PCombineBoardModel> GetSystemCombineBoardModels() override;

    //////////////// 本地代码 Model
    virtual bool AddControlModel(PModel model) override;
    virtual bool DeleteControlModel(PModel model) override;
    virtual bool SaveControlModel(PModel model) override;
    virtual PModel GetControlModel(const QString &boardName) override;
    virtual QList<PModel> GetAllControlModel() override;

    //////////////// 设备类型 Model
    virtual bool AddDeviceModel(PDeviceModel deviceModel) override;
    virtual bool DeleteDeviceModel(PDeviceModel deviceModel) override;
    virtual bool ModifyDeviceModel(const QString &oldName, PDeviceModel deviceModel) override;
    virtual QList<PDeviceModel> GetDeviceModels(const QString &prototypeName) override;
    virtual QMap<QString, QList<PDeviceModel>> GetDeviceModels() override;
    virtual bool HaveDeviceModel(const QString &prototypeName) override;
    virtual PDeviceModel GetSystemDeviceModel(const QString &prototypename) override;
    virtual QList<PDeviceModel> GetSystemDeviceModels() override;

    //////////////// 项目
    virtual QVariant GetProjectConfig(const QString &key) override;
    virtual void SetProjectConfig(const QString &key, const QVariant &value, bool isDelete = false) override;
    virtual QString GetCurrentProjectAbsoluteDir() override;
    virtual QString GetCurrentProjectRelativeDir() override;

    //////////////// 其他
    virtual QString GetModelPrototypeNameCHS(const QString &prototypeName) override;
    virtual QString GetRealTimeSimulationName() override;
    virtual QString GetSimulationStorageName() override;
    virtual QString GetCodeModelDlldir() override;
    virtual QColor GetColorOfVolLevel(const double &voltageLevel) override;

    //////////////// 数据字典
    virtual bool CreateAndRunDataDictionary() override;
    virtual void UpdateBoardDataDictionary(PModel boardModel) override;

    //////////////// 仿真
    virtual bool CreateSimulationVariableList(const QString &storageName, const QString &simulationIdentifier,
                                              const QVector<ParamIdentifier> &allParamIds) override;
    virtual bool ClearLastSimulationData(const QString &storageName) override;
    virtual QSharedPointer<QMap<QString, QMap<QString, QMap<quint64, double>>>>
    GetAllSimulationResultData(const QString &storageName = CURRENTSIMULATION) override;
    virtual bool AppendSimulationVariableList(const QVector<ParamIdentifier> &appendParamIds) override;
    virtual bool WriteSimulationVariableBlockValue(const QString &simulationIdentifier,
                                                   const QVector<ParamIdentifier> &allParamIds,
                                                   const QVector<quint64> &timeList,
                                                   const QVector<QVector<double>> &variableBlock) override;
    virtual void WriteSimulationVariableFinished(const QString &simulationIdentifier) override;

    /// @brief 打开quikis
    /// @param filePath 仿真结果文件夹路径
    virtual void OpenQUIKIS(const QString filePath) override;

signals:
    void openMonitor();

private slots:
    void onBoardActiveStatusChange(QVariant activeStatus);
    void onConfigInfoChange(const QString &key, const QVariant &value);
    void onBoardLabelChange(const QVariant &value);
    void onRecordResult(const QString &recordName, QVector<ParamIdentifier> params);
    void onRenameResult(const QString &sourceName, const QString &newName, QVector<ParamIdentifier> params);
    void onDeleteResult(const QString &sourceName, QVector<ParamIdentifier> params);
    void onLocalCodeBlockChange(const QString &ModelName);
    void onCopyModelFinished();
    void onBoardModelRename(QVariantMap info);
    void onCombineModelSync(QList<QWeakPointer<Model>> models);

private:
    QString setProjectPath(QString prefixDir, QString projName);
    QString setProjectPathExt(QString prefixDir, QString projName);
    QString getProjectPrefix(const QString &projectPath); // 获取项目的前缀路径,即工作空间路径
    QString setExportProjectPath(QString prefixDir, QString name);
    void saveProjectPath();
    QString readProjectPath();
    void bindSignal(PKLProject project);
    void initBoardStatus(PModel model);
    void recordBoardOpenStatus();  // 记录项目中画板打开状态
    void recoverBoardOpenStatus(); // 恢复项目中画板打开状态

    PKLProject getBuiltinProject(); // 系统内建model统一放在内建项目中管理
    void buildDDSstructure();
    bool checkProjectIsStale(const QString &projePath); // 检测导入的项目是否过期
    void saveProject(PKLProject project);

private:
    // 项目列表 key为项目完整路径
    QMap<QString, PKLProject> projects;
    // 当前项目路径
    QString curProjectPath;

    KLFileManager *fileManager;

    IProjectManager *projectManager;
};

#endif // PROJECTMANAGERSERVER_H
