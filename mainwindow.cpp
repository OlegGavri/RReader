#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsView>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pagegraphicsitem.h"

//
// Constants
//

// Gap beetween pages
constexpr int PageGap = 10;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    document(nullptr)
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
        document = Poppler::Document::load(fileName);
        if(document == nullptr || document->isLocked())
        {
            QMessageBox::critical(this, tr("Open file error"), tr("Open file error"));
            delete document;
            return;
        }

        document->setRenderHint(Poppler::Document::TextAntialiasing);

        QGraphicsView * view = ui->graphicsView;
        QGraphicsScene * scene = view->scene();

        const int numPages = document->numPages();

        int y = 0;
        for(int i = 0; i < numPages; i++)
        {
            PageGraphicsItem * item = new PageGraphicsItem(document, i);
            item->setPos(0, y);
            scene->addItem(item);

            int h = item->boundingRect().height();
            y += h + PageGap;
        }

        // Set number of pages in spin box
        QSpinBox * spinBoxPageNum = ui->spinBoxPageNum;
        spinBoxPageNum->setMaximum(numPages);

        // Update scene height before go to page. Without this showPage not
        // working in this place.
        scene->height();
        showPage(0);

        enableNavigations();
    }
}

void MainWindow::on_actionClose_triggered(bool)
{
    QGraphicsView * view = ui->graphicsView;
    QGraphicsScene * scene = view->scene();

    scene->clear();

    disableNavigations();

    delete document;
    document = nullptr;
}

void MainWindow::on_spinBoxPageNum_editingFinished()
{
    // Go to page pageNum
    const int pageNum = ui->spinBoxPageNum->value() - 1;
    showPage(pageNum);
}

void MainWindow::enableNavigations()
{
    ui->toolButtonFirstPage->setEnabled(true);
    ui->toolButtonPrevPage->setEnabled(true);
    ui->toolButtonNextPage->setEnabled(true);
    ui->toolButtonLastPage->setEnabled(true);
    ui->spinBoxPageNum->setEnabled(true);

    ui->actionGoFirst->setEnabled(true);
    ui->actionGoNext->setEnabled(true);
    ui->actionGoPrev->setEnabled(true);
    ui->actionGoLast->setEnabled(true);
}

void MainWindow::disableNavigations()
{
    ui->toolButtonFirstPage->setEnabled(false);
    ui->toolButtonPrevPage->setEnabled(false);
    ui->toolButtonNextPage->setEnabled(false);
    ui->toolButtonLastPage->setEnabled(false);
    ui->spinBoxPageNum->setEnabled(false);

    ui->actionGoFirst->setEnabled(false);
    ui->actionGoNext->setEnabled(false);
    ui->actionGoPrev->setEnabled(false);
    ui->actionGoLast->setEnabled(false);
}

void MainWindow::showPage(const int pageNum)
{
    assert(pageNum < document->numPages());

    QGraphicsView * view = ui->graphicsView;
    QGraphicsScene * scene = view->scene();
    QList<QGraphicsItem*> items = scene->items(Qt::AscendingOrder);
    PageGraphicsItem * showedItem = static_cast<PageGraphicsItem*>(items[pageNum]);

    // Show the page so that the top of page matches top of view
    qreal yItemScene = showedItem->sceneYTop();          /* Page y coordinate in scene */
    int viewHeight = view->viewport()->size().height(); /* Height of graphicsView viewport */

    // Map viewHeith to scene coordinate
    QPointF p0 = view->mapToScene(0,0);
    QPointF p1 = view->mapToScene(0, viewHeight);
    qreal yOffsetScene = p1.y() - p0.y();

    // Center position that the top of page matches top of view
    int centerPos = yItemScene + yOffsetScene / 2;
    view->centerOn(0, centerPos);
}
