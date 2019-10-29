#ifndef LYNXUARTQT_H
#define LYNXUARTQT_H

#include "lynxiodevice.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QIODevice>
#include <QtDebug>
#include <QTime>

class LynxUartQt : public LynxIoDevice
{
public:
    LynxUartQt(LynxManager * const lynx = nullptr);
    LynxUartQt(const LynxUartQt & other);
    ~LynxUartQt() { this->close(); }

    const LynxUartQt & operator =(const LynxUartQt & other);

    bool open();
    bool open(int port, unsigned long baudRate);
    bool open(const QSerialPortInfo & port, unsigned long baudrate);
    void close();

    QSerialPort & port() { return _port; }
    const QSerialPort & port() const { return _port; }

private:
    int read(int count = 1);
    void write();
    int bytesAvailable() const;
    uint32_t getMillis() const;

    QSerialPort _port;
    QTime _timer;

};

#endif // LYNXUARTQT_H
