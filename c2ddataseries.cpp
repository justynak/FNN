#include "c2ddataseries.h"


C2DDataSeries::C2DDataSeries()
{

}

/*C2DDataSeries::C2DDataSeries(QList<QVector2D> list);
{
    _data = list;
}
*/

C2DDataSeries::~C2DDataSeries()
{
   // _data.clear();
}


QList<qreal> C2DDataSeries::GetXLine()
{
    QList<qreal> xlist;

    foreach (QVector2D point, _data)
    {
        xlist.append(point.x());
    }

    return xlist;  
}

QList<qreal> C2DDataSeries::GetYLine()
{
    QList<qreal> ylist;

    foreach (QVector2D point, _data)
    {
        ylist.append(point.y());
    }

    return ylist;   
}

void C2DDataSeries::SetXLine(QList<qreal> xLine)
{
    /*
    uint i =0;
    foreach (QVector2D point, this->_data) {
        point.setX(xLine[i]);
        ++i;
    }
    */
}

void C2DDataSeries::SetYLine(QList<qreal> yLine)
{
    /*
    */
}
