#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsView>
#include <QScrollBar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pagegraphicsitem.h"
#include "contentitemmodel.h"

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
    addPageNumSpinBox();

    QGraphicsScene * scene = new QGraphicsScene();
    QGraphicsView * view = ui->graphicsView;
    view->setScene(scene);

    // Receive document scrolling signal for tracking current page number and etc.
    QScrollBar * verticalScrollBar = view->verticalScrollBar();
    connect(verticalScrollBar, &QAbstractSlider::valueChanged, this, &MainWindow::verticalScroll_valueChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addPageNumSpinBox()
{
    spinBoxPageNum = new QSpinBox(this);

    spinBoxPageNum->setObjectName(QString::fromUtf8("spinBoxPageNum"));
    spinBoxPageNum->setEnabled(false);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(spinBoxPageNum->sizePolicy().hasHeightForWidth());
    spinBoxPageNum->setSizePolicy(sizePolicy);
    spinBoxPageNum->setMinimumSize(QSize(0, 0));
    spinBoxPageNum->setMinimum(1);
    spinBoxPageNum->setMaximum(10000);

    ui->toolBar->insertWidget(ui->actionGoNext, spinBoxPageNum);

    connect(spinBoxPageNum, SIGNAL(editingFinished()), this, SLOT(spinBoxPageNum_editingFinished()));
}

void MainWindow::updateDocumentContent()
{
    QVector<Poppler::OutlineItem> outline = document->outline();
    ContentItemModel * contentItemModel = new ContentItemModel(outline);
    ui->treeViewContent->setModel(contentItemModel);

    // Resize column. First column("Name") take all aviable size, second(page number) minimum size.
    ui->treeViewContent->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeViewContent->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
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
        spinBoxPageNum->setMaximum(numPages);

        // Update scene height before go to page. Without this showPage not
        // working in this place.
        scene->height();
        showPage(0);

        updateDocumentContent();
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

void MainWindow::on_actionGoFirst_triggered(bool)
{
    goFirstPage();
}

void MainWindow::on_actionGoPrev_triggered(bool)
{
    goPrevPage();
}

void MainWindow::on_actionGoNext_triggered(bool)
{
    goNextPage();
}

void MainWindow::on_actionGoLast_triggered(bool)
{
    goLastPage();
}

void MainWindow::on_actionContent_triggered(bool checked)
{
    ui->dockWidgetContent->setVisible(checked);
}

void MainWindow::on_treeViewContent_activated(const QModelIndex &index)
{
    // User activate item in content. Go to selected content item.
    ContentItemModel * model = static_cast<ContentItemModel*>(ui->treeViewContent->model());
    int page = model->getPageFor(index);
    showPage(page);
}

void MainWindow::spinBoxPageNum_editingFinished()
{
    // Go to page pageNum
    const int pageNum = spinBoxPageNum->value() - 1;
    showPage(pageNum);
}

void MainWindow::verticalScroll_valueChanged(int)
{
    //
    // Update current page number
    //

    // Find item at the center of view
    QGraphicsView * view = ui->graphicsView;
    QSize viewSize = view->viewport()->size();
    int viewWidth = viewSize.width();
    int viewHeight = viewSize.height();

    QGraphicsItem * item = view->itemAt(viewWidth/2, viewHeight/2);

    if(item != nullptr)
    {
        const QSignalBlocker blocker(spinBoxPageNum);

        PageGraphicsItem * pageItem = static_cast<PageGraphicsItem*>(item);
        const int pageNum = pageItem->getPageNum();

        _currentPage = pageNum;
        spinBoxPageNum->setValue(pageNum);
    }
}

void MainWindow::enableNavigations()
{
    spinBoxPageNum->setEnabled(true);
    ui->actionGoFirst->setEnabled(true);
    ui->actionGoNext->setEnabled(true);
    ui->actionGoPrev->setEnabled(true);
    ui->actionGoLast->setEnabled(true);
}

void MainWindow::disableNavigations()
{
    spinBoxPageNum->setEnabled(false);
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

    _currentPage = pageNum;
}

void MainWindow::goFirstPage()
{
    showPage(0);
}

void MainWindow::goPrevPage()
{
    const int currPage = currentPage();
    if(currPage == 0)
        return;

    showPage(currPage - 1);
}

void MainWindow::goNextPage()
{
    const int currPage = currentPage();

    if(currPage == documentPageNumber() - 1)
        return;

    showPage(currPage + 1);
}

void MainWindow::goLastPage()
{
    showPage(documentPageNumber() - 1);
}

int MainWindow::currentPage() const
{
    return _currentPage;
}

int MainWindow::documentPageNumber() const
{
    return document->numPages();
}
