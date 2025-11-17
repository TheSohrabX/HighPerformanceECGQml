#pragma once

#include <QAbstractListModel>
#include <QPointF>
#include <QVector>

struct EggFeederPrivate;

class EggFeeder : public QObject
{
    Q_OBJECT

public:
    explicit EggFeeder(QObject* parent = nullptr);

private:
    EggFeederPrivate* dPtr;
};

