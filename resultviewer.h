#ifndef RESULTVIEWER_H
#define RESULTVIEWER_H

#include <QMainWindow>

#include <QGraphicsScene>

namespace Ui {
class ResultViewer;
}

class ResultViewer : public QMainWindow
{
    Q_OBJECT

public:
    ResultViewer(const QImage *before, QWidget *parent = nullptr);
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

    static double *FFTWCompute(const uchar *input, int width, int height);
    static uchar *inverseFFTWCompute(const double *input, int width, int height);

};

#endif // RESULTVIEWER_H
