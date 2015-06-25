#ifndef ICMULTIPLEDIMDATASERIES_H
#define ICMULTIPLEDIMDATASERIES_H

#include <QList>
#include <QVector2D>

#include "icdata.h"

template< class T >

class ICMultipleDimDataSeries : ICData
{
protected:
    QList<T> _data;

public:
    ICMultipleDimDataSeries() : _data(QList<T>()) {}

    ICMultipleDimDataSeries(QList<T> data) : _data(data) {}
    virtual ~ICMultipleDimDataSeries() {_data.clear();}

    bool AttachPoint(T p)
    {
        _data.append(p);
        return true;
    }

    bool RemovePoint(T p)
    {
        return _data.removeOne(p);
    }

    bool ReplacePoint(T p, int i)
    {
        return _data.replace(i, p);
    }

    T operator[] (int number)
    {
        if(number < _data.size())
            return _data[number];
        else
            return T();
    }

    int GetSampleNumber()
    {
        return _data.size();
    }
};

#endif // ICMULTIPLEDIMDATASERIES_H
