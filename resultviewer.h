#ifndef RESULTVIEWER_H
#define RESULTVIEWER_H

#include <QMainWindow>

#include <QGraphicsScene>
#include <fftw3.h>
#include <math.h>
#include <QDebug>

namespace Ui {
class ResultViewer;
}

class ResultViewer : public QMainWindow
{
    Q_OBJECT

public:
    ResultViewer(const QImage *before, int F, int D, QWidget *parent = nullptr);
    ~ResultViewer();

private slots:
    void resizeEvent(QResizeEvent* event);

    void showEvent(QShowEvent *event);

private:
    Ui::ResultViewer *ui;

    const QImage *beforeImage;
    QPixmap beforePixmap;
    QPixmap afterPixmap;
    QGraphicsScene *beforeScene;
    QGraphicsScene *afterScene;

    static double *FFTWCompute(const uchar *input, int size);
    static uchar *iFFTWCompute(const double *input, int size);
};

#endif // RESULTVIEWER_H
