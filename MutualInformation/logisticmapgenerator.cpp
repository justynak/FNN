#include "logisticmapgenerator.h"
#include <QDebug>

void LogisticMapGenerator::Solve()
{    
    _data->append(_x0);

    for(int i=1; i<_sampleCount; ++i)
    {
        qreal xnext = _r * (_data->value(i-1))*(1 - (_data->value(i-1)));
        _data->append(xnext);
        //qDebug() << "sample nr " << i << ": "<< samples[i] << "\n";
    }
}

