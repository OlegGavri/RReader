#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QSpinBox>
#include <QScrollBar>

#include <poppler/qt5/poppler-qt5.h>

#include "document.h"
#include "recentfileslist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Create application window, open documents from command line and from
    // previouse session. Read and apply settings.
    // openDocumenst - list of documents(system path to it) for open
    MainWindow(const QStringList openDocuemnts, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSpinBox * spinBoxPageNum;
    QSpinBox * spinBoxZoom;
    QScrollBar * verticalScrollBar;

    // This vector contatain open documents.
    // Order of documents is the same as the order of tabs in MainWindow
    QVector<Document*> openDocuments;

    // Index in openDocuments of current open document
    int currentDocumentIndex = 0;

    // Last open file dir
    QString lastOpenFileDir = QString();

    // List of actions in menu File-><Recent documents>.
    // This action text is document name, and user data is file path
    RecentFilesList recentDocumentsAction;
    QAction * recentFileSeparator;

    // Get name of file without path from full path
    static QString getFileBaseName(const QString fileName);
    static QString getFileDir(const QString fileName);

    // Enable/Disable navigations elements
    void enableNavigations();
    void disableNavigations();

    // Navigate to page
    void showPage(const int pageNum);

    void addZoomSpinBox();
    void addPageNumSpinBox();

    // Navigate to
    void goFirstPage();
    void goPrevPage();
    void goNextPage();
    void goLastPage();

    // Current page in current document
    int currentPage() const;

    // Number of pages in current document
    int documentPageNumber() const;

    // Setup tab bar
    void addTab(const QString fileName);

    // Get current document(selected in tab bar)
    Document * getCurrentDocument() const;

    // Open new document
    void openDocument(const QString fileName);

    void saveSettings();
    void restoreSettings();

    // Enable/disable signal transfer from verticalScrollBar to
    // slot in this window. Can't block all signals from verticalScrollBar because it is used
    // for property widgets work. Only need block signal for this window
    void enableVerticalScrollBarSignal();
    void disableVerticalScrollBarSignal();

    // Switch to document in open documents
    // index - switched document index
    void switchToDocument(const int index);

    // Add new file in recent document list
    void addRecentDocument(const QString fileName);

    // Return paths of recent documents list
    QList<QString> getRecentDocuments() const;

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    // Auto connected slots
    void on_actionOpen_triggered(bool checked = false);
    void on_actionClose_triggered(bool checked = false);
    void on_actionGoFirst_triggered(bool checked = false);
    void on_actionGoPrev_triggered(bool checked = false);
    void on_actionGoNext_triggered(bool checked = false);
    void on_actionGoLast_triggered(bool checked = false);
    void on_actionContent_triggered(bool checked = false);
    void on_treeViewContent_activated(const QModelIndex &index);
    void on_tabBarDocuments_currentChanged(int index);
    void on_actionZoomIn_triggered(bool checked = false);
    void on_actionZoomOut_triggered(bool checked = false);

    // Slots connected in code
    void spinBoxPageNum_editingFinished();
    void spinBoxZoom_editingFinished();
    void verticalScroll_valueChanged(int i);
    void tabBarDocuments_tabCloseRequested(int indes);
    void tabBardDocument_tabMoved(int from, int to);

    // Handle File-><recent document list> QAction
    void recentDocumentAction_triggered(bool);
};
#endif // MAINWINDOW_H
