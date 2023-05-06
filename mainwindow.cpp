#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsView>

#include <poppler/qt5/poppler-qt5.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

//
// Constants
//

// Pages border width
const int PageBorderWidth = 2;

// Gap beetween pages
constexpr int PageGap = 10;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QGraphicsScene * scene = new QGraphicsScene();
    QGraphicsView * view = ui->graphicsView;
    view->setScene(scene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered(bool)
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        QDir::homePath(),
        "PDF documents(*.pdf)");

    if(!fileName.isEmpty())
    {
        Poppler::Document * document = Poppler::Document::load(fileName);
        if(document == nullptr || document->isLocked())
        {
            QMessageBox::critical(this, tr("Open file error"), tr("Open file error"));
            delete document;
            return;
        }

        document->setRenderHint(Poppler::Document::TextAntialiasing);

        QGraphicsView * view = ui->graphicsView;
        QGraphicsScene * scene = view->scene();

        int numPages = document->numPages();

        int y = 0;
        for(int i = 0; i < numPages; i++)
        {
            Poppler::Page * page = document->page(i);
            if(page == nullptr)
            {
                qDebug() << "Page get error";
                return;
            }

            QImage image = page->renderToImage();
            if(image.isNull())
            {
                qDebug() << "Error: image is null";
                return;
            }

            int h = image.height();

            QGraphicsPixmapItem * item = scene->addPixmap(QPixmap::fromImage(image));
            item->setPos(0, y);
            QRectF rect = item->boundingRect();

            pageItems.push_back(item);

            QPen borderPen = QPen();
            borderPen.setWidth(PageBorderWidth);
            QGraphicsItem * rectItem = scene->addRect(rect, borderPen);
            rectItem->setPos(0, y);
            y += h + PageGap;

            delete page;
        }

        delete document;
    }
}

void MainWindow::on_spinBoxPageNum_editingFinished()
{
    // Go to page pageNum
    const int pageNum = ui->spinBoxPageNum->value();

    QGraphicsView * view = ui->graphicsView;
    QGraphicsItem * showedItem = pageItems[pageNum];

    // Show the page so that the top of page matches top of view
    qreal yItemScene = showedItem->pos().y();           /* Page y coordinate in scene */
    int viewHeight = view->viewport()->size().height(); /* Height of graphicsView viewport */

    // Map viewHeith to scene coordinate
    QPointF p0 = view->mapToScene(0,0);
    QPointF p1 = view->mapToScene(0, viewHeight);
    qreal yOffsetScene = p1.y() - p0.y();

    // Center position that the top of page matches top of view
    int centerPos = yItemScene + yOffsetScene / 2;
    view->centerOn(0, centerPos);
}
