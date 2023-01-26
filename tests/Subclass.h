#pragma once

#include <QObject>

class DirectQObjectSubclass : public QObject
{
    Q_OBJECT

public slots:
    void firstSlot(int) {}
    void secondSlot(int) {}

signals:
    void firstSignal(int, int);
    void secondSignal(int, int);
    void thirdSignal(int, int);
};

class IndirectSubclass : public DirectQObjectSubclass
{
    Q_OBJECT

public:
    IndirectSubclass(QString name) : _name(name)
    {}

    QString name() const { return _name; }

public slots:
    void someSlot(const QString& name) { _name = name; }

signals:
    void someSignal(QString text);

private:
    QString _name;
};

