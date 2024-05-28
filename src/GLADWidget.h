#ifndef GLAD_WIDGET_H
#define GLAD_WIDGET_H

#include <QOpenGLWidget>

class GLADWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    GLADWidget(QWidget* parent=nullptr);
    ~GLADWidget();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    virtual QSize sizeHint() const override;
};

#endif // GLAD_WIDGET_H