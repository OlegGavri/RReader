#include <QMenu>
#include <QContextMenuEvent>

#include "bookmarkstableview.h"

BookmarksTableView::BookmarksTableView(QWidget * parent):
    QTableView(parent)
{

}

BookmarksTableView::~BookmarksTableView()
{
}

void BookmarksTableView::contextMenuEvent(QContextMenuEvent * event)
{
    QMenu menu;
    menu.addAction(tr("Goto bookmark"));
    menu.addSeparator();
    menu.addAction(tr("Add bookmark"));
    menu.addAction(tr("Remove bookmart"));
    menu.addAction(tr("Edit bookmark"));

    menu.exec(event->globalPos());
}
