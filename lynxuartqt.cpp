#include "lynxuartqt.h"

LynxUartQt::LynxUartQt(LynxManager * const lynx) : LynxIoDevice(lynx)
{
    _timer.start();
}

LynxUartQt::LynxUartQt(const LynxUartQt & other) : LynxIoDevice(other._lynx)
{
    _port.setPort(QSerialPortInfo(other._port));

    _open = _port.isOpen();

    _timer.start();
}

const LynxUartQt & LynxUartQt::operator =(const LynxUartQt & other)
{
    if (&other == this)
        return *this;

    this->_lynx = other._lynx;

    _port.setPort(QSerialPortInfo(other._port));

    return *this;
}

bool LynxUartQt::open()
{
    if(_open)
        this->close();

    _open = _port.open(QSerialPort::ReadWrite);

    return _open;
}

bool LynxUartQt::open(int port, unsigned long baudRate)
{
    if(_open)
        this->close();

    QString portName = QString::asprintf("COM%i", port);

    _port.setPortName(portName);
    _port.setBaudRate(int(baudRate));

    _open = _port.open(QSerialPort::ReadWrite);

    return _open;
}

bool LynxUartQt::open(const QSerialPortInfo & port, unsigned long baudRate)
{
    if(_open)
        this->close();

    _port.setPort(port);
    _port.setBaudRate(int(baudRate));

    _open = _port.open(QSerialPort::ReadWrite);

    return _open;
}

void LynxUartQt::close()
{
    _port.close();
    _open = false;
}

int LynxUartQt::read(int count)
{
    if(!_open)
        return 0;


    QByteArray temp = _port.read(count);

    for (int i = 0; i < temp.count(); i++)
    {
        _readBuffer.append(temp.at(i));
    }

    return temp.count();
}

void LynxUartQt::write()
{
    if(!_open)
        return;

    _port.write(_writeBuffer.data(), _writeBuffer.count());
}

int LynxUartQt::bytesAvailable() const
{
    if(!_open)
        return 0;

    return static_cast<int>(_port.bytesAvailable());
}

uint32_t LynxUartQt::getMillis() const
{
    return uint32_t(_timer.elapsed());
}
