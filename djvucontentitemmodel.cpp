#include <optional>


#include "djvucontentitemmodel.h"

using namespace std;


static optional<int> parsePageNum(miniexp_t exp)
{
    // Page string has form #<num>(e.g #423)
    const char * pageStr = miniexp_to_str(exp);
    if(pageStr && pageStr[0] == '#')
    {
        QString str = pageStr + 1;
        bool ok;

        int pageNum = str.toInt(&ok);

        if(ok)
            return pageNum;
    }

    return nullopt;
}

//
// Parse DJVU document outline and return outline in
// Tree form.
// expOutline - S-expression element for parsing
// node - node to add outlines tree from expOutline. Children expression from expOutline become
//        child nodes for node
//
void DjvuContentItemModel::parseOutline(
    const miniexp_t expOutline,
    Tree<OutlineItem>::node * node)
{
    //
    // Outline has form:
    // (bookmarks (#<num> (<name> <(inner list with)>)
    //

    // Number of items
    const int len = miniexp_length(expOutline) - 1;

    // The first cda element is not important. It contain 'bookmark' for root
    // element or name of parent element for other.
    miniexp_t lista = miniexp_cdr(expOutline);

    for(int i = 0; i < len; i++)
    {
        miniexp_t listn = miniexp_nth(i, lista);

        miniexp_t expName = miniexp_car(listn);
        miniexp_t expCdr = miniexp_cdr(listn);

        miniexp_t expPage = miniexp_car(expCdr);

        optional<int> pageNum = parsePageNum(expPage);

        QString name = miniexp_to_str(expName);

        OutlineItem outlineItem;
        outlineItem.name = name;
        outlineItem.page = pageNum;

        Tree<OutlineItem>::node * newNode = node->appendChild(outlineItem);

        // node contain nested list, make recursion call this function for it.
        if(miniexp_length(expCdr) > 1)
        {
            parseOutline(expCdr, newNode);
        }
    }
}

DjvuContentItemModel::DjvuContentItemModel(miniexp_t outline, QObject * parent):
    ContentsItemModel(parent)
{
    Tree<OutlineItem>::node * rootNode = outlinesTree.getRoot();
    parseOutline(outline, rootNode);
}

QModelIndex DjvuContentItemModel::index(
    int row,
    int column,
    const QModelIndex &parent) const
{
    Node * node;

    if(!parent.isValid())
    {
        node = outlinesTree.getRoot();
    }
    else
    {
        node = static_cast<Node*>(parent.internalPointer());
    }

    Node * elem = node->getChild(row);
    return createIndex(row, column, elem);
}

QVariant DjvuContentItemModel::data(
    const QModelIndex &index,
    int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole)
        return QVariant();

    Node * node = static_cast<Node*>(index.internalPointer());

    OutlineItem item = node->getData();

    int column = index.column();
    if(column == 0)
        return QVariant(item.name);
    else if(column == 1)
        return item.page.has_value() ? QVariant(item.page.value()) : QVariant("");
    else
        return QVariant();
}

QModelIndex DjvuContentItemModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    Node * node = static_cast<Node*>(index.internalPointer());
    Node * parentNode = node->getParent();

    if(parentNode == nullptr)
        return QModelIndex();
    else
        return createIndex(parentNode->getParentIndex(), 0, parentNode);
}

int DjvuContentItemModel::rowCount(const QModelIndex &parent) const
{
    // Nodes with column > 0 has't child
    if(parent.column() > 0)
        return 0;

    Node * node;
    if(!parent.isValid())
        node = outlinesTree.getRoot();
    else
        node = static_cast<Node*>(parent.internalPointer());

    return node->getChildNum();
}

int DjvuContentItemModel::columnCount(const QModelIndex &) const
{
    // Contents in this application have two column:
    // Name and page
    return 2;
}

QVariant DjvuContentItemModel::headerData(
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

optional<int> DjvuContentItemModel::getPageFor(
    const QModelIndex &index) const
{
    assert(index.isValid());
    Node * node = static_cast<Node*>(index.internalPointer());
    OutlineItem item = node->getData();

    return item.page;
}
