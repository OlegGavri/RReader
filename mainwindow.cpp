#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

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
