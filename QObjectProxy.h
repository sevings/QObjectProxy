#pragma once

#include <QObject>
#include <QPointer>

//! \brief QObjectProxy is a proxy for signals and slots.
//! When the proxied object emits a signal, the proxy re-emits the same signal itself.
//! When you invoke a slot on the proxy, it invokes the same slot on the proxied object.
//! The proxied object and the proxy must live in the same thread.
//! It doesn't proxy the signals and slots of QObject.
class QObjectProxy : public QObject
{
public:
    QObjectProxy(const QMetaObject* meta, QObject* parent = nullptr);
    ~QObjectProxy();

    void setObject(QObject* obj);

    void clear() { setObject(nullptr); }

    QObject* object() const { return _obj; }
    bool     isNull() const { return !_obj; }

    const QMetaObject* metaObject() const override { return &_meta; }

    int   qt_metacall(QMetaObject::Call call, int id, void** args) override;
    void* qt_metacast(const char* className) override;

private:
    QMetaObject       _meta;
    QPointer<QObject> _obj;
};
