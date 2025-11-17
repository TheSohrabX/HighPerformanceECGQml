#include "EggModel.h"

struct EggFeederPrivate
{
    QVector<QPointF> points;
    int              maxPoints = 0;
};

EggFeeder::EggFeeder(QObject *parent) :
    QObject(parent), dPtr(new EggFeederPrivate)
{}
