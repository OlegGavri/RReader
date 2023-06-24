#ifndef DJVUCONTENTITEMMODEL_H
#define DJVUCONTENTITEMMODEL_H

#include <optional>

#include <libdjvu/miniexp.h>

#include "contentsitemmodel.h"
#include "Tree.h"

//
// AbstractItemModel for QTreeView. Containt outline of DJVULibre document
//
class DjvuContentItemModel : public ContentsItemModel
{
public:
    // Data structure for storing outlines name and page number
    struct OutlineItem
    {
        QString name;
        std::optional<int> page;
    };

    // Type of node in outline tree
    typedef Tree<OutlineItem>::node Node;

public:
    explicit DjvuContentItemModel(miniexp_t outline, QObject * parent = nullptr);
    ~DjvuContentItemModel() override {}

    // Overrided methods
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    std::optional<int> getPageFor(const QModelIndex &index) const override;

private:
    Tree<OutlineItem> outlinesTree;

    static void parseOutline(
        const miniexp_t expOutline,
        Tree<OutlineItem>::node * node);
};

#endif // DJVUCONTENTITEMMODEL_H
