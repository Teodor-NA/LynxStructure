#include "lynxuartqt.h"

LynxUartQt::LynxUartQt(LynxManager * const lynx, int portIndex) : LynxIoDevice(lynx)
{
    _port.setPortIndex(portIndex);
    _timer.start();
}

LynxUartQt::LynxUartQt(const LynxUartQt & other, int portIndex) : LynxIoDevice(other._lynx)
{
    _port.setPortIndex(portIndex);
    _port.port().setPortName(other._port.port().portName());
    _port.port().setBaudRate(other._port.port().baudRate());
//    qDebug() << "Port rebuilt with:";
//    qDebug() << "Name:" << _port.port().portName();
//    qDebug() << "Baud Rate:" << _port.port().baudRate() << "\n";
    _open = _port.port().isOpen();

    _timer.start();
}

const LynxUartQt & LynxUartQt::operator =(const LynxUartQt & other)
{
    if (&other == this)
        return *this;

    this->_lynx = other._lynx;
    _port.port().setPortName(other._port.port().portName());
    _port.port().setBaudRate(other._port.port().baudRate());
//    qDebug() << "Port rebuilt with:";
//    qDebug() << "Name:" << _port.port().portName();
//    qDebug() << "Baud Rate:" << _port.port().baudRate() << "\n";
    _open = _port.port().isOpen();

    return *this;
}

bool LynxUartQt::open()
{
    if(_open)
        this->close();

    _open = _port.port().open(QSerialPort::ReadWrite);

    return _open;
}

bool LynxUartQt::open(int port, unsigned long baudRate)
{
    if(_open)
        this->close();

    QString portName = QString::asprintf("COM%i", port);

    _port.port().setPortName(portName);
    _port.port().setBaudRate(int(baudRate));

    _open = _port.port().open(QSerialPort::ReadWrite);

    return _open;
}

bool LynxUartQt::open(const QSerialPortInfo & port, unsigned long baudRate)
{
    if(_open)
        this->close();

    _port.port().setPort(port);
    _port.port().setBaudRate(int(baudRate));

    _open = _port.port().open(QSerialPort::ReadWrite);

    return _open;
}

void LynxUartQt::close()
{
    _port.port().close();
    _open = false;
}

int LynxUartQt::read(int count)
{
    if(!_open)
        return 0;


    QByteArray temp = _port.port().read(count);
    qDebug()<<temp;

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

    _port.port().write(_writeBuffer.data(), _writeBuffer.count());
}

int LynxUartQt::bytesAvailable() const
{
    if(!_open)
        return 0;

    return static_cast<int>(_port.port().bytesAvailable());
}

uint32_t LynxUartQt::getMillis() const
{
    return uint32_t(_timer.elapsed());
}
