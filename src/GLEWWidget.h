#ifndef GLEW_WIDGET_H
#define GLEW_WIDGET_H

#include <QOpenGLWidget>

class GLEWWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    GLEWWidget(QWidget* parent=nullptr);
    ~GLEWWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    virtual QSize sizeHint() const override;
};

#endif // GLEW_WIDGET_H