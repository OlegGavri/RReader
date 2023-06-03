#include <stdexcept>

#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsView>
#include <QScrollBar>
#include <QSettings>
#include <QtDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pagegraphicsitem.h"
#include "pdfcontentitemmodel.h"

using namespace std;

//
// Constants
//

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addPageNumSpinBox();

    QGraphicsScene * scene = new QGraphicsScene();
    QGraphicsView * view = ui->graphicsView;
    view->setScene(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag);

    // Receive document scrolling signal for tracking current page number and etc.
    QScrollBar * verticalScrollBar = view->verticalScrollBar();
    connect(verticalScrollBar, &QAbstractSlider::valueChanged, this, &MainWindow::verticalScroll_valueChanged);

    restoreSettings();
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

void MainWindow::on_actionOpen_triggered(bool)
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        lastOpenFileDir.isEmpty() ? QDir::homePath() : lastOpenFileDir,
        "PDF documents(*.pdf *.djvu)");

    if(!fileName.isEmpty())
    {
        try{
            openDocument(fileName);
        } catch (runtime_error & e) {
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

    const int docsNum = openDocuments.size();

    if(docsNum == 0)
    {
        return;
    }

    int closedDocIndex = currentDocumentIndex;

    Document * closedDocument = openDocuments[closedDocIndex];

    if(docsNum == 1)
    {
        view->setScene(nullptr);
        treeViewContent->setModel(nullptr);
        disableNavigations();

        openDocuments.clear();
        delete closedDocument;
        tabBar->removeTab(0);

        currentDocumentIndex = 0;
    }
    else
    {
        int switchedDocIndex = (closedDocIndex == docsNum - 1) ? closedDocIndex - 1 : closedDocIndex + 1;
        Document * switchedDocument = openDocuments[switchedDocIndex];

        QGraphicsScene * scene = switchedDocument->getScene();
        QAbstractItemModel * contentModel = switchedDocument->getContentItemModel();
        int currentPage = switchedDocument->getCurrentPage();

        view->setScene(scene);
        treeViewContent->setModel(contentModel);
        showPage(currentPage);

        tabBar->removeTab(closedDocIndex);

        openDocuments.removeAt(closedDocIndex);
        delete closedDocument;

        currentDocumentIndex = openDocuments.indexOf(switchedDocument);
    }
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
    ContentsItemModel * model = static_cast<ContentsItemModel*>(ui->treeViewContent->model());
    optional<int> page = model->getPageFor(index);

    if(page.has_value())
        showPage(page.value());
}

void MainWindow::on_tabBarDocuments_currentChanged(int index)
{
    // Change current document, scene and content tree
    currentDocumentIndex = index;
    Document * currentDocument = openDocuments[index];

    QGraphicsScene * scene = currentDocument->getScene();
    ContentsItemModel * contentModel = currentDocument->getContentItemModel();
    int currentPage = currentDocument->getCurrentPage();

    ui->graphicsView->setScene(scene);
    ui->treeViewContent->setModel(contentModel);
    // Resize column. First column("Name") take all aviable size, second(page number) minimum size.
    ui->treeViewContent->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeViewContent->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    showPage(currentPage);
}

void MainWindow::on_actionZoomIn_triggered(bool)
{
    Document * currentDoc = getCurrentDocument();
    int currentPage = currentDoc->getCurrentPage();

    currentDoc->zoomIn();
    showPage(currentPage);
}

void MainWindow::on_actionZoomOut_triggered(bool)
{
    Document * currentDoc = getCurrentDocument();
    int currentPage = currentDoc->getCurrentPage();

    currentDoc->zoomOut();
    showPage(currentPage);
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

        Document * document = getCurrentDocument();

        document->setCurrentPage(pageNum);
        spinBoxPageNum->setValue(pageNum);
    }
}

QString MainWindow::getFileBaseName(const QString fileName)
{
    QFileInfo fileInfo(fileName);
    return fileInfo.baseName();
}

QString MainWindow::getFileDir(const QString fileName)
{
    QFileInfo fileInfo(fileName);
    return fileInfo.absoluteFilePath();
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
    Document * document = getCurrentDocument();
    assert(pageNum < document->getPageNumber());

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

    document->setCurrentPage(pageNum);
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
    Document * document = getCurrentDocument();
    return document->getCurrentPage();
}

int MainWindow::documentPageNumber() const
{
    Document * document = getCurrentDocument();
    return document->getPageNumber();
}

void MainWindow::addTab(const QString fileName)
{
    // If tab bar don't contain tab, create it. Set base file name in tab, and full name
    // in tooltip.
    QTabBar * tabBar = ui->tabBarDocuments;
    QSignalBlocker blocker(tabBar);

    int index = tabBar->addTab("");

    tabBar->setTabText(index, getFileBaseName(fileName));
    tabBar->setTabToolTip(index, fileName);
    tabBar->setCurrentIndex(index);
}

Document * MainWindow::getCurrentDocument() const
{
    return openDocuments[currentDocumentIndex];
}

void MainWindow::openDocument(const QString fileName)
{
    Document * document = Document::createDocument(fileName);
    openDocuments.push_back(document);

    QGraphicsScene * scene = document->getScene();
    QGraphicsView * view = ui->graphicsView;
    QTreeView * treeViewContent = ui->treeViewContent;

    QAbstractItemModel * contentModel = document->getContentItemModel();

    view->setScene(scene);

    if(contentModel)
    {
        treeViewContent->setModel(contentModel);
        // Resize column. First column("Name") take all aviable size, second(page number) minimum size.
        treeViewContent->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        treeViewContent->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    }
    else
        treeViewContent->setModel(nullptr);

    // Add new tab in Tab bar
    addTab(fileName);

    showPage(0);
    enableNavigations();

    lastOpenFileDir = getFileDir(fileName);

    currentDocumentIndex = openDocuments.size() - 1;
}

void MainWindow::saveSettings()
{
    QSettings settings;

    // Save list of open files
    QList<QVariant> fileNameList;
    Document * doc;
    foreach(doc, openDocuments)
    {
        QString fileName = doc->getFileName();
        fileNameList.push_back(QVariant(fileName));
    }

    settings.setValue("openFilesList", QVariant(fileNameList));

    // Save window state
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::restoreSettings()
{
    QSettings settings;

    // Restore window state
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    // Open files was opened in previouse session
    QList<QVariant> fileNameList = settings.value("openFilesList").toList();
    QVariant elem;
    foreach(elem, fileNameList)
    {
        QString fileName = elem.toString();

        try {
            openDocument(fileName);
        } catch(runtime_error & e) {
            qWarning() << "Error when open document " << fileName;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}
