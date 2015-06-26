#include "chenongenerator.h"
#include <QtMath>

CHenonGenerator::~CHenonGenerator()
{

}

qreal CHenonGenerator::GetA()
{
    return _a;
}

qreal CHenonGenerator::GetB()
{
    return _b;
}

void CHenonGenerator::SetA(qreal a)
{
    _a = a;
}

void CHenonGenerator::SetB(qreal b)
{
    _b = b;
}

C2DDataSeries *CHenonGenerator::GetData()
{
    return _outputData;
}

void CHenonGenerator::Solve()
{
    _outputData = new C2DDataSeries();

    qreal xk = _initPoint.x();
    qreal yk = _initPoint.y();

    qreal xknext, yknext;


    for(int i = 0; i<_sampleCount; ++i)
    {
        xknext = 1 - _a* xk * xk + yk;
        yknext = _b * xk;

        _outputData->AttachPoint(QVector2D(xknext, yknext));
        yk = yknext; xk = xknext;

        //qDebug() << QObject::tr("Point %4 calculated: %1, %2, %3 \n").arg(xk).arg(yk).arg(zk).arg(i);
    }
}

