#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    void on_dParamSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;

    QImage  *imageObject;
    QPixmap image;
    QGraphicsScene *scene;

    void loadPreviewImage();
    void updatePreviewSize();
    void updateDParam();
    void updateFParam();
};
#endif // MAINWINDOW_H
