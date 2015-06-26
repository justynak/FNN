#ifndef CLORENTZGENERATOR_H
#define CLORENTZGENERATOR_H

#include "icdatagenerator.h"
#include "c3ddataseries.h"

class CLorentzGenerator : ICDataGenerator
{
public:
    CLorentzGenerator() :
        _a(10), _b(2.6666667), _r(28), _dt(0.005), _initPoint(QVector3D(1.2, 1.3, 1.6))
        {_sampleCount = 100000;}

    CLorentzGenerator(qreal a, qreal b, qreal r, qreal dt, QVector3D initPoint, int sampleCount) :
        _a(a), _b(b), _r(r), _dt(dt), _initPoint(initPoint){_sampleCount = sampleCount;}

    virtual ~CLorentzGenerator();

    double GetA();
    double GetB();
    double GetR();
    double GetDt();

    void SetA(qreal a);
    void SetB(qreal b);
    void SetR(qreal r);
    void SetDt(qreal dt);

    C3DDataSeries* GetData();
    void Solve();


protected:
    qreal _a, _b, _r, _dt;
    QVector3D _initPoint;
    C3DDataSeries* _outputData;
};

#endif // CLORENTZGENERATOR_H
