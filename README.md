# QObjectProxy
QObjectProxy is a signal and slot proxy.

When the proxied object emits a signal, the proxy re-emits the same signal itself. When you invoke a slot on the proxy, it invokes the same slot on the proxied object.

The proxied object and the proxy must live in the same thread.

It does not proxy the signals and slots of QObject.

# Usage

```.cpp
class ProxiedClass : public QObject
{
    Q_OBJECT
public:
    ProxiedClass(QString name) : _name(name)
    {}

public slots:
    void someSlot(QString text) { qInfo() << QString{"%1: %2"}.arg(_name).arg(text); }

signals:
    void someSignal(QString text);

private:
    QString _name;
};

ProxiedClass* p1 = new ProxiedClass{"One"};
ProxiedClass* p2 = new ProxiedClass{"Two"};

QObjectProxy* proxy = new QObjectProxy{&ProxiedClass::staticMetaObject};
proxy->setObject(p1);
```

We emit the signal from p1, proxy re-emits it, p2 receives it.
Output: "Two: emitted from p1".

```.cpp
QObject::connect(proxy, SIGNAL(someSignal(QString)), p2, SLOT(someSlot(QString)), Qt::DirectConnection);
emit p1->someSignal("emitted from p1");
```

We invoke the slot on proxy, proxy invokes it on p1.
Output: "One: invoked on proxy".

```.cpp
QMetaObject::invokeMethod(proxy, "someSlot", Qt::DirectConnection, Q_ARG(QString, "invoked on proxy"));
```
