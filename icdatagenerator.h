#ifndef ICDATAGENERATOR_H
#define ICDATAGENERATOR_H

#include "icsolver.h"

class ICDataGenerator : public ICSolver
{
public:
    ICDataGenerator() : _sampleCount(0) {}
    virtual ~ICDataGenerator();

    void SetSampleCount(int count);
    int GetSampleCount();

protected:
    int _sampleCount;
};

#endif // ICDATAGENERATOR_H
