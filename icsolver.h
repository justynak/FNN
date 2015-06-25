#ifndef ICSOLVER_H
#define ICSOLVER_H

#include "icalgorithm.h"
#include "icdata.h"
#include "icnotifier.h"
#include "icsolution.h"

class ICSolver
{
public:
    ICSolver() :
        _inputData(NULL), _outputData(NULL), _notifier(NULL) {}

    ICSolver(ICData* inputData, ICNotifier* notifier):
        _inputData(inputData), _notifier(notifier), _outputData(NULL) {}

    virtual ~ICSolver();

    void SetInputData(ICData* inputData);
    //void SetAlgorithm(ICAlgorithm* algorithm);
    void SetNotifier(ICNotifier* notifier);

    ICData* GetInputData();
    ICSolution* GetOutputData();
    //ICAlgorithm* GetAlgorithm();
    ICNotifier* GetNotifier();

    virtual void Solve() = 0;

protected:
    ICData* _inputData;
    ICSolution* _outputData;
    //ICAlgorithm* _algorithm;
    ICNotifier* _notifier;
};



#endif // ICSOLVER_H
