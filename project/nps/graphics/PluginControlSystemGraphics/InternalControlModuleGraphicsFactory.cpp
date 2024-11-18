#include "InternalControlModuleGraphicsFactory.h"
#include "CircleSumSourceGraphicsObject.h"
#include "ConstantSourceGraphicsObject.h"
#include "ControlModulePortGraphicsObject.h"
#include "ControlModuleSourceGraphicsObject.h"
#include "GainMouduleSourceGraphicsObject.h"
#include "GoToFromSourceGraphicsObjct.h"
#include "InAndOutSourceGraphicsObject.h"
#include "IrRegularAnchorPortGraphics.h"
#include "TransferFunctionSourceGraphicsObject.h"
#include "BusBarControlGraphicsObject.h"
#include "StepSourceGraphicsObject.h"
#include "SawtoothWavesGraphicsObject.h"
#include "SwitchGraphicsObject.h"
#include "MutiportSwitchGraphicsObject.h"
#include "defines.h"

InternalControlModuleGraphicsFactory::InternalControlModuleGraphicsFactory() : ICanvasGraphicsObjectFactory()
{
    // QStringList mathList;
    // // mathList << "pow"
    // //          << "sin"
    // //          << "ceil"
    // //          << "SineWave"
    // //   << "sqrt"
    // //   << "delay";
    // MathMLManager &mathMLMgr = MathMLManager::getInstance();
    // for each (auto math in mathList) {
    //     QString resPath = QString(":/MathML/%1").arg(math);
    //     mathMLMgr.registerMathML(math, resPath);
    // }
}

SourceGraphicsObject *
InternalControlModuleGraphicsFactory::createSourceGraphicsObject(ICanvasScene *canvasScene,
                                                                 QSharedPointer<SourceProxy> source)
{
    QString sourcePrototypeName = source->prototypeName();
    if (sourcePrototypeName == "Add") {

        return new CircleSumSourceGraphicsObject(canvasScene, source);
    } else if (sourcePrototypeName == "Constant") {
        return new ConstantSourceGraphicsObject(canvasScene, source);
    } else if (sourcePrototypeName == "Goto" || sourcePrototypeName == "From") {
        return new GoToFromSourceGraphicsObjct(canvasScene, source);
    } else if (sourcePrototypeName == "In" || sourcePrototypeName == "Out") {
        return new InAndOutSourceGraphicsObject(canvasScene, source);
    } else if (sourcePrototypeName == "Gain") {
        return new GainMouduleSourceGraphicsObject(canvasScene, source);
    } else if (sourcePrototypeName == "TransFunc_Discrete" || sourcePrototypeName == "TransFunc") {
        return new TransferFunctionSourceGraphicsObject(canvasScene, source, TRANSFUNC_CONTINUE_LETTER);
    } else if (sourcePrototypeName == "DiscreteTransFunc") {
        return new TransferFunctionSourceGraphicsObject(canvasScene, source, TRANSFUNC_DISCRETE_LETTER);
    } else if(sourcePrototypeName == "BusCreator" || sourcePrototypeName == "BusSelector" || sourcePrototypeName == "Mux" || sourcePrototypeName == "DeMux"){
        return new BusBarControlGraphicsObject(canvasScene, source);
    } else if(sourcePrototypeName == "Step"){
        return new StepSourceGraphicsObject(canvasScene, source);
    } else if(sourcePrototypeName == "Sawtooth_waves"){
        return new SawtoothWavesGraphicsObject(canvasScene, source);
    }else if(sourcePrototypeName == "Switch"){
        return new SwitchGraphicsObject(canvasScene, source);
    }else if(sourcePrototypeName == "MultiportSwitch"){
        return new MutiportSwitchGraphicsObject(canvasScene, source);
    }

    return new ControlModuleSourceGraphicsObject(canvasScene, source);
}

PortGraphicsObject *InternalControlModuleGraphicsFactory::createPortGraphicsObject(ICanvasScene *canvasScene,
                                                                                   SourceGraphicsObject *sourceGraphics,
                                                                                   QSharedPointer<PortContext> context)
{
    PSourceProxy sourceProxy = sourceGraphics->getSourceProxy();
    if (sourceProxy->prototypeName() == "Add") {
        return new IrRegularAnchorPortGraphics(canvasScene, sourceGraphics, context);
    }
    return new ControlModulePortGraphicsObject(canvasScene, sourceGraphics, context);
}
