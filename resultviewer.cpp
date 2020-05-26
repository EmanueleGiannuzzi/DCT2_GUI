#include "resultviewer.h"
#include "ui_resultviewer.h"

#include <fftw3.h>
#include <QTimer>
#include <QProgressBar>
#include <thread>

ResultViewer::ResultViewer(const QImage *before, int fParam, int dParam, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ResultViewer),
    beforeImage(before),
    F(fParam),
    D(dParam)
{
    ui->setupUi(this);

//    QProgressBar *progressBar = this->parent()->findChild<QProgressBar*>("progressBar");
//    progressBar->setEnabled(true);

    this->beforePixmap = QPixmap::fromImage(*this->beforeImage);
    this->beforeScene = new QGraphicsScene(this);
    this->beforeScene->addPixmap(this->beforePixmap);
    this->beforeScene->setSceneRect(this->beforePixmap.rect());
    this->ui->beforeGraphicsView->setScene(this->beforeScene);

    //updateAfterImage();
    QTimer::singleShot(0, this, SLOT(updateAfterImage()));
}

void ResultViewer::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    this->ui->beforeGraphicsView->fitInView(this->beforePixmap.rect(), Qt::KeepAspectRatio);
    this->ui->afterGraphicsView->fitInView(this->afterPixmap.rect(), Qt::KeepAspectRatio);
}

void ResultViewer::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    this->ui->beforeGraphicsView->fitInView(this->beforePixmap.rect(), Qt::KeepAspectRatio);
    this->ui->afterGraphicsView->fitInView(this->afterPixmap.rect(), Qt::KeepAspectRatio);
}

void ResultViewer::updateAfterImage()
{
    const int width = this->beforeImage->width();
    const int height = this->beforeImage->height();
    //const int depth = this->depth();
    const int arraySize = width*height;

    const int dimRow = (int)(height / F);
    const int dimCol = (int)(width / F);
    const int fArraySize = F*F;

    uchar *resultImageData = new uchar[arraySize];


//    QProgressBar *progressBar = this->parent()->findChild<QProgressBar*>("progressBar");
    QProgressBar *progressBar = this->ui->progressBar;
    progressBar->setRange(0, dimRow*dimCol);

    for(int k = 0; k < dimRow; ++k) {
        for(int l = 0; l < dimCol; ++l) {
            double *matrix = new double[fArraySize];
            int row = k*F;
            int col = l*F;
            for(int i = 0; i < F; ++i) {
                for(int j = 0; j < F; ++j) {
                    matrix[i*F+j] = (double)qGray(this->beforeImage->pixel(col+j, row+i));
                }
            }

            double *arrayResult = ResultViewer::FFTWCompute(matrix, F);
            for(int i = 0; i < F; ++i) {
                for(int j = 0; j < F; ++j) {
                    if (i+j >= D){
                        arrayResult[i+F*j] = 0.0;
                    }
                }
            }
            double *inverseResult = ResultViewer::iFFTWCompute(arrayResult, F);

            for(int i = 0; i < F; ++i) {
                for(int j = 0; j < F; ++j) {
                    resultImageData[(row+i)*width+(col+j)] = inverseResult[i*F+j];
                }
            }

            if(progressBar != nullptr) {
                progressBar->setValue(k*dimRow+l);
            }
        }
    }

    for(int i = 0; i < height; ++i) {
        for(int j = dimCol*F; j < width; ++j) {
            resultImageData[i*width+j] = qGray(beforeImage->pixel(j, i));
        }
    }

    for(int i = dimRow*F; i < height; ++i) {
        for(int j = 0; j < dimCol*F; ++j) {
            resultImageData[i*width+j] = qGray(beforeImage->pixel(j, i));
        }
    }

    QImage resultImage = QImage(width, height, QImage::Format_Grayscale8);

    for(int i = 0; i<height; ++i) {
        for(int j = 0; j<width; ++j) {
            int colorValue = resultImageData[i*width+j];
            QRgb color = qRgb(colorValue, colorValue, colorValue);
            resultImage.setPixel(j, i, color);
        }
    }

    delete[] resultImageData;

    this->afterPixmap = QPixmap::fromImage(resultImage);
    afterScene = new QGraphicsScene(this);
    afterScene->addPixmap(this->afterPixmap);
    afterScene->setSceneRect(this->afterPixmap.rect());
    this->ui->afterGraphicsView->setScene(this->afterScene);

    this->ui->afterGraphicsView->setEnabled(true);
    this->ui->afterGraphicsView->fitInView(this->afterPixmap.rect(), Qt::KeepAspectRatio);

    if(progressBar != nullptr) {
        progressBar->setValue(progressBar->maximum());
    }
}

double *ResultViewer::FFTWCompute(const double *input, int size)
{
    const int arraySize = size*size;

    double *in = new double[arraySize];
    double *out = new double[arraySize];

    for(int i = 0; i<arraySize; ++i) {
        in[i] = input[i];
    }

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(size, size, in, out, FFTW_REDFT10, FFTW_REDFT10, FFTW_MEASURE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    fftw_cleanup();

    delete[] in;
    return out;
}

double *ResultViewer::iFFTWCompute(const double *input, int size)
{
    const int arraySize = size*size;

    double *out = new double[arraySize];
    double *in = new double[arraySize];

    for(int i = 0; i<arraySize; ++i) {
        in[i] = (double)input[i];
    }

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(size, size, in, out, FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    fftw_cleanup();

    double scaleFactor = 4*arraySize;

    for(int i = 0; i<arraySize; ++i) {
        double scaledValue = qMin(qMax(out[i]/scaleFactor, 0.0), 255.0);
        out[i] = (uchar)floor(scaledValue);
    }

    delete[] in;

    return out;
}

ResultViewer::~ResultViewer()
{
    delete ui;
    //delete beforeImage;//Probabilmente viene già eliminatata da MainWindow
    delete beforeScene;
    delete afterScene;
}
