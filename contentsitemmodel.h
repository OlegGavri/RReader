#ifndef CONTENTSITEMMODEL_H
#define CONTENTSITEMMODEL_H

#include <optional>

#include <QAbstractItemModel>

//
// AbstractItemModel for documents contents(outline). Contents QTreeView use this model
// for display content and navigation when user select some item in it
// This class add to QAbstractItemModel 1 method - getPageFor
class ContentsItemModel : public QAbstractItemModel
{
public:
    ContentsItemModel(QObject * parent = nullptr):QAbstractItemModel(parent){}

    // Return page number associated with this content item. Use optionnal
    // because content item may not contation page number
    virtual std::optional<int> getPageFor(const QModelIndex &index) const = 0;
};


#endif // CONTENTSITEMMODEL_H
