#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QGLWidget>

class TestWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit TestWidget(QWidget *parent = nullptr);
    ~TestWidget() override;

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void timerEvent(QTimerEvent *event) override;

private:
    void loadGLTexture();

private:
    GLfloat xRot;
    GLfloat yRot;
    GLfloat zRot;
    GLuint texture[1];
};

#endif // TESTWIDGET_H
