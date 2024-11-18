#include "ProjectManagerViewServer.h"
#include "PluginProjectManagerView.h"

ProjectManagerViewServer::ProjectManagerViewServer(PluginProjectManagerView *projview) : m_projectManagerView(projview)
{
}

ProjectManagerViewServer::~ProjectManagerViewServer() { }

void ProjectManagerViewServer::JumpToReferenceModel(const QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || m_projectManagerView == nullptr) {
        return;
    }
    m_projectManagerView->JumpToReferenceModel(model);
}

void ProjectManagerViewServer::editExternalProject(const QString &projectAbsPath)
{
    if (m_projectManagerView != nullptr) {
        m_projectManagerView->editExternalProject(projectAbsPath);
    }
}
