#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QLayout>

#include "EasyGLWidget.h"
#include "GLADWidget.h"
#include "GLEWWidget.h"

class MainWindow : public QDialog
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent=nullptr);
    ~MainWindow();

private:
    QGridLayout* m_layout;
    EasyGLWidget* m_easy;
    GLADWidget* m_glad;
    GLEWWidget* m_glew;
};

#endif // MAINWINDOW_H