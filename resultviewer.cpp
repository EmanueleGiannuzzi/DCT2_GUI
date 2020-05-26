#include "resultviewer.h"
#include "ui_resultviewer.h"

#include <fftw3.h>
#include <QDebug>
#include <QVector>

#include "imagefilter.h"


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

    const int width = this->beforeImage->width();
    const int height = this->beforeImage->height();
    const int depth = this->depth();
    const int arraySize = width*height;

    qInfo() << width << " " << height << " " << depth;

    const int dimRow = (int)(height / F);
    const int dimCol = (int)(width / F);

    const int fArraySize = F*F;

    qInfo() << "BANANA";

    //    QVector<uchar*> matrices;
    //    for(int k = 0; k+F <= height; k+=F) {
    //        for(int l = 0; l+F <= width; l+=F) {
    //            uchar* matrix = new uchar[fArraySize];
    //            for(int i = 0; i < F; ++i) {
    //                for(int j = 0; j < F; ++j) {
    //                    matrix[i*F+j] = input[(k+i)*width+(l+j)];
    //                }
    //            }
    //            matrices.push_back(matrix);
    //        }
    //    }
    double **compressed = new double*[dimCol*dimRow];
    for(int k = 0; k < dimRow; ++k) {
        for(int l = 0; l < dimCol; ++l) {
            double* matrix = new double[fArraySize];
//            qInfo() << k <<  " " << l;
            for(int i = 0; i < F; ++i) {
                for(int j = 0; j < F; ++j) {
                    //qInfo() << k <<  " " << l << " " << i <<  " " << j;
                    matrix[i*F+j] = (double)qGray(beforeImage->pixel(((l*F)+i), ((k*F)+j)));//input[((k+i)*width)+(l+j)];
                }
            }

            //ImageFilter::image image(matrix, F, F);
            //ImageFilter::filter_image(image, true, ResultViewer::frequency_modifier);

            qInfo() << k*dimCol+l;
            compressed[k*dimCol+l] = matrix;
            //compressed.push_back(matrix);
        }
    }
    qInfo() << "BANANA1";

    //    QVector<uchar*> compressed;//(dimRow * dimCol);
    //    for(uchar* matrix : matrices) {
    //        double *arrayResult = ResultViewer::FFTWCompute(matrix, F);
    //        for(int k = 0; k < F; ++k) {
    //            for(int l = 0; l < F; ++l) {
    //                if (k+l >= D){
    //                    arrayResult[k+dimCol*l] = 0.0;
    //                }
    //            }
    //        }
    //        uchar *inverseResult = ResultViewer::iFFTWCompute(arrayResult, F);
    //        compressed.push_back(inverseResult);
    //    }

    qInfo() << "BANANA2";

    uchar *resultImageData = new uchar[arraySize];

    for(int k = 0; k < dimRow; ++k) {
        for(int l = 0; l < dimCol; ++l) {
            const double* matrixDouble = compressed[k*dimRow+l];//compressed.at(k*dimRow+l);

            int row = k*F;
            int col = l*F;

            qInfo() << row << " " << col << " " << k <<  " " << l;
            for(int i = 0; i < F; ++i) {
                for(int j = 0; j < F; ++j) {
//                    qInfo() << i << " " << j << ((row+i)*width)+(col+j);
                    resultImageData[((row+i)*width)+(col+j)] = (uchar)floor(matrixDouble[i*F+j]);
                }
            }
        }
    }

    qInfo() << "BANANA3";

    qInfo() << "A " << F << " " << dimCol << " " <<  F*dimCol;
//    for(int i = 0; i < width; ++i) {
//        for(int j = F*dimRow; j < height; ++j) {
//            resultImageData[i*width+j] = qGray(beforeImage->pixel(i, j));//input[i*width+j];
//        }
//    }
    qInfo() << "B " << F << " " << dimRow << " " <<  F*dimRow;
//    for(int i = F*dimCol; i < height; ++i) {
//        for(int j = 0; j < dimRow*F; ++j) {
//            resultImageData[i*width+j] = qGray(beforeImage->pixel(j, i));//input[i*width+j];
//        }
//    }
    qInfo() << "BANANA4";

    QImage resultImage = QImage(width, height, QImage::Format_Grayscale8);

    for(int i = 0; i<width; ++i) {
        for(int j = 0; j<height; ++j) {
            int colorValue = resultImageData[i*width+j];
            QRgb color = qRgb(colorValue, colorValue, colorValue);
            resultImage.setPixel(i, j, color);
        }
    }
    qInfo() << "BANANA5";

    //    delete[] resultImageData;

    //TODO: DELETE VECTOR?
    //    for(uchar* matrix : matrices) {
    //        delete[] matrix;
    //    }
//    for(double* matrix : compressed) {
//        delete[] matrix;
//    }

    delete[] compressed;

    this->afterPixmap = QPixmap::fromImage(resultImage);
    afterScene = new QGraphicsScene(this);
    afterScene->addPixmap(this->afterPixmap);
    afterScene->setSceneRect(this->afterPixmap.rect());
    this->ui->afterGraphicsView->setScene(this->afterScene);


    qInfo() << "DONE";

}

void ResultViewer::frequency_modifier(double *frequency_space, double *image_space)
{

}

double *ResultViewer::FFTWCompute(const uchar *input, int size)
{
    const int arraySize = size*size;

    double *in = new double[arraySize];
    double *out = new double[arraySize];

    for(int i = 0; i<arraySize; ++i) {
        in[i] = (double)(input[i]);
    }

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(size, size, &in[0], &out[0], FFTW_REDFT10, FFTW_REDFT10, FFTW_MEASURE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    fftw_cleanup();

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
        out[i] = (uchar)floor(scaledValue);
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
