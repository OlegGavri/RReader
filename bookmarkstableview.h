#ifndef BOOKMARKSTABLEVIEW_H
#define BOOKMARKSTABLEVIEW_H

#include <QTableView>

class BookmarksTableView : public QTableView
{
    Q_OBJECT

public:
    explicit BookmarksTableView(QWidget * parent = nullptr);
    ~BookmarksTableView();

private:
    void contextMenuEvent(QContextMenuEvent * event) override;
};

#endif // BOOKMARKSTABLEVIEW_H
