#ifndef ICALGORITHM_H
#define ICALGORITHM_H
#include "icdata.h"

class ICAlgorithm
{
public:
    ICAlgorithm();
    ICAlgorithm(ICData* input, ICData* output) :
        _inputDataSeries(input),_outputDataSeries(output) {}

    virtual ~ICAlgorithm();

    void SetInputData(ICData *inputDataSeries);
    void SetOutputData(ICData* outputDataSeries);

    ICData* GetOutputData();
    ICData* GetInputData();

    virtual void Calculate() = 0;

protected:
    ICData* _inputDataSeries;
    ICData* _outputDataSeries;
};

#endif // ICALGORITHM_H
