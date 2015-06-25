#ifndef ICSOLUTION_H
#define ICSOLUTION_H

#include "icdata.h"

class ICSolution
{
public:
    ICSolution();
    virtual ~ICSolution();

protected:
    ICData* data;
};

#endif // ICSOLUTION_H
