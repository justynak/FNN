#include "icdatagenerator.h"

ICDataGenerator::~ICDataGenerator()
{

}

void ICDataGenerator::SetSampleCount(int count)
{
    _sampleCount = count;
}

int ICDataGenerator::GetSampleCount()
{
    return _sampleCount;
}

