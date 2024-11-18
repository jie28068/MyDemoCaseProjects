#ifndef LAYERPROPERTY_H
#define LAYERPROPERTY_H

#include "CommonWrapper.h"
#include "LayerWidget.h"

class ICanvasScene;

#define LAYER_TAB_NAME_LAYERS QObject::tr("Layer Configuration")             // 图层配置
#define LAYER_TAB_NAME_LAYERSSETTING QObject::tr("Layer Attribute Settings") // 图层属性设定

class LayerProperty : public CommonWrapper
{
    Q_OBJECT
public:
    LayerProperty(QSharedPointer<ICanvasScene> pscene);
    ~LayerProperty();

    void init();

    // CommonWrapper
    virtual QString getTitle() override;
    virtual void onDialogExecuteResult(QDialog::DialogCode code) override;

private:
    QList<QCheckBox *> m_checkBoxListOld;
    LayerWidget *m_pLayerWidget;
    LayerSettingWidget *m_pLayerSettingWidget;

    QMap<QString, bool> m_layerItemsMap; // 图层显示名称和canvascontext保存名称对应关系
    QSharedPointer<ICanvasScene> m_pCanvasScene;
};

#endif // LAYERPROPERTY_H
