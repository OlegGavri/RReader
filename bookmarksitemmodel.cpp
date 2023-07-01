#include <cassert>

#include <QDataStream>

#include "bookmarksitemmodel.h"

using namespace std;

//
// Index pointer is pointer to its Bookmark
//

//
// Constants
//

// Columns title
const QString TitleName = "Name";
const QString TitlePage = "Page";

// Number of column in the view
const int ColumnNumber = 2;


BookmarksItemModel::BookmarksItemModel(
    QList<Bookmark> & bookmarks,
    QObject * parent):
    QAbstractTableModel(parent),
    bookmarks(bookmarks)
{

}

BookmarksItemModel::~BookmarksItemModel()
{

}

int BookmarksItemModel::getPageFor(const QModelIndex &index) const
{
    assert(index.isValid());

    // Get bookmars by row number of index. And return its page number
    const int row = index.row();
    const Bookmark bookmark = bookmarks[row];
    const int pageNum = bookmark.pageNum;

    return pageNum;
}

QVariant BookmarksItemModel::data(
    const QModelIndex &index,
    int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        const Bookmark * pBookmark = static_cast<const Bookmark*>(index.internalPointer());
        const int column = index.column();

        // Column 0 - bookmark name, 1 - page number
        if(column == 0)
        {
            return pBookmark->text;
        }
        else if(column == 1)
        {
            return pBookmark->pageNum;
        }
        else
            return QVariant();
    }
    else
        return QVariant();
}

int BookmarksItemModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
    {
        // Number of rows is number of bookmarks - this is for root
        return bookmarks.size();
    }
    else
        return 0;
}

int BookmarksItemModel::columnCount(const QModelIndex &) const
{
    return ColumnNumber;
}

QVariant BookmarksItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if(section == 0)
            return TitleName;
        else if(section == 1)
            return TitlePage;
    }

    return QVariant();
}

Qt::ItemFlags BookmarksItemModel::flags(const QModelIndex &index) const
{
    // Item in Name column is editable. User can change the bookmark name
    if(index.isValid() && index.column() == 0)
    {
        return Qt::ItemIsEditable;
    }
    else
        return Qt::NoItemFlags;
}

bool BookmarksItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // Item in Bookmark Name column is editable.
    // Change corresponding bookmark in bookmarks list
    if(role == Qt::EditRole)
    {
        const int row = index.row();
        assert(index.column() == 0);
        assert(row < bookmarks.size());

        Bookmark & bookmark = bookmarks[row];
        QString text = value.toString();

        bookmark.text = text;
        return true;
    }

    return false;
}
