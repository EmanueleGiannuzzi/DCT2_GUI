#ifndef RESULTVIEWER_H
#define RESULTVIEWER_H

#include <QMainWindow>

#include <QGraphicsScene>

namespace Ui {
class ResultViewer;
}

class ResultViewer : public QMainWindow
{
    Q_OBJECT

public:
    ResultViewer(const QPixmap *before, QWidget *parent = nullptr);
    ~ResultViewer();

private:
    Ui::ResultViewer *ui;

    const QPixmap *beforePixmap;
    QPixmap afterPixmap;
    QGraphicsScene *beforeScene;
    QGraphicsScene *afterScene;

};

#endif // RESULTVIEWER_H
