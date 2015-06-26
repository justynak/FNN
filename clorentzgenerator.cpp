#include "clorentzgenerator.h"

CLorentzGenerator::~CLorentzGenerator()
{

}

double CLorentzGenerator::GetA()
{
    return _a;
}

double CLorentzGenerator::GetB()
{
    return _b;
}

double CLorentzGenerator::GetR()
{
    return _r;
}

double CLorentzGenerator::GetDt()
{
    return _dt;
}

void CLorentzGenerator::SetA(qreal a)
{
    _a = a;
}

void CLorentzGenerator::SetB(qreal b)
{
    _b = b;
}

void CLorentzGenerator::SetR(qreal r)
{
    _r = r;
}

void CLorentzGenerator::SetDt(qreal dt)
{
    _dt = dt;
}

C3DDataSeries *CLorentzGenerator::GetData()
{
    return _outputData;
}

void CLorentzGenerator::Solve()
{
    _outputData = new C3DDataSeries();

    qreal xk = _initPoint.x();
    qreal yk = _initPoint.y();
    qreal zk = _initPoint.z();
    //qreal xknext, yknext, zknext;

    for(int i = 0; i<_sampleCount; ++i)
    {
        xk = xk + _a * (yk - xk) * _dt;
        yk = yk + (_r * xk - yk - xk * zk) * _dt;
        zk = zk + (xk * yk - _b * zk) * _dt;

        _outputData->AttachPoint(QVector3D(xk, yk, zk));
       // xk = xknext; yk=yknext; zk=zknext;
        //qDebug() << QObject::tr("Point %4 calculated: %1, %2, %3 \n").arg(xk).arg(yk).arg(zk).arg(i);
    }
}

