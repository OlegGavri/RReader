#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>

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

    // Current opened document
    Poppler::Document * document;

    // Enable/Disable navigations elements
    void enableNavigations();
    void disableNavigations();

    // Set view on given page
    void showPage(const int pageNum);

public slots:
    void on_actionOpen_triggered(bool checked = false);
    void on_actionClose_triggered(bool checked = false);
    void on_spinBoxPageNum_editingFinished();
};
#endif // MAINWINDOW_H
