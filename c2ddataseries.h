#ifndef C2DDATASERIES_H
#define C2DDATASERIES_H

#include <QList>
#include <QVector2D>
#include "icmultipledimdataseries.h"

class C2DDataSeries : public ICMultipleDimDataSeries<QVector2D>
{
public:
    C2DDataSeries();
    C2DDataSeries(QList<QVector2D> list);

    ~C2DDataSeries();

    QList<qreal> GetXLine();
    QList<qreal> GetYLine();

    void SetXLine(QList<qreal> xLine);
    void SetYLine(QList<qreal> yLine);


protected:
    //QList<QVector2D> _data;
};

#endif // C2DDATASERIES_H
