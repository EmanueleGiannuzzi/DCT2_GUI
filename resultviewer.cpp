#include "resultviewer.h"
#include "ui_resultviewer.h"

#include "fftw3.h"

ResultViewer::ResultViewer(const QPixmap *before, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ResultViewer),
    beforePixmap(before)
{
    ui->setupUi(this);

    beforeScene = new QGraphicsScene(this);
    beforeScene->addPixmap(*this->beforePixmap);
    beforeScene->setSceneRect((*this->beforePixmap).rect());
    this->ui->beforeGraphicsView->setScene(this->beforeScene);

    //TODO: DCT2 COSE

  /*this->afterPixmap = QPixmap::fromImage(*after);
    afterScene = new QGraphicsScene(this);
    afterScene->addPixmap(this->afterPixmap);
    afterScene->setSceneRect(this->afterPixmap.rect());
    this->ui->afterGraphicsView->setScene(this->afterScene);
    this->ui->afterGraphicsView->fitInView(this->afterPixmap.rect(), Qt::KeepAspectRatio);*/
}

void ResultViewer::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   this->ui->beforeGraphicsView->fitInView((*this->beforePixmap).rect(), Qt::KeepAspectRatio);
}

void ResultViewer::showEvent(QShowEvent *event)
{
    this->ui->beforeGraphicsView->fitInView((*this->beforePixmap).rect(), Qt::KeepAspectRatio);
}

ResultViewer::~ResultViewer()
{
    delete ui;
    //delete beforePixmap;//Probabilmente viene già eliminatata da MainWindow
    delete beforeScene;
    delete afterScene;
}
