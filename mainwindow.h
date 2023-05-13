#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QSpinBox>

#include <poppler/qt5/poppler-qt5.h>

#include "document.h"

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

    // This vector contatain open documents.
    // Order of documents is the same as the order of tabs in MainWindow
    QVector<Document*> openDocuments;

    // Index in openDocuments of current open document
    int currentDocumentIndex = 0;

    // Last open file dir
    QString lastOpenFileDir = QString();

    // Get name of file without path from full path
    static QString getFileBaseName(const QString fileName);
    static QString getFileDir(const QString fileName);

    // Enable/Disable navigations elements
    void enableNavigations();
    void disableNavigations();

    // Navigate to page
    void showPage(const int pageNum);

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

public slots:
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
    void spinBoxPageNum_editingFinished();
    void verticalScroll_valueChanged(int i);
};
#endif // MAINWINDOW_H
