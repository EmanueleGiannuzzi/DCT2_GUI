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

    static double *FFTWCompute(const uchar *input, int size);
    static uchar *inverseFFTWCompute(const double *input, int size);

    static double *FFTWCompute1D(const double *input, int size)
    {
        double *in = new double[size];
        double *out = nullptr;

        for(int i = 0; i<size; ++i) {
            in[i] = input[i];
        }

        fftw_plan my_plan;
        my_plan = fftw_plan_r2r_1d(size, in, out, FFTW_REDFT10, FFTW_ESTIMATE);
        fftw_execute(my_plan);
        fftw_destroy_plan(my_plan);
        fftw_cleanup();

        delete[] in;
        return out;
    }

    static double *FFTWCompute2D(const uchar *input, int size)
    {
        const int arraySize = size*size;

        double *in = new double[arraySize];

        for(int i = 0; i<arraySize; ++i) {
            in[i] = (double)(input[i]/255.0);
        }

        double *newMat = new double[arraySize];
        for(int i = 0; i<size; ++i) {
            double *row = &in[i];
            double *resultRow = FFTWCompute1D(row, size);
            for(int j = 0; j<size; ++j) {
                newMat[i*size+j] = resultRow[j];
            }
        }

        for(int j = 0; j<size; ++j) {
            double col[size];
            for(int i = 0; i<size; ++i) {
                col[j] = newMat[i*size+j];
            }
            double *resultCol = FFTWCompute1D(col, size);
            for(int k = 0; k<size; ++k) {
                newMat[k*size+j] = resultCol[k];
            }
        }

        delete[] in;
        return newMat;
    }

    static double *iFFTWCompute1D(const double *input, int size)
    {
        double *out = new double[size];
        double *in = new double[size];

        for(int i = 0; i<size; ++i) {
            in[i] = (double)input[i];
        }

        fftw_plan my_plan;
        my_plan = fftw_plan_r2r_1d(size, in, out, FFTW_REDFT01, FFTW_ESTIMATE);
        fftw_execute(my_plan);
        fftw_destroy_plan(my_plan);
        fftw_cleanup();

        double scaleFactor = size << 1;
        for(int i = 0; i<size; ++i) {
            out[i] /= scaleFactor;
        }

        delete[] in;

        return out;
    }

    static uchar *iFFTWCompute2D(const double *input, int size)
    {
        const int arraySize = size*size;

        uchar *out = new uchar[arraySize];

        double *tempOut = new double[arraySize];
        double *in = new double[arraySize];

        for(int i = 0; i<arraySize; ++i) {
            in[i] = (double)input[i];
        }

        double newMat[arraySize];
        for(int i = 0; i<size; ++i) {
            double *row = &in[i];
            double *resultRow = iFFTWCompute1D(row, size);
            for(int j = 0; j<size; ++j) {
                newMat[i*size+j] = resultRow[j];
            }
        }

        for(int j = 0; j<size; ++j) {
            double col[size];
            for(int i = 0; i<size; ++i) {
                col[j] = newMat[i*size+j];
            }
            double *resultCol = iFFTWCompute1D(col, size);
            for(int k = 0; k<size; ++k) {
                newMat[k*size+j] = resultCol[k];
            }
        }

        for(int i = 0; i<arraySize; ++i) {
            double trimmedValue = qMax(qMin(newMat[i], 1.0), 0.0);
            out[i] = (uchar)(trimmedValue * 255);
        }

        delete[] in;
        delete[] tempOut;

        return out;
    }

};

#endif // RESULTVIEWER_H
