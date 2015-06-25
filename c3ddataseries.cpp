#include "c3ddataseries.h"

C3DDataSeries::C3DDataSeries()
{

}

C3DDataSeries::C3DDataSeries(QList<QVector3D> list)
{
    _data = list;
}

C3DDataSeries::~C3DDataSeries()
{
    _data.clear();
}

QList<qreal> C3DDataSeries::GetXLine()
{
    QList<qreal> xlist;

    foreach (QVector3D point, _data)
    {
        xlist.append(point.x());
    }

    return xlist;
}

QList<qreal> C3DDataSeries::GetYLine()
{
    QList<qreal> ylist;

    foreach (QVector3D point, _data)
    {
        ylist.append(point.y());
    }

    return ylist;
}

QList<qreal> C3DDataSeries::GetZLine()
{
    QList<qreal> zlist;

    foreach (QVector3D point, _data)
    {
        zlist.append(point.z());
    }

    return zlist;
}

void C3DDataSeries::SetXLine(QList<qreal> xLine)
{
    /*
     */
}

void C3DDataSeries::SetYLine(QList<qreal> yLine)
{
    /*
     */
}

void C3DDataSeries::SetZLine(QList<qreal> zLine)
{
    /*
    */
}

