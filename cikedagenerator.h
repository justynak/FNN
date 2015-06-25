#ifndef CIKEDAGENERATOR_H
#define CIKEDAGENERATOR_H

#include "icdatagenerator.h"


class CIkedaGenerator : ICDataGenerator
{
public:
    CIkedaGenerator() : _u(0.6) {}
    CIkedaGenerator(qreal u);
    virtual ~CIkedaGenerator();

    qreal GetUValue();
    void SetUValue(qreal u);

    void Solve();

private:
    qreal _u;
};


#endif // CIKEDAGENERATOR_H
