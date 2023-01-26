#include <QtTest>

#include "../QObjectProxy.h"

#include "Subclass.h"

class QObjectProxyTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void emitSignal();
    void invokeSlot();
    void checkInheritance();

private:
    IndirectSubclass* _one = nullptr;
    IndirectSubclass* _two = nullptr;
    QObjectProxy*     _prx = nullptr;
};
