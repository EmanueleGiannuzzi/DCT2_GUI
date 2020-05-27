#ifndef RESULTVIEWER_H
#define RESULTVIEWER_H

#include <QMainWindow>

#include <QGraphicsScene>
#include <fftw3.h>

namespace Ui {
class ResultViewer;
}

class ResultViewer : public QMainWindow
{
    Q_OBJECT

public:
    ResultViewer(const QImage *before, int fParam, int dParam, QWidget *parent = nullptr);
    ~ResultViewer();


private slots:
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent *event);

    void updateAfterImage();

private:
    Ui::ResultViewer *ui;

    const QImage *beforeImage;
    QPixmap beforePixmap;
    QPixmap afterPixmap;
    QGraphicsScene *beforeScene;
    QGraphicsScene *afterScene;
    int F;
    int D;

    static double *FFTWCompute(const double *input, int size);
    static double *iFFTWCompute(const double *input, int size);

};

#endif // RESULTVIEWER_H
