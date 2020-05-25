#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "resultviewer.h"

#include <QMainWindow>
#include <QGraphicsScene>

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
    void on_toolButton_clicked();

    void resizeEvent(QResizeEvent* event);

    void on_fParamSpinBox_valueChanged(int arg1);

    void on_dParamSlider_rangeChanged(int min, int max);

    void on_dParamSlider_valueChanged(int value);

    void on_runButton_clicked();

    void on_imagePathLineEdit_returnPressed();

private:
    Ui::MainWindow *ui;

    ResultViewer *resultWindow;

    QImage *previewImage;
    QPixmap previewPixmap;
    QGraphicsScene *scene;

    void loadPreviewImage();
    void updatePreviewSize();
    void updateDParam();
    void updateFParam();
};
#endif // MAINWINDOW_H
