//-------------------------------------------------------------------------------------------
//------------------------------------- Version 1.0.1.3 -------------------------------------
//-------------------------------------------------------------------------------------------

#include "UartHandler.h"

#ifdef ARDUINO
UartHandler::UartHandler()
{
}

UartHandler::UartHandler(LynxLib::LynxHandler* lynxHandler)
{
	_lynxHandler = lynxHandler;
}

bool UartHandler::open(int port, int baudRate)
{
    if (_open)
        return false;

    _port = port;
    _open = true;

    switch (this->_port)
    {
    case 0:
    {
        Serial.begin(baudRate);
        break;
    }
    case 1:
    {
        Serial1.begin(baudRate);
        break;
    }
#ifdef ARDUINO_MEGA
    case 2:
    {
        Serial2.begin(baudRate);
        break;
    }
    case 3:
    {
        Serial3.begin(baudRate);
        break;
    }
#endif // ARDUINO_MEGA
    default:
        return false;
    }

    this->flush();

    return true;
}

void UartHandler::close()
{
    if (!_open)
        return;

    switch (this->_port)
    {
    case 0:
    {
        Serial.end();
        break;
    }
    case 1:
    {
        Serial1.end();
        break;
    }
#ifdef ARDUINO_MEGA
    case 2:
    {
        Serial2.end();
        break;
    }
    case 3:
    {
        Serial3.end();
        break;
    }
#endif // ARDUINO_MEGA
    }

    _open = false;
}

char UartHandler::read()
{
    if (!_open)
        return 0;

    switch (this->_port)
    {
    case 0:
    {
        return Serial.read();
    }
    case 1:
    {
        return Serial1.read();
    }
#ifdef ARDUINO_MEGA
    case 2:
    {
        return Serial2.read();
    }
    case 3:
    {
        return Serial3.read();
    }
#endif // ARDUINO_MEGA
    default:
        break;
    }

    return 0;
}

int UartHandler::read(LynxLib::LynxList<char>& buffer, int size)
{
    if (!_open)
        return -1;

	buffer.reserveAndCopy(size);

    switch (this->_port)
    {
    case 0:
	{
		for (int i = 0; i < size; i++)
		{
			buffer.append(Serial.read());
		}
		return size;
	}
    case 1:
    {
		for (int i = 0; i < size; i++)
		{
			buffer.append(Serial1.read());
		}
		return size;
    }
#ifdef ARDUINO_MEGA
    case 2:
    {
		for (int i = 0; i < size; i++)
		{
			buffer.append(Serial2.read());
	}
		return size;
    }
    case 3:
    {
		for (int i = 0; i < size; i++)
		{
			buffer.append(Serial3.read());
		}
		return size;
    }
#endif // ARDUNO_MEGA
    default:
        break;
    }

    return -1;
}

int UartHandler::write(const LynxLib::LynxList<char>& buffer)
{
	// buffer.reserveAndCopy(buffer.count());

    if (!_open)
        return -1;

    switch (this->_port)
    {
    case 0:
    {
        return Serial.write(&buffer.at(0), buffer.count());
    }
    case 1:
    {
        return Serial1.write(&buffer.at(0), buffer.count());
    }
#ifdef ARDUINO_MEGA
    case 2:
    {
        return Serial2.write(&buffer.at(0), buffer.count());
    }
    case 3:
    {
        return Serial3.write(&buffer.at(0), buffer.count());
    }
#endif // ARDUINO_MEGA
    default:
        break;
    }

    return -1;
}

int UartHandler::bytesAvailable()
{
    switch (this->_port)
    {
    case 0:
    {
        return Serial.available();
    }
    case 1:
    {
        return Serial1.available();
    }
#ifdef ARDUINO_MEGA
    case 2:
    {
        return Serial2.available();
    }
    case 3:
    {
        return Serial3.available();
    }
#endif // ARDUINO_MEGA
    default:
        break;
    }

    return -1;

}

void UartHandler::onNewData(const LynxLib::LynxID& id, int index)
{
	NewData::onNewUartData(id, index);
	_newData = false;
}

void UartHandler::flush()
{
    switch (this->_port)
    {
    case 0:
    {
        Serial.flush();
    }
    case 1:
    {
        Serial1.flush();
    }
#ifdef ARDUINO_MEGA
    case 2:
    {
        Serial2.flush();
    }
    case 3:
    {
        Serial3.flush();
    }
#endif // ARDUINO_MEGA
    default:
        break;
    }

}

#endif //ARDUINO

#ifdef QT_LYNX


InterruptObject::InterruptObject(UartHandler* handler, QObject* parent) : QObject(parent)
{
    _handler = handler;
}

void InterruptObject::update()
{
    if(_handler == LYNX_NULL)
        return;

    _handler->update();
}

void InterruptObject::newData(const LynxLib::LynxID& id, int index)
{
    emit onNewData(id, index);
}

UartHandler::UartHandler() :
    _interruptObject(this)
{

}

UartHandler::UartHandler(LynxLib::LynxHandler* lynxHandler) :
    _interruptObject(this)
{
    QObject::connect(&serialPort, SIGNAL(readyRead()), &_interruptObject, SLOT(update()));

    this->connectToLynx(lynxHandler);
}

void UartHandler::connectNewDataInterrupt(QObject* targetObject)
{
    QObject::connect(&(this->_interruptObject), SIGNAL(onNewData(const LynxLib::LynxID&, int)), targetObject, SLOT(onNewData(const LynxLib::LynxID&, int)));
}

bool UartHandler::open(int port, int baudRate)
{
    if(_open)
        return false;

    _port = port;

    QString temp = QString::asprintf("COM%d", _port);
    serialPort.setPortName(temp);

    if(!serialPort.setBaudRate(baudRate))
        return false;

    if(!serialPort.open(QIODevice::ReadWrite))
        return false;

    _open = true;
    this->flush();
    return true;
}

bool UartHandler::open(QSerialPortInfo port, int baudRate)
{
    if(_open)
        return false;

    serialPort.setPort(port);

    if(!serialPort.setBaudRate(baudRate))
        return false;
    if(!serialPort.open(QIODevice::ReadWrite))
        return false;

    _open = true;

    return true;
}

void UartHandler::close()
{
    if(_open)
        serialPort.close();

    _open = false;
}

char UartHandler::read()
{
    QByteArray temp = serialPort.read(1);
    return temp.at(0);
}

int UartHandler::read(LynxLib::LynxList<char>& buffer, int size)
{
    QByteArray temp = serialPort.read(size);

    // Do a non-destructive reserve in case the buffer is too small, to avoid unnessecary reallocation
    buffer.reserveAndCopy(temp.count() + buffer.count());

    for (int i = 0; i < temp.count(); i++)
    {
        buffer.append(temp.at(i));
    }

    return temp.count();
}

int UartHandler::write(const LynxLib::LynxList<char>& buffer)
{
    // qDebug() << "Bytes to write: " << size;
    return(int(serialPort.write(&(buffer.at(0)), buffer.count())));
}

int UartHandler::bytesAvailable()
{
    return(int(serialPort.bytesAvailable()));
}

void UartHandler::onNewData(const LynxLib::LynxID& id, int index)
{
    _interruptObject.newData(id, index);
}

void UartHandler::flush()
{
    serialPort.flush();
}

#endif //QT_LYNX

#ifdef TI
void UartHandler::onNewData(const LynxLib::LynxID &id,int index)
{

    NewData::onNewUartData(id,index);

}
UartHandler::UartHandler()
{
    // TODO MAGNUS
    // Write here if you need something in the constructor

    // Clear buffer
//    for(int i = 0; i < DATABUFFER_SIZE; i++)
//    {
//        _dataBuffer[i] = 0;
//    }
    rxBuffer.init(DATABUFFER_SIZE);
    txBuffer.init(DATABUFFER_SIZE);
}
void UartHandler::init(SCI_Handle _sciHandle,CLK_Handle _clkHandle)
{
    this->sciHandle = _sciHandle;
    this->clkHandle=_clkHandle;
}
bool UartHandler::open(int port, int baudRate)
{
    if(_open)
        return false;

    _port = port;

    // TODO MAGNUS
    // Open serial port "serPort" with baud-rate "baudRate" inclde switch case for baud
    // Return true if success, false if failure
    // enable the SCI-A clock
    CLK_enableSciaClock(clkHandle);
    //
    //    // 1 stop bit,  No loopback
    //    // No parity,8 char bits,
    //    // async mode, idle-line protocol
    SCI_disableParity(sciHandle);
    SCI_setNumStopBits(sciHandle, SCI_NumStopBits_One);
    SCI_setCharLength(sciHandle, SCI_CharLength_8_Bits);

    // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    SCI_enableTx(sciHandle);
    SCI_enableRx(sciHandle);
    SCI_enableTxInt(sciHandle);
    SCI_enableRxInt(sciHandle);

    //SCI_enableLoopBack(sciHandle);

    // SCI BRR = LSPCLK/(SCI BAUDx8) - 1
    SCI_setBaudRate(sciHandle, SCI_BaudRate_e(baudRate));

    SCI_enable(sciHandle);
    //
    //    return;
    //}
    //
    //void scia_fifo_init()
    //{
    SCI_enableFifoEnh(sciHandle);
    SCI_resetTxFifo(sciHandle);
    SCI_clearTxFifoInt(sciHandle);
    SCI_resetChannels(sciHandle);
    SCI_setTxFifoIntLevel(sciHandle, SCI_FifoLevel_2_Words);
    SCI_enableTxFifoInt(sciHandle);

    SCI_resetRxFifo(sciHandle);
    SCI_clearRxFifoInt(sciHandle);
    SCI_setRxFifoIntLevel(sciHandle, SCI_FifoLevel_2_Words);
    SCI_enableRxFifoInt(sciHandle);
    _open=true;
    return true;//success

}
void UartHandler::close()
{
    if(!_open)
        return;

    SCI_disableTx(sciHandle);
    SCI_disableRx(sciHandle);
    SCI_disableTxInt(sciHandle);
    SCI_disableRxInt(sciHandle);
    SCI_disable(sciHandle);
    _open=false;

}
char UartHandler::read()
{
    // TODO MAGNUS
    // Read a single byte from the serial port and return it
    if(!_open)
        return 0;
    return rxBuffer.read();
}

int UartHandler::read(LynxLib::LynxList<char>& buffer, int size)
{
    // TODO MAGNUS
    // Read "size" number of bytes from the serial port and put them in "buffer".
    // Return number of bytes read, or a negative number if error
    if(!_open)
           return -1;
    return rxBuffer.read(buffer,size);
}

int UartHandler::write(const LynxLib::LynxList<char>& buffer)
{
    // TODO MAGNUS
    // Write "size" number of bytes from "buffer" to the serial port.
    // Return number of bytes written, or a negative number if error
    //for (int i = 0; i < size; ++i)
    if(!_open)
           return -1;

    txBuffer.write(&buffer.at(0), buffer.count());

    return buffer.count();
}

int UartHandler::bytesAvailable()
{
    // TODO MAGNUS
    // Probe the serial port for incoming bytes.
    // Return number of incoming bytes
    return rxBuffer.count();
}

void UartHandler::flush()
{
    // TODO MAGNUS
    // Flush internal TI uart buffer (the ringbuffer)
}

#endif //TI

bool UartHandler::opened()
{
    return _open;
}

void UartHandler::update(UartHandler* uartHandler)
{
    uartHandler->update();
}

// int iteration = 0;

void UartHandler::update()
{

    if (!_open || (_lynxHandler == LYNX_NULL))
    {
        _errorList.write(-20);
        return;
    }

    if (this->bytesAvailable() < 0)
    {
        _errorList.write(-21);
        _errorCounter++;
        return;
    }

    if(_state == eIdle)
    {
        if (this->bytesAvailable() > 0)
        {
            _readBuffer.clear();
            _receivedBytes = 0;
            _state = eScanning;
        }

     }

    if(_state == eScanning)
    {
		if (this->bytesAvailable() < 1)
			return;

        int shuffleCount = 0;
        do
        {
            if(_shuffleBytes)
            {
                shuffleCount++;
                _readBuffer[0] = _readBuffer.at(1);
                _readBuffer[1] = _readBuffer.at(2);
                _readBuffer[2] = read();
                _receivedBytes++;
                _shuffleBytes = false;
            }
            else
            {
                if(this->bytesAvailable() < LYNX_ID_BYTES)
                    return;

                _receivedBytes += this->read(_readBuffer, LYNX_ID_BYTES);
            }

            _tempID.deviceID = static_cast<uint8_t>(_readBuffer.at(0));
            _tempID.structTypeID = static_cast<uint8_t>(_readBuffer.at(1));
            _tempID.structInstanceID = static_cast<uint8_t>(_readBuffer.at(2));

            if(_lynxHandler->isMember(_tempID))
            {
                _state = eIndexing;
            }
            else if(_tempID.deviceID == static_cast<uint8_t>(LYNX_INTERNAL_DATAGRAM))
            {
                _readSize = _lynxHandler->getTransferSize(_tempID);
                if(_readSize > 0)
                {
                    _readSize = _readSize + LYNX_CHECKSUM_BYTES;
                    _state = eReading;
                }
                else
                {
                    _errorList.write(-11);
                    _shuffleBytes = true;
                    _errorCounter++;
                }
            }
            else
            {
                _errorList.write(-11);
                _shuffleBytes = true;
                _errorCounter++;
			}
            
        }while ((this->bytesAvailable() > 0) && _shuffleBytes);
    }

    if(_state == eIndexing)
    {
        if(this->bytesAvailable() < LYNX_INDEXER_BYTES)
            return;

        _receivedBytes += this->read(_readBuffer, LYNX_INDEXER_BYTES);

        _lynxIndex = (int(_readBuffer.at(LYNX_ID_BYTES)) & int(0xFF)) | ((int(_readBuffer.at(LYNX_ID_BYTES + 1)) & int(0xFF)) << 8);

        _readSize = _lynxHandler->getTransferSize(_tempID, _lynxIndex - 1);

        if(_readSize <= 0)
        {
            _errorList.write(_readSize);
            _errorCounter++;
            _state = eIdle;
            return;
        }

        _readSize += LYNX_CHECKSUM_BYTES;
        _state = eReading;
    }

    if(_state == eReading)
    {
        if (this->bytesAvailable() >= _readSize)
        {
            _receivedBytes += this->read(_readBuffer, _readSize);
            int bytesTransferred = _lynxHandler->fromBuffer(_readBuffer);

            _state = eIdle;
            if (bytesTransferred < 1)
            {
                _errorList.write(bytesTransferred);
                _errorCounter++;
                return;
            }

            _newData = true;
            this->onNewData(_tempID, (_lynxIndex - 1));
        }
    }
}

int UartHandler::send(const LynxLib::LynxID & _lynxID, int subIndex)
{
    if(!_open || (_lynxHandler == LYNX_NULL))
    {
        _errorList.write(-20);
        return -20;
    }

    int size;

    size = _lynxHandler->toBuffer(_lynxID, _writeBuffer, subIndex);

    if(size > 0)
    {
        _sentBytes = this->write(_writeBuffer);
        if(_sentBytes < 0)
            _errorList.write(_sentBytes);

        return  _sentBytes;
    }

    _errorList.write(size);
    return size;
}

bool UartHandler::newData()
{
    bool temp = this->_newData;
    _newData = false;
    return temp;
}

LynxLib::LynxList<int> UartHandler::getErrorList()
{
    int errorCount = _errorList.count();
    LynxLib::LynxList<int> errorList(errorCount);

    for(int i = 0; i < errorCount; i++)
    {
        errorList.append(_errorList.read());
    }

    return errorList;
}

int UartHandler::sendDirect(const char* data, int size)
{
    LynxLib::LynxList<char> temp(data, size);
    return this->write(temp);
}

int UartHandler::sendString(const LynxLib::LynxString & str)
{
	if (!_open)
        return -20;

    if(str.count() < 1)
        return -2;

    _writeBuffer.clear();
    _writeBuffer.append(LYNX_INTERNAL_DATAGRAM);
    _writeBuffer.append(static_cast<char>(LynxLib::eLynxString));

    int writeSize = (str.count() > 254) ? 255 : (str.count() + 1);

    _writeBuffer.append(static_cast<char>(writeSize));
    _writeBuffer.append(str.toCstr(), writeSize - 1);
    _writeBuffer.append('\0');

    char checksum = 0;
    for (int i = 0; i < _writeBuffer.count(); i++)
    {
        checksum += _writeBuffer.at(i);
    }
    _writeBuffer.append(checksum);

    return this->write(_writeBuffer);
}

int UartHandler::sendString(const char * cstr, int size)
{
    LynxLib::LynxString temp(cstr, size);
    return this->sendString(temp);
}

int UartHandler::getError()
{
	return _errorList.read();
}

