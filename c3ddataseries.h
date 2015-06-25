#ifndef C3DDATASERIES_H
#define C3DDATASERIES_H

#include <QVector3D>
#include <QList>
#include "icmultipledimdataseries.h"

class C3DDataSeries : public ICMultipleDimDataSeries<QVector3D>
{
public:
    C3DDataSeries();
    C3DDataSeries(QList<QVector3D> list);
    virtual ~C3DDataSeries();

    QList<qreal> GetXLine();
    QList<qreal> GetYLine();
    QList<qreal> GetZLine();

    void SetXLine(QList<qreal> xLine);
    void SetYLine(QList<qreal> yLine);
    void SetZLine(QList<qreal> zLine);

protected:
};

#endif // C3DDATASERIES_H
