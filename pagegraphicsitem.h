#ifndef PAGEGRAPHICSITEM_H
#define PAGEGRAPHICSITEM_H

#include <QGraphicsItem>

#include <poppler/qt5/poppler-qt5.h>

//
// This class draw one page of PDF document in scene
//
class PageGraphicsItem : public QGraphicsItem
{
public:
    PageGraphicsItem(const Poppler::Document * document, const int pageNum);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

    // Return y coordinate of top boundedRect side in scene coordinate system
    qreal sceneYTop() const;

    int getPageNum() const;

private:
    const Poppler::Document * document;
    const int pageNum;
};

#endif // PAGEGRAPHICSITEM_H
