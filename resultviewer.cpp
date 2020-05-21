#include "resultviewer.h"
#include "ui_resultviewer.h"

ResultViewer::ResultViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ResultViewer)
{
    ui->setupUi(this);
}

ResultViewer::~ResultViewer()
{
    delete ui;
}
