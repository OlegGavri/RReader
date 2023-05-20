#ifndef DJVUPAGEGRAPHICSITEM_H
#define DJVUPAGEGRAPHICSITEM_H

#include <libdjvu/ddjvuapi.h>

#include "pagegraphicsitem.h"

class DjvuPageGraphicsItem : public PageGraphicsItem
{
public:
    DjvuPageGraphicsItem(ddjvu_document_t * const document, const int pageNum, const qreal scale);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override;

    qreal sceneYTop() const override;

    int getPageNum() const override;

private:
    ddjvu_document_t * const document;
    const int pageNum;
    const qreal scale;
};

#endif // DJVUPAGEGRAPHICSITEM_H
