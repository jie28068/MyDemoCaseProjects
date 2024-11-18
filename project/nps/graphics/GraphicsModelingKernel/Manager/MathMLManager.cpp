#include "MathMLManager.h"
#include "qwt_mml_document.h"

#include <QApplication>
#include <QBuffer>
#include <QFile>
#include <QMap>
#include <QPainter>
#include <QSvgGenerator>
#include <QSvgRenderer>

class MathMLManagerPrivate
{
public:
    QMap<QString, QSvgRenderer *> mathMLSvgRenderMap;
};

MathMLManager::MathMLManager()
{
    dataPtr.reset(new MathMLManagerPrivate);
}

MathMLManager::~MathMLManager() { }

MathMLManager &MathMLManager::getInstance()
{
    static MathMLManager instance;
    return instance;
}

bool MathMLManager::registerMathML(const QString &name, const QString &filePath)
{
    if (name.isEmpty() || dataPtr->mathMLSvgRenderMap.contains(name)) {
        return false;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    bool ret = true;
    QByteArray contents = file.readAll();
    QString xmlContents;
    xmlContents.append(contents);
    QwtMathMLDocument *mathMLDoc = new QwtMathMLDocument();
    bool succeed = mathMLDoc->setContent(xmlContents);
    if (!succeed) {
        ret = false;
    } else {
        QByteArray svgBytes;
        QBuffer buffer(&svgBytes);
        {
            QSize docSize = mathMLDoc->size();
            QRect imageRect = QRect(QPoint(0, 0), docSize);
            QSvgGenerator svgGenerator;
            int ratio = qApp->devicePixelRatio();
            svgGenerator.setResolution(96 * ratio);
            svgGenerator.setSize(docSize);
            svgGenerator.setViewBox(QRectF(QPoint(0, 0), docSize));
            svgGenerator.setOutputDevice(&buffer);

            QPainter svgPainter(&svgGenerator);
            svgPainter.setRenderHint(QPainter::Antialiasing, true);
            svgPainter.setRenderHint(QPainter::TextAntialiasing, true);

            QRectF mathMLRect;
            mathMLRect.setSize(docSize);
            mathMLRect.moveCenter(imageRect.center());

            mathMLDoc->paint(&svgPainter, mathMLRect.topLeft().toPoint());
        }
        QSvgRenderer *render = new QSvgRenderer();
        succeed = render->load(svgBytes);
        if (succeed) {
            render->setAspectRatioMode(Qt::KeepAspectRatio);
            dataPtr->mathMLSvgRenderMap[name] = render;
        } else {
            ret = false;
        }
    }

    delete mathMLDoc;

    return ret;
}

QSvgRenderer *MathMLManager::getMathMLDocument(const QString &name)
{
    return dataPtr->mathMLSvgRenderMap.value(name, nullptr);
}
