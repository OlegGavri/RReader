#ifndef PAGEGRAPHICSITEM_H
#define PAGEGRAPHICSITEM_H

#include <QGraphicsItem>

//
// This class draw one page of document in scene
// This is abstract class. Classes for several documents types(PDF, DJVU etc) is
// inheritance from this
//
class PageGraphicsItem : public QGraphicsItem
{
public:
    // Return y coordinate of top boundedRect side in scene coordinate system
    virtual qreal sceneYTop() const = 0;

    // Page number of this page in document
    virtual int getPageNum() const = 0;
};

#endif // PAGEGRAPHICSITEM_H
