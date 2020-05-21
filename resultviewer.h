#ifndef RESULTVIEWER_H
#define RESULTVIEWER_H

#include <QMainWindow>

namespace Ui {
class ResultViewer;
}

class ResultViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ResultViewer(QImage *before, QImage *after, QWidget *parent = nullptr) : beforeImage(before), afterImage(after) {
        //TODO: VISUALIZZA IMMAGINI
    };
    ~ResultViewer();

private:
    Ui::ResultViewer *ui;

    QImage *beforeImage;
    QPixmap beforePixmap;
    QImage *afterImage;
    QPixmap afterPixmap;
};

#endif // RESULTVIEWER_H
