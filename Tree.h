#ifndef TREE_H
#define TREE_H

#include <QVector>

// The tree store document outline in tree form
template<class T>
class Tree
{
public:
    // Tree node
    class node
    {
        // Stored data
        const T data;

        // Parent and child nodes
        node * const parent;
        QVector<node*> childrens;

        // Index of this node in parent nodes list
        const int parentIndex;

    public:
        node(T t, node * parent, const int parentIndex):
            data(t),
            parent(parent),
            parentIndex(parentIndex)
        {
        }

        // Destructor remove also all children elements
        ~node()
        {
            node * n;
            foreach(n, childrens)
            {
                delete n;
            }
        }

        node * appendChild(T t)
        {
            node * n = new node(t, this, childrens.size());
            childrens.push_back(n);
            return n;
        }

        node * getParent() const
        {
            return parent;
        }

        bool hasChildren() const
        {
            return !childrens.empty();
        }

        int getChildNum() const
        {
            return childrens.size();
        }

        node * getChild(const int i) const
        {
            return childrens[i];
        }

        T getData() const
        {
            return data;
        }

        int getParentIndex() const
        {
            return parentIndex;
        }
    };

    Tree()
    {
        T emptyData;
        root = new node(emptyData, nullptr, 0);
    }

    ~Tree()
    {
        delete root;
    }

    node * getRoot() const
    {
        return root;
    }
private:
    // Root node. Contaion empy data, parent = nullptr
    node * root;
};

#endif // TREE_H
