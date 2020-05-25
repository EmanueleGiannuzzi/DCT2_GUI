#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete resultWindow;
    delete previewImage;
    delete scene;
}


void MainWindow::on_toolButton_clicked()
{
    QString imageDirectory = QStandardPaths::locate(QStandardPaths::PicturesLocation, "", QStandardPaths::LocateDirectory);
    QString imagePath = QFileDialog::getOpenFileName(
                this,
                tr("Open BMP Image"),
                imageDirectory,
                tr("Bitmap (*.bmp)")
                );
    this->ui->imagePathLineEdit->setText(imagePath);
    loadPreviewImage();
}

void MainWindow::loadPreviewImage()
{
    QString imagePath = this->ui->imagePathLineEdit->text();

    this->previewImage = new QImage();
    this->previewImage->load(imagePath);

    this->previewPixmap = QPixmap::fromImage(*previewImage);

    scene = new QGraphicsScene(this);
    scene->addPixmap(this->previewPixmap);
    scene->setSceneRect(this->previewPixmap.rect());

    this->ui->previewGraphicsView->setScene(this->scene);
    this->ui->previewGraphicsView->fitInView(this->previewPixmap.rect(), Qt::KeepAspectRatio);

    updateFParam();

    this->ui->runButton->setEnabled(true);
    this->ui->dParamSlider->setEnabled(true);
    this->ui->fParamSpinBox->setEnabled(true);
    this->ui->fParamSpinBox->setValue(1);
}

void MainWindow::updateFParam()
{
    int width = this->previewPixmap.width();
    int height = this->previewPixmap.height();

    this->ui->fParamSpinBox->setMaximum(qMin(width, height));
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

   this->ui->previewGraphicsView->fitInView(this->previewPixmap.rect(), Qt::KeepAspectRatio);
}

void MainWindow::on_fParamSpinBox_valueChanged(int arg1)
{
    updateDParam();
}

void MainWindow::on_dParamSlider_rangeChanged(int min, int max)
{
    if(this->ui->dParamSlider->value() > max) {
        this->ui->dParamSlider->setValue(max);
    }
}

void MainWindow::on_dParamSlider_valueChanged(int value)
{
    this->ui->dParamValueLabel->setText(QString::number(value));
}

void MainWindow::on_runButton_clicked()
{
    this->resultWindow = new ResultViewer(this->previewImage, this->ui->fParamSpinBox->value(), this->ui->dParamSlider->value(), this);
    resultWindow->show();
}

void MainWindow::on_imagePathLineEdit_returnPressed()
{
    loadPreviewImage();
}
