#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent):
    QDialog{parent},
    m_layout{new QGridLayout},
    m_easy{new EasyGLWidget},
    m_glad{new GLADWidget},
    m_glew{new GLEWWidget}
{
    m_layout->addWidget(m_easy, 0, 0, 2, 1);
    m_layout->addWidget(m_glad, 0, 1);
    m_layout->addWidget(m_glew, 1, 1);
    setLayout(m_layout);
}

MainWindow::~MainWindow()
{

}