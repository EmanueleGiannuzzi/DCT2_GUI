#include "resultviewer.h"
#include "ui_resultviewer.h"

#include <fftw3.h>
#include <QDebug>
#include <QVector>


ResultViewer::ResultViewer(const QImage *before, int F, int D, QWidget *parent) :
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

    const uchar *input = beforeImage->bits();
    const int width = this->beforeImage->width();
    const int height = this->beforeImage->height();
    const int arraySize = width*height;

    const int dimRow = (int)(height / F);
    const int dimCol = (int)(width / F);

    const int fArraySize = F*F;

    qInfo() << "BANANA";

    QVector<uchar*> matrices;
    for(int k = 0; k+F <= height; k+=F) {
        for(int l = 0; l+F <= width; l+=F) {
            uchar* matrix = new uchar[fArraySize];
            for(int i = 0; i < F; ++i) {
                for(int j = 0; j < F; ++j) {
                    matrix[i*F+j] = input[(k+i)*width+(l+j)];
                }
            }
            matrices.push_back(matrix);
        }
    }
    qInfo() << "BANANA1";

    QVector<uchar*> compressed;//(dimRow * dimCol);
    for(uchar* matrix : matrices) {
        double *arrayResult = ResultViewer::FFTWCompute(matrix, F);
        for(int k = 0; k < F-1; ++k) {
            for(int l = 0; l < F-1; ++l) {
                if (k+l > D){
                    arrayResult[k+dimCol*l] = 0.0f;
                }
            }
        }
        uchar *inverseResult = ResultViewer::iFFTWCompute(arrayResult, F);
        compressed.push_back(inverseResult);
    }

    qInfo() << "BANANA2";

    uchar *resultImageData = new uchar[arraySize];

    for(int k = 0; k < dimRow; ++k) {
        for(int l = 0; l < dimCol; ++l) {
            const uchar *matrix = compressed.at(k*dimRow+l);
            int row = k*F;
            int col = l*F;
            for(int i = 0; i < F; ++i) {
                for(int j = 0; j < F; ++j) {
                    resultImageData[(row+i)*width+(col+j)] = matrix[i*F+j];
                }
            }
        }
    }

    qInfo() << "BANANA3";

    for(int i = 0; i < height; ++i) {
        for(int j = F*dimCol; j < width; ++j) {
            resultImageData[i*width+j] = input[i*width+j];
        }
    }
    for(int i = F*dimRow; i < height; ++i) {
        for(int j = 0; j < dimCol*F; ++j) {
            resultImageData[i*width+j] = input[i*width+j];
        }
    }
    qInfo() << "BANANA4";

    QImage resultImage = QImage(width, height, QImage::Format_Grayscale8);
    for(int i = 0; i<height; ++i) {
        for(int j = 0; j<width; ++j) {
            int colorValue = resultImageData[i*width+j];
            qInfo() << colorValue;
            QRgb color = qRgb(colorValue, colorValue, colorValue);
            resultImage.setPixel(j, i, color);
        }
    }
    qInfo() << "BANANA5";

    delete[] resultImageData;

    //TODO: DELETE VECTOR?


    this->afterPixmap = QPixmap::fromImage(resultImage);
    afterScene = new QGraphicsScene(this);
    afterScene->addPixmap(this->afterPixmap);
    afterScene->setSceneRect(this->afterPixmap.rect());
    this->ui->afterGraphicsView->setScene(this->afterScene);

    qInfo() << "DONE";

}

double *ResultViewer::FFTWCompute(const uchar *input, int size)
{
    const int arraySize = size*size;

    double *in = new double[arraySize];
    double *out = new double[arraySize];

    for(int i = 0; i<arraySize; ++i) {
        in[i] = (double)(input[i]);
        qInfo() << "Input: " <<in[i];
    }

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(size, size, &in[0], &out[0], FFTW_REDFT10, FFTW_REDFT10, FFTW_ESTIMATE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);

    delete[] in;
    return out;
}

uchar *ResultViewer::iFFTWCompute(const double *input, int size)
{
    const int arraySize = size*size;

    uchar *out = new uchar[arraySize];

    double *tempOut = new double[arraySize];
    double *in = new double[arraySize];

    for(int i = 0; i<arraySize; ++i) {
          in[i] = (double)input[i];
      }

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(size, size, &in[0], &tempOut[0], FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    fftw_cleanup();

    double scaleFactor = 4*arraySize;

    for(int i = 0; i<arraySize; ++i) {
        double scaledValue = tempOut[i]/ scaleFactor;
        if (scaledValue < 0) {scaledValue = 0;}
        if (scaledValue > 255) {scaledValue = 255;}
        out[i] = (int)floor(scaledValue);
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
