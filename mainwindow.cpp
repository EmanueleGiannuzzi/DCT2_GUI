#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_toolButton_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(
                this,
                tr("Open File"),
                "",
                tr("Bitmap (*.bmp)" )
                );
    this->ui->imagePathLineEdit->setText(imagePath);
    loadPreviewImage();
}

void MainWindow::loadPreviewImage()
{
    QString imagePath = this->ui->imagePathLineEdit->text();

    this->imageObject = new QImage();
    this->imageObject->load(imagePath);


    this->image = QPixmap::fromImage(*imageObject);

    scene = new QGraphicsScene(this);
    scene->addPixmap(this->image);
    scene->setSceneRect(this->image.rect());

    this->ui->previewGraphicsView->setScene(this->scene);
    this->ui->previewGraphicsView->fitInView(this->image.rect(), Qt::KeepAspectRatio);

    updateFParam();
}

void MainWindow::updateFParam()
{
    int width = this->image.width();
    int height = this->image.height();
    if(width < height) {
        this->ui->fParamSpinBox->setMaximum(width);
    }
    else {
        this->ui->fParamSpinBox->setMaximum(height);
    }
    updateDParam();
}

void MainWindow::updateDParam()
{
    int max = (2 * this->ui->fParamSpinBox->value())-2;
    this->ui->dParamMaxLabel->setText(QString::number(max));
    this->ui->dParamSlider->setMaximum(max);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   this->ui->previewGraphicsView->fitInView(this->image.rect(), Qt::KeepAspectRatio);
}

void MainWindow::on_fParamSpinBox_valueChanged(int arg1)
{
    updateDParam();
}

void MainWindow::on_dParamSlider_sliderMoved(int position)
{
    this->ui->dParamValueLabel->setText(QString::number(position));
}
