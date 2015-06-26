#include "cikedagenerator.h"
#include <QObject>
#include <QDebug>

CIkedaGenerator::~CIkedaGenerator()
{

}

double CIkedaGenerator::GetA ()
{
    return _a;
}

double CIkedaGenerator::GetB ()
{
    return _b;
}

double CIkedaGenerator::GetC ()
{
    return _c;
}

double CIkedaGenerator::GetD ()
{
    return _d;
}

C3DDataSeries *CIkedaGenerator::GetData()
{
    return _outputData;
}

void CIkedaGenerator::SetA (qreal a)
{
    _a = a;
}

void CIkedaGenerator::SetB (qreal b)
{
    _b = b;
}

void CIkedaGenerator::SetC (qreal c)
{
    _c = c;
}

void CIkedaGenerator::SetD (qreal d)
{
    _d = d;
}


void CIkedaGenerator::Solve()
{
    _outputData = new C3DDataSeries();

    qreal xk = _initPoint.x();
    qreal xknext = _initPoint.x();
    qreal yk = _initPoint.y();
    qreal zk = _c - _d/(qPow(xk,2) + qPow(yk,2) + 1);

    for(int i = 0; i<_sampleCount; ++i)
    {
        zk = _c - _d/(qPow(xk,2) + qPow(yk,2) + 1);
        xknext = _a + _b*(xk * qCos(zk) - yk* qSin(zk)) ;
        yk = _b * (xk * qSin(zk) + yk * qCos(zk));
        xk = xknext;
        _outputData->AttachPoint(QVector3D(xk, yk, zk));

        //qDebug() << QObject::tr("Point %4 calculated: %1, %2, %3 \n").arg(xk).arg(yk).arg(zk).arg(i);
    }
}

