#ifndef CTIMESERIES_H
#define CTIMESERIES_H

#include "icdata.h"
#include <QList>

class CTimeseriesOneDimension : ICData
{
public:
    CTimeseriesOneDimension();
    virtual ~CTimeseriesOneDimension();
    void SetSamplingPeriod(double samplingPeriod);
    void SetTimeLine(QList<double> timeline);


protected:
    QList<double> _timeline;
    QList<double> _samplesline;
    double _samplingPeriod;

};

class CTimeseriesMultiDimension : ICData
{
public:
    CTimeseriesMultiDimension();
    virtual ~CTimeseriesMultiDimension();

protected:
    QList<double> time;
    QList<QList<double> > sample;
    qint32 dimensionNumber = 1;
    double samplingPeriod;
};

#endif // CTIMESERIES_H
