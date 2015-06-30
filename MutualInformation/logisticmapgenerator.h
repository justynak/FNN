#ifndef LOGISTICMAPGENERATOR_H
#define LOGISTICMAPGENERATOR_H
#include <QList>

class LogisticMapGenerator
{
public:
    LogisticMapGenerator() : _data(NULL), _r(3.6), _sampleCount(1000), _x0(0.5) {}
    LogisticMapGenerator(QList<qreal>* data, qreal r,qreal x0, int sampleCount) :
        _data(data), _r(r), _x0(x0), _sampleCount(sampleCount) {}
    ~LogisticMapGenerator() {}

    void SetData(QList<qreal> *data)
        {_data = data;}
    void SetR(qreal r){_r = r;}

    QList<qreal>* GetData(){return _data;}

    void Solve();

private:
    QList<qreal> *_data;
    qreal _r;
    int _sampleCount;
    qreal _x0;
};

#endif // LOGISTICMAPGENERATOR_H
