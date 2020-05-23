#include "resultviewer.h"
#include "ui_resultviewer.h"

#include <fftw3.h>
#include <QDebug>

#include <math.h>

ResultViewer::ResultViewer(const QImage *before, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ResultViewer),
    beforeImage(before)
{
    ui->setupUi(this);

    this->beforePixmap = QPixmap::fromImage(*this->beforeImage);
    this->beforeScene = new QGraphicsScene(this);
    this->beforeScene->addPixmap(this->beforePixmap);
    this->beforeScene->setSceneRect(this->beforePixmap.rect());
    this->ui->beforeGraphicsView->setScene(this->beforeScene);

    int width = this->beforeImage->width();
    int height = this->beforeImage->height();

    int size = qMin(width, height);//TODO


    //double *arrayResult = ResultViewer::FFTWCompute(this->beforeImage->bits(), size);
    //uchar *inverseResult = ResultViewer::inverseFFTWCompute(arrayResult, size);

    double *arrayResult = ResultViewer::FFTWCompute2D(this->beforeImage->bits(), size);
    uchar *inverseResult = ResultViewer::iFFTWCompute2D(arrayResult, size);

    //QImage resultImage = QImage(*inverseResult, width, height, QImage::Format_Grayscale8);
    QImage resultImage = QImage(width, height, QImage::Format_Grayscale8);
    for(int i = 0; i<height; ++i) {
        for(int j = 0; j<width; ++j) {
            int colorValue = inverseResult[i*width+j];
            QRgb color = qRgb(colorValue, colorValue, colorValue);
            resultImage.setPixel(j, i, color);
        }
    }

    delete[] inverseResult;
    delete[] arrayResult;

    this->afterPixmap = QPixmap::fromImage(resultImage);
    afterScene = new QGraphicsScene(this);
    afterScene->addPixmap(this->afterPixmap);
    afterScene->setSceneRect(this->afterPixmap.rect());
    this->ui->afterGraphicsView->setScene(this->afterScene);
}

double *ResultViewer::FFTWCompute(const uchar *input, int size)
{
    const int arraySize = size*size;

    double *in = new double[arraySize];
    double *out = new double[arraySize];

    for(int i = 0; i<arraySize; ++i) {
        in[i] = (double)(input[i]/255.0);
    }

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(size, size, in, out, FFTW_REDFT10, FFTW_REDFT10, FFTW_ESTIMATE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);

    delete[] in;
    return out;
}

uchar *ResultViewer::inverseFFTWCompute(const double *input, int size)
{
    const int arraySize = size*size;

    uchar *out = new uchar[arraySize];

    double *tempOut = new double[arraySize];
    double *in = new double[arraySize];

    /*in[0] = input[0] / sqrt(1.0 / 2 /size);
    double f = sqrt(1.0 / 2 /size);
    for(int i = 1; i<arraySize; ++i) {
        in[i] = (double)input[i] / f;
    }*/

    for(int i = 0; i<arraySize; ++i) {
        in[i] = (double)input[i];
    }

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(size, size, in, tempOut, FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    fftw_cleanup();

    double scaleFactor = arraySize << 1;
    for(int i = 0; i<arraySize; ++i) {
        double scaledValue = qMax(qMin((tempOut[i] / scaleFactor), 1.0), 0.0);
        out[i] = scaledValue * 255;
    }

    delete[] in;
    delete[] tempOut;

    return out;
}

void ResultViewer::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    this->ui->beforeGraphicsView->fitInView(this->beforePixmap.rect(), Qt::KeepAspectRatio);
    this->ui->afterGraphicsView->fitInView(this->afterPixmap.rect(), Qt::KeepAspectRatio);
}

void ResultViewer::showEvent(QShowEvent *event)
{
    this->ui->beforeGraphicsView->fitInView(this->beforePixmap.rect(), Qt::KeepAspectRatio);
    this->ui->afterGraphicsView->fitInView(this->afterPixmap.rect(), Qt::KeepAspectRatio);
}

ResultViewer::~ResultViewer()
{
    delete ui;
    //delete beforeImage;//Probabilmente viene già eliminatata da MainWindow
    delete beforeScene;
    delete afterScene;
}
