#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsView>

#include <poppler/qt5/poppler-qt5.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QGraphicsScene * scene = new QGraphicsScene();
    scene->addText("This text");
    QGraphicsView * view = new QGraphicsView(scene);

    setCentralWidget(view);
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

        Poppler::Page * page = document->page(0);
        if(page == nullptr)
        {
            QMessageBox::critical(this, tr("Open page error"), tr("Open page error"));
            return;
        }

        QImage image = page->renderToImage();
        if(image.isNull())
        {
            qDebug() << "Error: image is null";
            return;
        }

        QGraphicsView * view = static_cast<QGraphicsView*>(centralWidget());
        QGraphicsScene * scene = view->scene();

        scene->addPixmap(QPixmap::fromImage(image));

        delete page;
        delete document;
    }
}
