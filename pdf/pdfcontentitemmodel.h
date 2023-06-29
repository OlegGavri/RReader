#ifndef PDFCONTENTITEMMODEL_H
#define PDFCONTENTITEMMODEL_H

//#include <QAbstractItemModel>
#include "contentsitemmodel.h"

#include <poppler/qt5/poppler-qt5.h>

//
// Items model for QTreeView show contents of document
//
class PdfContentItemModel : public ContentsItemModel
{
    //
    // The structure to store Poppler::OutlineItem elements in tree form
    //

    //TODO: use Tree<> class
    class TreeElement
    {
    public:
        explicit TreeElement(Poppler::OutlineItem item):
            outlineItem(item),
            parent(nullptr)
        {

        }

        explicit TreeElement():
            parent(nullptr)
        {

        }

        void appendChild(TreeElement * elem)
        {
            childrens.push_back(elem);
            elem->parent = this;
            elem->parentIndex = childrens.size() - 1;
        }

        TreeElement * getParent() const
        {
            return parent;
        }

        bool hasChildren()
        {
            return !childrens.empty();
        }

        int getChildNum() const
        {
            return childrens.size();
        }

        TreeElement * getChild(const int i) const
        {
            return childrens[i];
        }

        Poppler::OutlineItem getOutline() const
        {
            return outlineItem;
        }

        int getParentIndex() const
        {
            return parentIndex;
        }

    private:
        Poppler::OutlineItem outlineItem;
        TreeElement * parent;
        QVector<TreeElement*> childrens;

        // Index of this TreeElement in its parent list
        int parentIndex = 0;
    };

    class Tree
    {
    public:
        Tree(QVector<Poppler::OutlineItem> outlines)
        {
            root = new TreeElement();
            createTreeFor(outlines, root);
        }

        ~Tree()
        {
            deleteWithChildren(root);
        }

        TreeElement * getRoot() const
        {
            return root;
        }

    private:
        TreeElement * root;

        // Traverse outlineItems tree and create
        // the same tree with parent as root element
        void createTreeFor(QVector<Poppler::OutlineItem> outlines, TreeElement * parent)
        {
            Poppler::OutlineItem outlineItem;
            foreach(outlineItem, outlines)
            {
                TreeElement * elem = new TreeElement(outlineItem);
                parent->appendChild(elem);

                if(outlineItem.hasChildren())
                    createTreeFor(outlineItem.children(), elem);
            }
        }

        // Delete TreeElement and its children
        void deleteWithChildren(TreeElement * elem)
        {
            for(int i = 0; i < elem->getChildNum(); i++)
            {
                TreeElement * childElem = elem->getChild(i);
                deleteWithChildren(childElem);
            }

            delete elem;
        }
    };

public:
    explicit PdfContentItemModel(QVector<Poppler::OutlineItem>, QObject * parent = nullptr);
    ~PdfContentItemModel() override {}

    // Overrided methods
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Return page number associated with this content item
    std::optional<int> getPageFor(const QModelIndex &index) const override;

private:
    const Tree tree;
};

#endif // PDFCONTENTITEMMODEL_H
