#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imageviewer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ImageViewer* previewViewer = new ImageViewer();
    //ui->previewLayout->addWidget(previewViewer);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

