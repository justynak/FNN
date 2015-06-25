#ifndef ICSOLVER_H
#define ICSOLVER_H

#include "icalgorithm.h"
#include "icdata.h"
#include "icmultipledimdataseries.h"
#include "icnotifier.h"
#include "icsolution.h"

class ICSolver
{
public:
    ICSolver() :
        _notifier(NULL) {}

    ICSolver(ICNotifier* notifier):
        _notifier(notifier) {}

    virtual ~ICSolver();

    void SetNotifier(ICNotifier* notifier);

    ICNotifier* GetNotifier();

    virtual void Solve() = 0;

protected:
    //ICMultipleDimDataSeries<void>* _inputData;
    //ICMultipleDimDataSeries<void>* _outputData;
    //ICAlgorithm* _algorithm;
    ICNotifier* _notifier;
};



#endif // ICSOLVER_H
