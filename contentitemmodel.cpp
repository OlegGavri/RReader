#include <cassert>

#include "contentitemmodel.h"



ContentItemModel::ContentItemModel(QVector<Poppler::OutlineItem> outlines, QObject * parent):
    QAbstractItemModel(parent),
    tree(outlines)
{
}

QModelIndex ContentItemModel::index(
    int row,
    int column,
    const QModelIndex &parent) const
{
    TreeElement * parentElem;

    if(!parent.isValid())
    {
        parentElem = tree.getRoot();
    }
    else
    {
        parentElem = static_cast<TreeElement*>(parent.internalPointer());
    }

    TreeElement * elem = parentElem->getChild(row);
    return createIndex(row, column, elem);
}

QVariant ContentItemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    TreeElement * elem = static_cast<TreeElement*>(index.internalPointer());
    Poppler::OutlineItem outlineItem = elem->getOutline();

    int column = index.column();
    if(column == 0)
        return QVariant(outlineItem.name());
    else if(column == 1)
        return outlineItem.destination()->pageNumber();
    else return QVariant();
}

QModelIndex ContentItemModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    TreeElement * elem = static_cast<TreeElement *>(index.internalPointer());
    TreeElement * parentElem = elem->getParent();

    if(parentElem == nullptr)
        return QModelIndex();
    else
        return createIndex(parentElem->getParentIndex(), 0, parentElem);
}

int ContentItemModel::rowCount(const QModelIndex &parent) const
{
    if(parent.column() > 0)
        return 0;

    TreeElement * elem;
    if(!parent.isValid())
        elem = tree.getRoot();
    else
        elem = static_cast<TreeElement*>(parent.internalPointer());

    return elem->getChildNum();
}

int ContentItemModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant ContentItemModel::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if(section == 0)
            return "Name";
        else if(section == 1)
            return "Page";
    }

    return QVariant();
}

int ContentItemModel::getPageFor(const QModelIndex &index) const
{
    assert(index.isValid());
    TreeElement * elem = static_cast<TreeElement*>(index.internalPointer());
    Poppler::OutlineItem outlineItem = elem->getOutline();

    QSharedPointer<const Poppler::LinkDestination> linkDestination = outlineItem.destination();
    int page = linkDestination->pageNumber();

    return page;
}
