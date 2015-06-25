#include "icsolver.h"


ICSolver::~ICSolver()
{

}

void ICSolver::SetInputData(ICData *inputData)
{
    _inputData = inputData;
}


void ICSolver::SetNotifier(ICNotifier *notifier)
{
    _notifier = notifier;
}

ICData *ICSolver::GetInputData()
{
    return _inputData;
}

ICSolution *ICSolver::GetOutputData()
{
    return _outputData;
}



