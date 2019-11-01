#ifndef LYNXUARTQT_H
#define LYNXUARTQT_H

#include "lynxiodevice.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QIODevice>
#include <QtDebug>
#include <QTime>

class LynxQtPortContainer : public QObject
{
    Q_OBJECT

    int _portIndex;
    QSerialPort _port;

public:
    explicit LynxQtPortContainer(QObject * parent = nullptr, int portIndex = -1) : QObject(parent), _portIndex(portIndex)
    { QObject::connect(&_port, SIGNAL(readyRead()), this, SLOT(readData())); }

//    void setPort(QSerialPortInfo port) { _port.setPort(port); }
//    void setPortName(QString portName) { _port.setPortName(portName); }
//    void setBaudRate(unsigned long baudRate) { _port.setBaudRate(qint32(baudRate)); }

    void setPortIndex(int index) { _portIndex = index; }

    QSerialPort & port() { return _port; }
    const QSerialPort & port() const { return _port; }

signals:
    void readyRead(int portIndex);

public slots:
    void readData() { emit readyRead(_portIndex); }
};

class LynxUartQt : public LynxIoDevice
{
public:
    LynxUartQt(LynxManager * const lynx = nullptr, int portIndex = -1);
    LynxUartQt(const LynxUartQt & other, int portIndex = -1);
    ~LynxUartQt() { this->close(); }

    const LynxUartQt & operator =(const LynxUartQt & other);

    bool open();
    bool open(int portContainer, unsigned long baudRate);
    bool open(const QSerialPortInfo & portContainer, unsigned long baudrate);
    void close();

    void setPortIndex(int index) { _port.setPortIndex(index); }

    LynxQtPortContainer & portContainer() { return _port; }
    const LynxQtPortContainer & portContainer() const { return _port; }
//    QSerialPort & port() { return _port; }
//    const QSerialPort & port() const { return _port; }

private:
    int read(int count = 1);
    void write();
    int bytesAvailable() const;
    uint32_t getMillis() const;

    LynxQtPortContainer _port;
    QTime _timer;

};

#endif // LYNXUARTQT_H
