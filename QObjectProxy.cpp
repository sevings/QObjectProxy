#include "QObjectProxy.h"

#include <QMetaMethod>


QObjectProxy::QObjectProxy(const QMetaObject* meta, QObject* parent) : QObject(parent), _meta(*meta)
{
    Q_ASSERT_X(_meta.superClass(), "QObjectProxy", "The proxied class must be a QObject subclass");

    // We want Qt to call QObjectProxy::qt_metacall() instead of the proxied class' qt_static_metacall().
    // Clear the field in the proxied class's QMetaObject and all superclasses', excluding QObject.
    for(auto meta = &_meta; meta->d.superdata;)
    {
        meta->d.static_metacall = nullptr;

        auto superdata    = new QMetaObject{*meta->d.superdata};
        meta->d.superdata = superdata;

        meta = superdata;
    }
}


QObjectProxy::~QObjectProxy()
{
    for(auto meta = _meta.d.superdata; meta->d.superdata;)
    {
        auto superdata = meta->d.superdata;

        delete meta;

        meta = superdata;
    }
}


void QObjectProxy::setObject(QObject* obj)
{
    Q_ASSERT(!obj || strcmp(obj->metaObject()->className(), _meta.className()) == 0);
    Q_ASSERT(!obj || obj->thread() == thread());

    if(_obj)
        _obj->disconnect(this);

    _obj = obj;

    if(!_obj)
        return;

    // Connect all the proxied object signals to the proxy's imaginary slots with the same indices.
    // QObject signals do not connect.
    // Because the objects must live in the same thread, we can use Qt::DirectConnection.
    // To use Qt::AutoConnection, we will most likely need to pass an array of the signal argument types to connect().
    // But I haven't attempted it.
    for(int i = QObject::staticMetaObject.methodCount(); i < _meta.methodCount(); i++)
    {
        if(_meta.method(i).methodType() == QMetaMethod::Signal)
            QMetaObject::connect(_obj, i, this, i, Qt::DirectConnection, nullptr);
    }
}


//! Qt uses this method to invoke slots and signals, to get and set properties, etc.
//! \param call - a type of this metacall, such as InvokeMetaMethod.
//! \param id   - an absolute index of the invoked method.
//! \param args - an array of arguments to pass to the invoked method.
int QObjectProxy::qt_metacall(QMetaObject::Call call, int id, void** args)
{
    // First of all, try calling the base method.
    // It could be a QObject signal or slot, and we don't proxy them.
    auto qoRet = QObject::qt_metacall(call, id, args);

    // qt_metacall() should return a method index relative to QMetaObject::methodOffset().
    // The value less than zero indicates that the index is within QObject method range
    // and that the call was successfull.
    if(qoRet < 0)
        return qoRet;

    if(!_obj)
    {
        qWarning("QObjectProxy: the proxied object has not been set");

        // We are at a loss for what to do. Subclasses can handle the call.
        return id - _meta.methodOffset();
    }

    // Check to see if Qt is not attempting to call a method connected to a signal of the proxied object.
    if(call != QMetaObject::InvokeMetaMethod || sender() != _obj || senderSignalIndex() != id)
    {
        // Someone is invoking an imaginary proxy slot.
        // Just invoke the proxied object's real slot.
        // We can do it, because objects live in the same thread.
        return _obj->qt_metacall(call, id, args);
    }

    const QMetaObject* classmeta = &_meta;

    // Find a QMetaObject that describes the class of the signal that was invoked.
    for(auto meta = classmeta->superClass(); id < meta->methodCount(); meta = meta->superClass())
        classmeta = meta;

    // The index position of the first signal in the class.
    // Similar to QMetaObject::methodOffset(), but non-signal methods are excluded.
    int signalOffset = 0;

    // The same as in QMetaObjectPrivate::signalOffset().
    for(auto supermeta = classmeta->superClass(); supermeta; supermeta = supermeta->superClass())
    {
        // Although the 'data' field is declared as uint*, it is actually QMetaObjectPrivate*.
        // QMetaObjectPrivate is a struct with many fields. They are all of the type 'int'.
        // As a result, we can directly access required fields by using array indices.

        // The first field is revision. Revision 7 indicates Qt 5.0 or later.
        Q_ASSERT(supermeta->d.data[0] >= 7);

        // The field 13 is the signal count of the class.
        signalOffset += supermeta->d.data[13];
    }

    // The index of the invoked signal in its class.
    // Because signals are always counted first, we don't need to exclude non-signals methods here.
    int localSignalIndex = id - classmeta->methodOffset();

    // Pretend it's the same signal, but this time it's emitted by the proxy, not the proxied object.
    // Qt will then invoke all slots connected to the proxy's signal.
    QMetaObject::activate(this, signalOffset, localSignalIndex, args);

    // Return the success status.
    return -1;
}


//! Qt uses this method to check class inheritance in QObject::inherits().
void* QObjectProxy::qt_metacast(const char* className)
{
    if(!className)
        return nullptr;

    if(strcmp(className, "QObjectProxy") == 0)
        return static_cast<void*>(this);

    // Qt just compares the return value to nullptr, but doesn't cast it.
    // As a result, we unable to qobject_cast QObjectProxy* to the proxied class pointer.

    return QObject::qt_metacast(className);
}
