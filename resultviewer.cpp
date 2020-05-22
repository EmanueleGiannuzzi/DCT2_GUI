#include "resultviewer.h"
#include "ui_resultviewer.h"

#include <fftw3.h>
#include <QDebug>

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



    double *arrayResult = ResultViewer::FFTWCompute(this->beforeImage->bits(), width, height);



    delete arrayResult;

  /*this->afterPixmap = QPixmap::fromImage(*after);
    afterScene = new QGraphicsScene(this);
    afterScene->addPixmap(this->afterPixmap);
    afterScene->setSceneRect(this->afterPixmap.rect());
    this->ui->afterGraphicsView->setScene(this->afterScene);
    this->ui->afterGraphicsView->fitInView(this->afterPixmap.rect(), Qt::KeepAspectRatio);*/
}

double *ResultViewer::FFTWCompute(const uchar *input, int width, int height)
{
    int arraySize = width*height;
    double in[arraySize], *out;
    out = new double[width*height];

    std::copy(input, input + arraySize, in);

    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_2d(height, width, in, out, FFTW_REDFT10, FFTW_REDFT10, FFTW_ESTIMATE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    return out;
}

/*double *ResultViewer::inverseFFTWCompute(const double *input)
{
    double *in, *out;
    int arraySize = width*height;
    out = new double[arraySize];
    in = (double *)input;
    fftw_plan my_plan;
    my_plan = fftw_plan_r2r_1d(arraySize, in, out, FFTW_REDFT10, FFTW_ESTIMATE);
    fftw_execute(my_plan);
    fftw_destroy_plan(my_plan);
    return out;
}*/

void ResultViewer::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   this->ui->beforeGraphicsView->fitInView(this->beforePixmap.rect(), Qt::KeepAspectRatio);
}

void ResultViewer::showEvent(QShowEvent *event)
{
    this->ui->beforeGraphicsView->fitInView(this->beforePixmap.rect(), Qt::KeepAspectRatio);
}

ResultViewer::~ResultViewer()
{
    delete ui;
    //delete beforeImage;//Probabilmente viene già eliminatata da MainWindow
    delete beforeScene;
    delete afterScene;
}
