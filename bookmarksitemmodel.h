#ifndef BOOKMARKSITEMMODEL_H
#define BOOKMARKSITEMMODEL_H

#include <QAbstractTableModel>
#include "bookmark.h"

//
// Item model for bookmarks.
// Used by bookmarks QListView for display and edit bookmarks
// This model contain 2 row - name and page number. Items except the root does not have
// child.
//
class BookmarksItemModel : public QAbstractTableModel
{
public:
    // boormarks - list of bookmarse used by this model
    explicit BookmarksItemModel(QList<Bookmark> & bookmarks, QObject * parent = nullptr);
    ~BookmarksItemModel() override;

    // Return page number for given QModelIndex
    int getPageFor(const QModelIndex &index) const;

    //
    // Overrided methods
    //

    // From QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
private:
    QList<Bookmark> bookmarks;
};


#endif // BOOKMARKSITEMMODEL_H
