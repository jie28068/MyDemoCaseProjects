#pragma once

#include "Global.h"

#include <QSvgRenderer>

class MathMLManagerPrivate;
class QwtMathMLDocument;

class GRAPHICSMODELINGKERNEL_EXPORT MathMLManager
{
public:
    static MathMLManager &getInstance();

    bool registerMathML(const QString &name, const QString &filePath);

    QSvgRenderer *getMathMLDocument(const QString &name);

private:
    MathMLManager();
    ~MathMLManager();

    QScopedPointer<MathMLManagerPrivate> dataPtr;
};