#include "QObjectProxyTest.h"

void QObjectProxyTest::initTestCase()
{
    _one = new IndirectSubclass{"One"};
    _two = new IndirectSubclass{"Two"};
    _prx = new QObjectProxy{&IndirectSubclass::staticMetaObject};
}

void QObjectProxyTest::cleanupTestCase()
{
    delete _one;
    _one = nullptr;
    delete _two;
    _two = nullptr;
    delete _prx;
    _prx = nullptr;
}

void QObjectProxyTest::emitSignal()
{
    _prx->setObject(_one);

    QVERIFY(connect(_prx, SIGNAL(someSignal(QString)), _two, SLOT(someSlot(QString)), Qt::DirectConnection));

    emit _one->someSignal("111");

    QCOMPARE(_two->name(), "111");
}

void QObjectProxyTest::invokeSlot()
{
    _prx->setObject(_one);

    QMetaObject::invokeMethod(_prx, "someSlot", Qt::DirectConnection, Q_ARG(QString, "111"));

    QCOMPARE(_one->name(), "111");
}

void QObjectProxyTest::checkInheritance()
{
    auto p = static_cast<QObject*>(_prx);

    QVERIFY(p->inherits("QObjectProxy"));
    QVERIFY(!p->inherits("ProxiedClass"));
}
