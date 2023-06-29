#include <cassert>

#include "pdfcontentitemmodel.h"

using namespace std;


PdfContentItemModel::PdfContentItemModel(QVector<Poppler::OutlineItem> outlines, QObject * parent):
    ContentsItemModel(parent),
    tree(outlines)
{
}

QModelIndex PdfContentItemModel::index(
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

QVariant PdfContentItemModel::data(const QModelIndex &index, int role) const
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

QModelIndex PdfContentItemModel::parent(const QModelIndex &index) const
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

int PdfContentItemModel::rowCount(const QModelIndex &parent) const
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

int PdfContentItemModel::columnCount(const QModelIndex &) const
{
    // Contents in this application have two column:
    // Name and page
    return 2;
}

QVariant PdfContentItemModel::headerData(
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

optional<int> PdfContentItemModel::getPageFor(const QModelIndex &index) const
{
    assert(index.isValid());
    TreeElement * elem = static_cast<TreeElement*>(index.internalPointer());
    Poppler::OutlineItem outlineItem = elem->getOutline();

    QSharedPointer<const Poppler::LinkDestination> linkDestination = outlineItem.destination();
    int page = linkDestination->pageNumber();

    return page;
}
