/****************************************************************************
** Meta object code from reading C++ file 'mega_httpdownthread.h'
**
** Created: Sun Apr 3 21:40:48 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mega_httpdownthread.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mega_httpdownthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HttpDownThread[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      33,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HttpDownThread[] = {
    "HttpDownThread\0\0start_download()\0"
    "suspend_download()\0"
};

const QMetaObject HttpDownThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_HttpDownThread,
      qt_meta_data_HttpDownThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HttpDownThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HttpDownThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HttpDownThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HttpDownThread))
        return static_cast<void*>(const_cast< HttpDownThread*>(this));
    return QThread::qt_metacast(_clname);
}

int HttpDownThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: start_download(); break;
        case 1: suspend_download(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
