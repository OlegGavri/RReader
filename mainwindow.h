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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSpinBox * spinBoxPageNum;
    QSpinBox * spinBoxZoom;
    QScrollBar * verticalScrollBar;

    // List of actions in menu File-><Recent documents>.
    // This action text is document name, and user data is file path
    RecentFilesList recentDocumentsAction;
    QAction * recentFileSeparator;

    // Enable/Disable navigations elements
    void enableNavigations();
    void disableNavigations();

    // Display(render) current page of current document in view
    void showCurrentPage();

    // Display pageNum of current document in view
    void showPage(const int pageNum);

    void addZoomSpinBox();
    void addPageNumSpinBox();

    // Enable/disable signal transfer from verticalScrollBar to
    // slot in this window. Can't block all signals from verticalScrollBar because it is used
    // for property widgets work. Only need block signal for this window
    void enableVerticalScrollBarSignal();
    void disableVerticalScrollBarSignal();

    // Set list of recent documents in menu File
    void setRecentDocuments(const QList<QString>);

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

    // Enable/disable contents panel
    void on_actionContent_triggered(bool checked = false);

    // User activete(double click or Enter) on contents element
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
