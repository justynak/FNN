#ifndef CIKEDAGENERATOR_H
#define CIKEDAGENERATOR_H

#include "icdatagenerator.h"
#include "c3ddataseries.h"
#include <QVector3D>
#include <qmath.h>

class CIkedaGenerator : public ICDataGenerator
{
public:
    CIkedaGenerator() : _a(0.6), _b(0.1), _c(0.2), _d(0.1), _initPoint(QVector3D(0.1, 0.1, 0.2)) { _sampleCount = (10000); }
    CIkedaGenerator(qreal a, qreal b, qreal c, qreal d, QVector3D initPoint, int sampleCount) :
        _a(a), _b(b), _c(c), _d(d), _initPoint(initPoint) {_sampleCount = (sampleCount);}

    virtual ~CIkedaGenerator();

    double GetA();
    double GetB();
    double GetC();
    double GetD();

    void SetA (qreal a);
    void SetB (qreal b);
    void SetC (qreal c);
    void SetD (qreal d);

    C3DDataSeries* GetData();
    void Solve();

private:
    qreal _a, _b, _c, _d;
    QVector3D _initPoint;
    C3DDataSeries* _outputData;

};


#endif // CIKEDAGENERATOR_H
