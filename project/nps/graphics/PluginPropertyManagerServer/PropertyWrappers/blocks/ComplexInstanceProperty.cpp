#include "ComplexInstanceProperty.h"
#include "CommonModelAssistant.h"

ComplexInstanceProperty::ComplexInstanceProperty(QSharedPointer<Kcc::BlockDefinition::Model> complexModel,
                                                 ComplexInstanceWidget::Operation ope, const QString &cpxProtoName,
                                                 bool readOnly)
    : CommonWrapper(readOnly),
      m_complexModel(complexModel),
      m_complexWidget(nullptr),
      m_Operation(ope),
      m_cpxInitProtoModelName(cpxProtoName)
{
}

ComplexInstanceProperty::~ComplexInstanceProperty() { }

void ComplexInstanceProperty::init()
{
    if (m_complexWidget == nullptr) {
        m_complexWidget = new ComplexInstanceWidget(m_complexModel, m_Operation, m_cpxInitProtoModelName);
    }
    addWidget(m_complexWidget, CMA::BOARD_TAB_NAME_INFO);
    setPropertyEditableStatus(m_readOnly);
}

QString ComplexInstanceProperty::getTitle()
{
    if (ComplexInstanceWidget::Operation_NewInstance == m_Operation) {
        return CMA::TITLE_NEW_COMPLEX_INSTANCE;
    } else if (ComplexInstanceWidget::Operation_EditInstance == m_Operation) {
        return CMA::TITLE_EDIT_COMPLEX_INSTANCE;
    }

    return QString();
}

QSharedPointer<Kcc::BlockDefinition::Model> ComplexInstanceProperty::getComplexModel()
{
    if (m_complexWidget != nullptr) {
        return m_complexWidget->getComplexInstance();
    }
    return QSharedPointer<Kcc::BlockDefinition::Model>(nullptr);
}

void ComplexInstanceProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    m_bPropertyIsChanged = false;
    if (m_complexWidget == nullptr || QDialog::Accepted != code) {
        return;
    }

    if (m_complexWidget != nullptr && m_complexWidget->saveData()) {
        m_bPropertyIsChanged = true;
        CMA::saveModel(getComplexModel());
    }
}
