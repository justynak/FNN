#ifndef CHENONGENERATOR_H
#define CHENONGENERATOR_H

#include "c2ddataseries.h"
#include "icdatagenerator.h"

class CHenonGenerator : ICDataGenerator
{
public:
    CHenonGenerator() :
        _a(1.4), _b(0.3), _initPoint(QVector2D(1.2,1.3)) { _sampleCount = 10000;}

    CHenonGenerator(qreal a, qreal b, QVector2D initpoint) :
        _a(a), _b(b), _initPoint(initpoint) {}

    virtual ~CHenonGenerator();

    qreal GetA();
    qreal GetB();

    void SetA(qreal a);
    void SetB(qreal b);

    C2DDataSeries *GetData();
    void Solve();

protected:
    qreal _a, _b;
    QVector2D _initPoint;
    C2DDataSeries* _outputData;

};

#endif // CHENONGENERATOR_H
