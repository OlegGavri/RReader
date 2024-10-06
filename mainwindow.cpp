#include <stdexcept>

#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsView>
#include <QSettings>
#include <QtDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pagegraphicsitem.h"
#include "documents.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add separators for recent files list
    ui->menuFile->insertSeparator(ui->actionExit);
    recentFileSeparator = ui->menuFile->insertSeparator(ui->actionExit);

    addPageNumSpinBox();
    addZoomSpinBox();

    QGraphicsView * view = ui->graphicsView;
    QTreeView * treeViewContent = ui->treeViewContent;
    view->setDragMode(QGraphicsView::ScrollHandDrag);

    // Set Documents tab bar settings
    QTabBar * tabBar = ui->tabBarDocuments;
    tabBar->setTabsClosable(true);
    tabBar->setMovable(true);

    // Receive document scrolling signal for tracking current page number and etc.
    verticalScrollBar = view->verticalScrollBar();

    // For each currently open documents create Tab bar, select current and
    // seting widgets to it.
    if(Documents::getCount() > 0)
    {
        for(int i = 0; i < Documents::getCount(); i++)
        {
            Document & doc = Documents::getDocument(i);
            tabBar->addTab(doc.getName());
        }

        Document & doc = Documents::getCurrent();
        QGraphicsScene * scene = doc.getScene();
        ContentsItemModel * model =doc.getContentItemModel();

        view->setScene(scene);
        treeViewContent->setModel(model);

        enableNavigations();
    }

    setRecentDocuments(Documents::getRecentDocuments());

    // Connect signals
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::closeAllWindows, Qt::QueuedConnection);
    connect(tabBar, &QTabBar::tabCloseRequested, this, &MainWindow::tabBarDocuments_tabCloseRequested);
    connect(tabBar, &QTabBar::tabMoved, this, &MainWindow::tabBardDocument_tabMoved);

    enableVerticalScrollBarSignal();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addZoomSpinBox()
{
    spinBoxZoom = new QSpinBox(this);

    spinBoxZoom->setObjectName("spinBoxZoom");
    spinBoxZoom->setEnabled(false);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(spinBoxZoom->sizePolicy().hasHeightForWidth());
    spinBoxZoom->setSizePolicy(sizePolicy);
    spinBoxZoom->setMinimumSize(QSize(0, 0));
    spinBoxZoom->setMinimum(1);
    spinBoxZoom->setMaximum(1000);

    ui->toolBar->insertWidget(ui->actionZoomOut, spinBoxZoom);

    connect(spinBoxZoom, SIGNAL(editingFinished()), this, SLOT(spinBoxZoom_editingFinished()));
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

void MainWindow::on_actionOpen_triggered(bool)
{
    QString lastOpenDir = Documents::getLastOpenDir();
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        lastOpenDir,
        "PDF documents(*.pdf *.djvu)");

    if(!fileName.isEmpty())
    {
        try{
            Document & doc = Documents::open(fileName);
            int newTabIndex = Documents::getCurrentNumber();

            QGraphicsView * view = ui->graphicsView;
            QTreeView * treeViewContent = ui->treeViewContent;
            QTabBar * tabBar = ui->tabBarDocuments;
            tabBar->insertTab(newTabIndex, doc.getName());
            tabBar->setCurrentIndex(newTabIndex);

            QGraphicsScene * scene = doc.getScene();
            QAbstractItemModel * contentModel = doc.getContentItemModel();
            int currentPage = doc.getCurrentPage();

            view->setScene(scene);
            treeViewContent->setModel(contentModel);

            // Set page spin box value and limits
            spinBoxPageNum->setValue(currentPage);
            spinBoxPageNum->setMaximum(doc.getPageNumber());

            showPage(currentPage);

            setRecentDocuments(Documents::getRecentDocuments());

            enableNavigations();
            ui->actionClose->setEnabled(true);
        } catch (runtime_error & e) {
            qCritical() << "Open file " << fileName << " error: " << e.what();
            QMessageBox::critical(this, "Error", e.what());
        }
    }
}

void MainWindow::on_actionClose_triggered(bool)
{
    //
    // Close current tab and switch to next. If closed tab is a last tab, switch to previouse
    //
    QGraphicsView * view = ui->graphicsView;
    QTreeView * treeViewContent = ui->treeViewContent;
    QTabBar * tabBar = ui->tabBarDocuments;

    QSignalBlocker blocker(tabBar);

    int docNum = Documents::getCurrentNumber();
    tabBar->removeTab(docNum);

    Documents::closeCurrent();

    if(Documents::getCount() == 0)
    {
        view->setScene(nullptr);
        treeViewContent->setModel(nullptr);
        disableNavigations();
        ui->actionClose->setEnabled(false);
    }
    else
    {
        Document & doc = Documents::getCurrent();
        tabBar->removeTab(docNum);

        QGraphicsScene * scene = doc.getScene();
        QAbstractItemModel * contentModel = doc.getContentItemModel();
        int currentPage = doc.getCurrentPage();


        view->setScene(scene);
        treeViewContent->setModel(contentModel);

        // Set page spin box value and limits
        spinBoxPageNum->setValue(currentPage);
        spinBoxPageNum->setMaximum(doc.getPageNumber());

        showPage(currentPage);
    }
}

void MainWindow::on_actionGoFirst_triggered(bool)
{
    QSignalBlocker bl(spinBoxPageNum);
    spinBoxPageNum->setValue(1);

    Document & doc = Documents::getCurrent();
    doc.setCurrentPage(0);
    showPage(doc.getCurrentPage());
}

void MainWindow::on_actionGoPrev_triggered(bool)
{
    QSignalBlocker bl(spinBoxPageNum);

    Document & doc = Documents::getCurrent();
    int currentPage = doc.getCurrentPage();

    if(currentPage > 0)
    {
        currentPage--;
        doc.setCurrentPage(currentPage);
        showPage(currentPage);

        spinBoxPageNum->setValue(currentPage+1);
    }
}

void MainWindow::on_actionGoNext_triggered(bool)
{
    QSignalBlocker bl(spinBoxPageNum);

    Document & doc = Documents::getCurrent();
    int currentPage = doc.getCurrentPage();

    if(currentPage < doc.getPageNumber() - 1)
    {
        currentPage++;
        doc.setCurrentPage(currentPage);
        showPage(currentPage);

        spinBoxPageNum->setValue(currentPage + 1);
    }
}

void MainWindow::on_actionGoLast_triggered(bool)
{
    QSignalBlocker bl(spinBoxPageNum);

    Document & doc = Documents::getCurrent();
    int currentPage = doc.getCurrentPage();

    if(currentPage < doc.getPageNumber() -1 )
    {
        currentPage = doc.getPageNumber() - 1;
        doc.setCurrentPage(currentPage);
        showPage(currentPage);

        spinBoxPageNum->setValue(currentPage + 1);
    }
}

void MainWindow::on_actionContent_triggered(bool checked)
{
    ui->dockWidgetContent->setVisible(checked);
}

void MainWindow::on_treeViewContent_activated(const QModelIndex &index)
{
    QSignalBlocker bl(spinBoxPageNum);

    // User activate item in content. Go to selected content item.
    ContentsItemModel * model = static_cast<ContentsItemModel*>(ui->treeViewContent->model());
    optional<int> page = model->getPageFor(index);

    if(page.has_value())
    {
        Document & doc = Documents::getCurrent();
        doc.setCurrentPage(page.value());
        showPage(page.value());

        spinBoxPageNum->setValue(page.value() + 1);
    }
}

void MainWindow::on_tabBarDocuments_currentChanged(int index)
{
    QSignalBlocker bl0(spinBoxZoom);
    QSignalBlocker bl1(spinBoxPageNum);

    Documents::setCurrent(index);

    Document & doc = Documents::getCurrent();

    QGraphicsScene * scene = doc.getScene();
    ContentsItemModel * contentModel = doc.getContentItemModel();
    int currentPage = doc.getCurrentPage();

    ui->graphicsView->setScene(scene);
    ui->treeViewContent->setModel(contentModel);

    // Since current document was changed and contentMode was changed it is needed to resize it
    // First column("Name") take all aviable size, second(page number) minimum size.
    if(contentModel)
    {
        ui->treeViewContent->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->treeViewContent->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    }

    // Set zoom
    qreal scale = doc.getScale();
    spinBoxZoom->setValue(scale * 100);

    spinBoxPageNum->setValue(currentPage);

    showPage(currentPage);
}

void MainWindow::on_actionZoomIn_triggered(bool)
{
    Document & doc = Documents::getCurrent();
    int currentPage = doc.getCurrentPage();

    doc.zoomIn();
    showPage(currentPage);

    // Update zoom spin box
    qreal scale = doc.getScale();
    int zoom = scale * 100;

    QSignalBlocker bl(spinBoxZoom);
    spinBoxZoom->setValue(zoom);
}

void MainWindow::on_actionZoomOut_triggered(bool)
{
    Document & doc = Documents::getCurrent();
    int currentPage = doc.getCurrentPage();

    doc.zoomOut();
    showPage(currentPage);

    // Update zoom spin box
    qreal scale = doc.getScale();
    int zoom = scale * 100;

    QSignalBlocker bl(spinBoxZoom);
    spinBoxZoom->setValue(zoom);
}

void MainWindow::spinBoxPageNum_editingFinished()
{
    // Go to page pageNum
    const int pageNum = spinBoxPageNum->value() - 1;
    Document & doc = Documents::getCurrent();
    assert(pageNum < doc.getPageNumber());

    doc.setCurrentPage(pageNum);
    showPage(pageNum);
}

void MainWindow::spinBoxZoom_editingFinished()
{
    // Set zoom for current document
    const int zoom = spinBoxZoom->value();
    const qreal scale = (qreal)(zoom) / 100.0;

    Document & doc = Documents::getCurrent();
    doc.setScale(scale);

    showCurrentPage();
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

        Document & doc = Documents::getCurrent();

        doc.setCurrentPage(pageNum);
        spinBoxPageNum->setValue(pageNum + 1);
    }
}

void MainWindow::tabBarDocuments_tabCloseRequested(int index)
{
    QSignalBlocker blZoom(spinBoxZoom);
    QSignalBlocker blPage(spinBoxPageNum);
    QSignalBlocker blTabBar(ui->tabBarDocuments);

    QTabBar * tabBar = ui->tabBarDocuments;

    //
    // Close document with index. If index is current switch to another document
    // (to next document or to previouse if closed document is last in list)
    //
    if(index == Documents::getCurrentNumber())
    {
        // Closed document is a current document
        QGraphicsView * view = ui->graphicsView;
        QTreeView * treeViewContent = ui->treeViewContent;

        if(Documents::getCount() == 1)
        {
            view->setScene(nullptr);
            treeViewContent->setModel(nullptr);
            disableNavigations();
            ui->actionClose->setEnabled(false);

            Documents::close(index);
        }
        else
        {
            view->setScene(nullptr);
            treeViewContent->setModel(nullptr);

            Documents::close(index);

            Document & doc = Documents::getCurrent();

            QGraphicsScene * scene = doc.getScene();
            ContentsItemModel * model = doc.getContentItemModel();
            int page = doc.getCurrentPage();
            int scale = doc.getScale();

            view->setScene(scene);
            treeViewContent->setModel(model);
            spinBoxZoom->setValue(scale * 100.0);
            spinBoxPageNum->setValue(page + 1);
            spinBoxPageNum->setMaximum(doc.getPageNumber());
        }
    }
    else
    {
        Documents::close(index);
    }

    tabBar->removeTab(index);
}

void MainWindow::tabBardDocument_tabMoved(int from, int to)
{
    Documents::swap(from, to);
}

void MainWindow::recentDocumentAction_triggered(bool)
{
    QAction * senderAction = dynamic_cast<QAction*>(sender());
    QString documentPath = senderAction->data().toString();

    QSignalBlocker blocker(spinBoxPageNum);

    try {
        Document & newDoc = Documents::open(documentPath);
        int newTabIndex = Documents::getCurrentNumber();

        QGraphicsView * view = ui->graphicsView;
        QTreeView * treeViewContents = ui->treeViewContent;
        QTabBar * tabBar = ui->tabBarDocuments;

        QGraphicsScene * scene = newDoc.getScene();
        QAbstractItemModel * contentsModel = newDoc.getContentItemModel();

        int currentPage = newDoc.getCurrentPage();

        view->setScene(scene);
        treeViewContents->setModel(contentsModel);
        tabBar->insertTab(newTabIndex, newDoc.getName());
        tabBar->setCurrentIndex(newTabIndex);

        spinBoxPageNum->setValue(currentPage);
        spinBoxPageNum->setMaximum(newDoc.getPageNumber());

        showPage(currentPage);

        setRecentDocuments(Documents::getRecentDocuments());

        enableNavigations();
        ui->actionClose->setEnabled(true);
    } catch(runtime_error & e) {
        qCritical() << "Open file " << documentPath << " error: " << e.what();
        QMessageBox::critical(
            this,
            tr("Error"),
            QString("Open file %1 error (%2)").arg(documentPath).arg(e.what()));
    }
}

void MainWindow::enableNavigations()
{
    spinBoxPageNum->setEnabled(true);
    spinBoxZoom->setEnabled(true);
    ui->actionGoFirst->setEnabled(true);
    ui->actionGoNext->setEnabled(true);
    ui->actionGoPrev->setEnabled(true);
    ui->actionGoLast->setEnabled(true);
    ui->actionZoomIn->setEnabled(true);
    ui->actionZoomOut->setEnabled(true);
}

void MainWindow::disableNavigations()
{
    spinBoxPageNum->setEnabled(false);
    spinBoxZoom->setEnabled(false);
    ui->actionGoFirst->setEnabled(false);
    ui->actionGoNext->setEnabled(false);
    ui->actionGoPrev->setEnabled(false);
    ui->actionGoLast->setEnabled(false);
    ui->actionZoomIn->setEnabled(false);
    ui->actionZoomOut->setEnabled(false);
}

void MainWindow::showCurrentPage()
{
    Document & doc = Documents::getCurrent();
    int page = doc.getCurrentPage();

    showPage(page);
}

void MainWindow::showPage(const int pageNum)
{
    disableVerticalScrollBarSignal();

    Document & document = Documents::getCurrent();
    assert(pageNum < document.getPageNumber());

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

    enableVerticalScrollBarSignal();
}

void MainWindow::enableVerticalScrollBarSignal()
{
    connect(
        verticalScrollBar,
        &QAbstractSlider::valueChanged,
        this,
        &MainWindow::verticalScroll_valueChanged);
}

void MainWindow::disableVerticalScrollBarSignal()
{
    disconnect(
        verticalScrollBar,
        &QAbstractSlider::valueChanged,
        this,
        &MainWindow::verticalScroll_valueChanged);
}

void MainWindow::setRecentDocuments(const QList<QString> list)
{
    QMenu * menuFile = ui->menuFile;

    // Clear recent document list
    for(QAction * action : recentDocumentsAction)
    {
        menuFile->removeAction(action);
        delete action;
    }
    recentDocumentsAction.clear();

    if(!list.empty())
    {
        recentFileSeparator = menuFile->addSeparator();
        // Add actions at the end of File menu
        for (QString docname : list)
        {
            QAction * action = menuFile->addAction(docname);
            action->setData(docname);
            connect(action, &QAction::triggered, this, &MainWindow::recentDocumentAction_triggered);
            recentDocumentsAction.append(action);
        }
    }
    else
    {
        if(recentFileSeparator)
            menuFile->removeAction(recentFileSeparator);
        delete recentFileSeparator;
        recentFileSeparator = nullptr;
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    QMainWindow::closeEvent(event);
}
