#include "TestWidget.h"
#include <QKeyEvent>
#include <QCoreApplication>

TestWidget::TestWidget(QWidget *parent)
    : QGLWidget(parent)
{
    xRot = 10.0;
    yRot = 10.0;
    zRot = 10.0;

    resize(500,400);
    startTimer(10);
}

TestWidget::~TestWidget()
{

}

// 加载纹理
void TestWidget::loadGLTexture()
{
    // 加载图片
    QImage buf;
    if(!buf.load(":/logo.png"))
    {
        QImage dummy(128, 128, QImage::Format_RGB32);
        dummy.fill(QColor(Qt::green).rgb());
        buf = dummy;
    }
    QImage tex = QGLWidget::convertToGLFormat(buf);

    // 创建一个纹理名字
    glGenTextures(1, &texture[0]);

    // 将纹理名字绑定到目标上
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // 创建纹理
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());

    // 设置显示图像时的放大、缩小的滤波方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// 初始化opengl
void TestWidget::initializeGL()
{
    // 载入纹理
    loadGLTexture();

    // 启用纹理
    glEnable(GL_TEXTURE_2D);

    // 其他初始化
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

// 尺寸改变
void TestWidget::resizeGL(int w, int h)
{
    h = (h == 0) ? 1 : h;

    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    gluPerspective(45.0,(GLfloat)w/(GLfloat)h,0.1,100.0);

    GLfloat zNear = 0.1;
    GLfloat zFar = 100.0;
    GLfloat aspect = (GLfloat)w / (GLfloat)h;
    GLfloat fH = tan(GLfloat(90.0 / 360.0 * 3.14159)) * zNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// 绘制代码,着色分为两种：平滑着色GL_SMOOTH、单调着色GL_FLAT
void TestWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // 立方体
    glPushMatrix();
    glTranslatef(-0.5, -0.5, -3.0);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);

    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 0.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 1.0, 0.0);

        glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 1.0, 1.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.0, 0.0, 1.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 0.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 1.0);

        glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.0, 1.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 1.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 0.0, 1.0);

        glTexCoord2f(0.0, 0.0); glVertex3f(1.0, 0.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 0.0, 1.0);

        glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 1.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.0, 1.0, 1.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);

        glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(0.0, 0.0, 1.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 0.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 0.0, 0.0);
    }
    glEnd();
    glPopMatrix();
}

// 全屏切换
void TestWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Escape:
        showNormal();
        break;
    case Qt::Key_F1:
        showFullScreen();
        break;
    default:
        break;
    }
}

// 定时滚动
void TestWidget::timerEvent(QTimerEvent *event)
{
    xRot += 0.5;
    yRot += 0.5;
    zRot += 0.5;
    updateGL(); // 重新绘制
}
