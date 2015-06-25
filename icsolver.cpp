#include "icsolver.h"


ICSolver::~ICSolver()
{

}


void ICSolver::SetNotifier(ICNotifier *notifier)
{
    _notifier = notifier;
}


