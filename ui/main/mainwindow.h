#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <exception>
#include "ui/gl/viewerwindow.h"
#include <QMainWindow>
#include <QVector3D>
#include <QFileDialog>
#include <QColorDialog>
#include <QSettings>
#include <QApplication>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateModelPosition();
    void updateModelRotation();
    void updateModelScale();

    void loadModel();
    void browsePath();

    void updateProjection();

    void browseBgColor();
    void updateBgColor();

    void browseLineColor();
    void updateLineColor();
    void updateLineType();
    void updateLineThick();

    void browsePointColor();
    void updatePointColor();
    void updatePointType();
    void updatePointSize();

private:
    void saveSettings();
    void loadSettings();
    void spawnViewer();

    Ui::MainWindow *ui;
    QString *modelPath = new QString("/Users/yuehbell/dev/C8_3DViewer_v1.0-0/src/models/Female.obj");
    ViewerWindow *viewerWin;
    QString settings_path = QApplication::applicationDirPath().append("/settings.ini");
};
#endif // MAINWINDOW_H
