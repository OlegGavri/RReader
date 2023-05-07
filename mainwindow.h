#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QSpinBox>

#include <poppler/qt5/poppler-qt5.h>

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

    // Current opened document
    Poppler::Document * document;

    // Current page
    int _currentPage = 0;

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

public slots:
    void on_actionOpen_triggered(bool checked = false);
    void on_actionClose_triggered(bool checked = false);
    void on_actionGoFirst_triggered(bool checked = false);
    void on_actionGoPrev_triggered(bool checked = false);
    void on_actionGoNext_triggered(bool checked = false);
    void on_actionGoLast_triggered(bool checked = false);
    void spinBoxPageNum_editingFinished();
    void verticalScroll_valueChanged(int i);
};
#endif // MAINWINDOW_H
