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
#include "settings.h"

using namespace std;

//
// Constants
//

const int RecentDocumentsListMaxSize = 5;

MainWindow::MainWindow(const QStringList openDocuments, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add separators for recent files list
    ui->menuFile->insertSeparator(ui->actionExit);
    recentFileSeparator = ui->menuFile->insertSeparator(ui->actionExit);

    addPageNumSpinBox();
    addZoomSpinBox();

    QGraphicsScene * scene = new QGraphicsScene();
    QGraphicsView * view = ui->graphicsView;
    view->setScene(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag);

    // Set Documents tab bar settings
    QTabBar * tabBar = ui->tabBarDocuments;
    tabBar->setTabsClosable(true);
    tabBar->setMovable(true);

    // Receive document scrolling signal for tracking current page number and etc.
    verticalScrollBar = view->verticalScrollBar();


    // Connect signals
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::closeAllWindows, Qt::QueuedConnection);
    connect(tabBar, &QTabBar::tabCloseRequested, this, &MainWindow::tabBarDocuments_tabCloseRequested);
    connect(tabBar, &QTabBar::tabMoved, this, &MainWindow::tabBardDocument_tabMoved);

    enableVerticalScrollBarSignal();

    // Open documents
    QString fileName;
    foreach(fileName, openDocuments)
    {
        try {
            openDocument(fileName);
        } catch(runtime_error & e) {
            qWarning() << "Error when open document " << fileName;
        }
    }

    restoreSettings();
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
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        lastOpenFileDir.isEmpty() ? QDir::homePath() : lastOpenFileDir,
        "PDF documents(*.pdf *.djvu)");

    if(!fileName.isEmpty())
    {
        try{
            openDocument(fileName);
            addRecentDocument(fileName);
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

    closedDocument->saveSettings();

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
    QSignalBlocker bl(spinBoxPageNum);

    // User activate item in content. Go to selected content item.
    ContentsItemModel * model = static_cast<ContentsItemModel*>(ui->treeViewContent->model());
    optional<int> page = model->getPageFor(index);

    if(page.has_value())
    {
        spinBoxPageNum->setValue(page.value());
        showPage(page.value());
    }
}

void MainWindow::on_tabBarDocuments_currentChanged(int index)
{
    QSignalBlocker bl0(spinBoxZoom);
    QSignalBlocker bl1(spinBoxPageNum);

    // Change current document, scene and content tree
    currentDocumentIndex = index;
    Document * currentDocument = openDocuments[index];

    QGraphicsScene * scene = currentDocument->getScene();
    ContentsItemModel * contentModel = currentDocument->getContentItemModel();
    int currentPage = currentDocument->getCurrentPage();

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
    qreal scale = currentDocument->getScale();
    spinBoxZoom->setValue(scale * 100);

    spinBoxPageNum->setValue(currentPage);

    showPage(currentPage);
}

void MainWindow::on_actionZoomIn_triggered(bool)
{
    Document * currentDoc = getCurrentDocument();
    int currentPage = currentDoc->getCurrentPage();

    currentDoc->zoomIn();
    showPage(currentPage);

    // Update zoom spin box
    qreal scale = currentDoc->getScale();
    int zoom = scale * 100;

    QSignalBlocker bl(spinBoxZoom);
    spinBoxZoom->setValue(zoom);
}

void MainWindow::on_actionZoomOut_triggered(bool)
{
    Document * currentDoc = getCurrentDocument();
    int currentPage = currentDoc->getCurrentPage();

    currentDoc->zoomOut();
    showPage(currentPage);

    // Update zoom spin box
    qreal scale = currentDoc->getScale();
    int zoom = scale * 100;

    QSignalBlocker bl(spinBoxZoom);
    spinBoxZoom->setValue(zoom);
}

void MainWindow::spinBoxPageNum_editingFinished()
{
    // Go to page pageNum
    const int pageNum = spinBoxPageNum->value() - 1;
    showPage(pageNum);
}

void MainWindow::spinBoxZoom_editingFinished()
{
    // Set zoom for current document
    const int zoom = spinBoxZoom->value();
    const qreal scale = (qreal)(zoom) / 100.0;

    Document * document = getCurrentDocument();
    document->setScale(scale);
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
        spinBoxPageNum->setValue(pageNum + 1);
    }
}

void MainWindow::tabBarDocuments_tabCloseRequested(int index)
{
    QSignalBlocker blZoom(spinBoxZoom);
    QSignalBlocker blPage(spinBoxPageNum);
    QSignalBlocker blTabBar(ui->tabBarDocuments);

    //
    // Close document with index. If index is current switch to another document
    // (to next document or to previouse if closed document is last in list)
    //
    Document * closedDocument = openDocuments[index];

    QTabBar * tabBar = ui->tabBarDocuments;

    if(index != currentDocumentIndex)
    {
        // Closed document is not current
        openDocuments.removeAt(index);
        delete closedDocument;
    }
    else
    {
        // Closed document is a current document
        QGraphicsView * view = ui->graphicsView;
        QTreeView * treeViewContent = ui->treeViewContent;

        const int documentsNumber = openDocuments.size();

        // If applicatoin contain only 1 document remove it, clear graphicsView and
        // disable navigation
        if(documentsNumber == 1)
        {
            openDocuments.clear();
            view->setScene(nullptr);
            treeViewContent->setModel(nullptr);

            disableNavigations();

            delete closedDocument;
        }
        else
        {
            // If there are more documents, close current and switch
            // to another
            const int switchedDocumentIndex = (index == documentsNumber - 1) ? index - 1 : index + 1;
            const Document * switchedDocument = openDocuments[switchedDocumentIndex];

            QGraphicsScene * scene = switchedDocument->getScene();
            ContentsItemModel * model = switchedDocument->getContentItemModel();
            int page = switchedDocument->getCurrentPage();
            qreal scale = switchedDocument->getScale();

            view->setScene(scene);
            treeViewContent->setModel(model);

            spinBoxPageNum->setValue(page);
            int zoom = scale * 100;
            spinBoxZoom->setValue(zoom);

            openDocuments.removeAt(index);
            delete closedDocument;
        }
    }

    tabBar->removeTab(index);
}

void MainWindow::tabBardDocument_tabMoved(int from, int to)
{
#if QT_VERSION >= 0x051400
    openDocuments.swapItemsAt(from, to);
#else
    Document * temp = openDocuments[to];
    openDocuments[to] = temp;
    openDocuments[from] = temp;
#endif
}

void MainWindow::recentDocumentAction_triggered(bool)
{
    QAction * senderAction = dynamic_cast<QAction*>(sender());
    QString documentPath = senderAction->data().toString();

    try {
        openDocument(documentPath);
    } catch(runtime_error & e) {
        QMessageBox::critical(
            this,
            tr("Error"),
            QString("Open file %1 error (%2)").arg(documentPath).arg(e.what()));
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
    spinBoxZoom->setEnabled(true);
    ui->actionGoFirst->setEnabled(true);
    ui->actionGoNext->setEnabled(true);
    ui->actionGoPrev->setEnabled(true);
    ui->actionGoLast->setEnabled(true);
}

void MainWindow::disableNavigations()
{
    spinBoxPageNum->setEnabled(false);
    spinBoxZoom->setEnabled(false);
    ui->actionGoFirst->setEnabled(false);
    ui->actionGoNext->setEnabled(false);
    ui->actionGoPrev->setEnabled(false);
    ui->actionGoLast->setEnabled(false);
}

void MainWindow::showPage(const int pageNum)
{
    disableVerticalScrollBarSignal();

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

    enableVerticalScrollBarSignal();
}

void MainWindow::goFirstPage()
{
    QSignalBlocker bl(spinBoxPageNum);
    showPage(0);
    spinBoxPageNum->setValue(1);
}

void MainWindow::goPrevPage()
{
    QSignalBlocker bl(spinBoxPageNum);

    const int currPage = currentPage();
    if(currPage == 0)
        return;

    showPage(currPage - 1);
    spinBoxPageNum->setValue(currPage);
}

void MainWindow::goNextPage()
{
    QSignalBlocker bl(spinBoxPageNum);

    const int currPage = currentPage();

    if(currPage == documentPageNumber() - 1)
        return;

    showPage(currPage + 1);
    spinBoxPageNum->setValue(currPage + 2);
}

void MainWindow::goLastPage()
{
    QSignalBlocker bl(spinBoxPageNum);
    const int currPage = documentPageNumber();

    showPage(currPage - 1);
    spinBoxPageNum->setValue(currPage);
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

    showPage(document->getCurrentPage());
    enableNavigations();

    lastOpenFileDir = getFileDir(fileName);

    // Update zoom
    QSignalBlocker bl(spinBoxZoom);
    qreal scale = document->getScale();
    int zoom = scale * 100;

    spinBoxZoom->setValue(zoom);
}

void MainWindow::saveSettings()
{
    //TODO: move to Settings
    QSettings settings;

    // Save list of open files
    QList<QString> fileNameList;
    Document * doc;
    foreach(doc, openDocuments)
    {
        QString fileName = doc->getFileName();
        fileNameList.push_back(fileName);
    }

    Settings::SetDocumentsList(fileNameList);
    Settings::SetOpenDocumentNumber(currentDocumentIndex);

    // Save window state
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    // Save settings for each open document
    foreach(doc, openDocuments)
    {
        doc->saveSettings();
    }

    // Save recent documents list
    QStringList recentDocumentsList = getRecentDocuments();
    Settings::SetRecentDocuments(recentDocumentsList);
}

void MainWindow::restoreSettings()
{
    //TODO: move to Settings
    QSettings settings;

    // Restore window state
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    // Open files was opened in previouse session
    QStringList fileNameList = Settings::GetDocumentList();
    QString fileName;
    foreach(fileName, fileNameList)
    {
        try {
            openDocument(fileName);
        } catch(runtime_error & e) {
            qWarning() << "Error when open document " << fileName;
        }
    }

    optional<int> documentNum = Settings::GetOpenDocumentNumber();
    if(documentNum.has_value())
    {
        const int index = documentNum.value();
        if(index < openDocuments.size() && index >= 0)
            switchToDocument(index);
    }

    QStringList recentDocuments = Settings::GetRecentDocuments();
    foreach(fileName, recentDocuments)
    {
        addRecentDocument(fileName);
    }
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

void MainWindow::switchToDocument(const int index)
{
    QSignalBlocker bl0(ui->tabBarDocuments);
    QSignalBlocker bl1(spinBoxPageNum);
    QSignalBlocker bl2(spinBoxZoom);
    disableVerticalScrollBarSignal();

    assert(index < openDocuments.size());
    currentDocumentIndex = index;
    Document * currentDocument = openDocuments[index];

    QGraphicsScene * scene = currentDocument->getScene();
    ContentsItemModel * contentModel = currentDocument->getContentItemModel();
    int currentPage = currentDocument->getCurrentPage();

    ui->graphicsView->setScene(scene);
    ui->treeViewContent->setModel(contentModel);

    // Resize column. First column("Name") take all aviable size, second(page number) minimum size.
    if(contentModel != nullptr)
    {
        ui->treeViewContent->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->treeViewContent->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    }

    // Set zoom
    qreal scale = currentDocument->getScale();
    spinBoxZoom->setValue(scale * 100);

    showPage(currentPage);
    ui->tabBarDocuments->setCurrentIndex(index);

    enableVerticalScrollBarSignal();
}

void MainWindow::addRecentDocument(const QString fileName)
{
    //
    // Add new action in menu File. Text of action - base filename.
    // user data of action - file path
    // If recent file list contain more than maximum number of item, remove old item.
    //
    const QString text = getFileBaseName(fileName);
    const QVariant path = fileName;

    // If file with fileName already exist in recent files list, do anything
    if(recentDocumentsAction.exist(fileName))
        return;

    QMenu * menu = ui->menuFile;

    QAction * action = new QAction(text);
    action->setData(path);

    if(recentDocumentsAction.size() == RecentDocumentsListMaxSize)
    {
        QAction * lastAction = recentDocumentsAction.last();
        menu->removeAction(lastAction);
        recentDocumentsAction.removeLast();

        delete lastAction;
    }

    recentDocumentsAction.push_back(action);

    menu->insertAction(recentFileSeparator, action);

    connect(action, &QAction::triggered, this, &MainWindow::recentDocumentAction_triggered);
}

QList<QString> MainWindow::getRecentDocuments() const
{
    QList<QString> list;
    QString filePath;

    QAction * action;
    foreach(action, recentDocumentsAction)
    {
        QString path = action->data().toString();
        list.push_back(path);
    }

    return list;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}
