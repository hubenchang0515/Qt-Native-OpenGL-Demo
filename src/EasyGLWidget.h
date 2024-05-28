#ifndef EASYGL_WIDGET_H
#define EASYGL_WIDGET_H

#include <EasyGL/EasyGL.h>
#include <QOpenGLWidget>

class EasyGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    EasyGLWidget(QWidget* parent=nullptr);
    ~EasyGLWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    virtual QSize sizeHint() const override;
};

#endif // EASYGL_WIDGET_H