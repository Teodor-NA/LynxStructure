//-------------------------------------------------------------------------------------------
//------------------------------------- Version 1.0.1.3 -------------------------------------
//-------------------------------------------------------------------------------------------

#pragma once



#include "LynxStructure.h"

#define QT_LYNX
#ifdef TI
#undef QT_LYNX

#endif // TI

#ifdef ARDUINO
#undef QT_LYNX
#include <arduino.h>
#define ARDUINO_MEGA
class NewData
{
public:
	static void onNewUartData(const LynxLib::LynxID& id, int index);
};
#endif //ARDUINO

#ifdef QT_LYNX
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
// #include <QDebug>



class UartHandler;

class InterruptObject : public QObject
{
    Q_OBJECT

    UartHandler* _handler = LYNX_NULL;

public:
    explicit InterruptObject(UartHandler* handler, QObject* parent = LYNX_NULL);
    void newData(const LynxLib::LynxID& id, int index);

signals:
    void onNewData(const LynxLib::LynxID& id, int index);

public slots:
    void update();
};

#endif //QT_LYNX

#ifdef TI
// TODO MAGNUS
// Write here if you need includes
#define DATABUFFER_SIZE 100
#include "DSP28x_Project.h"
#include   "f2802x_common/include/sci.h"
#include   "f2802x_common/include/clk.h"
class NewData
{
public:
    static void onNewUartData(const LynxLib::LynxID& id,int index);
};
#endif // TI

enum E_State
{
    eIdle = 0,
    eScanning,
    eIndexing,
    eReading,
    eDone
};

class UartHandler
{
public:
    UartHandler();

    UartHandler(LynxLib::LynxHandler* lynxHandler);

    // Opens the serial connection. Returns true if it was opened successfully.
    bool open(int port, int baudRate);

    // Closes the serial connection
    void close();

    // Returns connection state of port
    bool opened();

    // Links between received data and lynx. Run this as often as possible or on interrupt from the serial port
    static void update(UartHandler* uartHandler);
    void update();

    // Sends content of the lynx id to the serial port.
    // If subindex is not entered or less than zero all variables will be sent, otherwise only the variable with the specified subindex will be sent.
    int send(const LynxLib::LynxID& lynxID, int subIndex = -1);

    // Sends data directly to the serial port
    int sendDirect(const char* data, int size);

    // Send a string
    int sendString(const LynxLib::LynxString & str);

    // Send a string
    int sendString(const char * cstr, int size = -1);

    // Returns true if new data has been received and added to lynx
    bool newData();

    // Triggered when new data is received
    void onNewData(const LynxLib::LynxID& id, int index);

//    // returns one character from the buffer at "index"
//    char bufferAt(int index);

    // Returns number of communication errors since strtup
    int errorCount() const { return _errorList.count(); }

    // Connect to lynxhandler
    void connectToLynx(LynxLib::LynxHandler* lynxHandler) { _lynxHandler = lynxHandler; }

    // Returns how many bytes were received in the last datagram, resets to 0 when read;
    int bytesRead() { int temp = _receivedBytes; _receivedBytes = 0; return temp; }

    // Returns how many bytes were sent in the last datagram, resets to 0 when read;
    int bytesWritten() { int temp = _sentBytes; _sentBytes = 0; return temp; }

    // Flushes the internal databuffer
    void flush();

	// Gets the last received ID
	const LynxLib::LynxID& lastReceivedId() const { return _tempID; }

    int receiveBufferCount() const{return _readBuffer.count();}
    int sendBufferCount() const{return _writeBuffer.count();}

    char receiveBufferAt(int index) const{return _readBuffer.at(index);}
    char sendBufferAt(int index) const{return _writeBuffer.at(index);}
//    int bufferCount() { return _dataBuffer.count(); }

//    int reservedBuffer() { return _dataBuffer.reservedSize(); }

    LynxLib::LynxList<int> getErrorList();
	int getError();
    // Returns number of bytes on port
    int bytesAvailable();
private:
    // Reads a single character from serial and returns it
    char read();

    // Reads "size" bytes from the serial port, and puts it in buffer
    int read(LynxLib::LynxList<char>& buffer, int size);

    // Writes the buffer to the serial port
    int write(const LynxLib::LynxList<char>& buffer);



    E_State _state = eIdle;
    bool _newData = false;
    int _errorCounter = 0;
    int _port;
    LynxLib::LynxList<char> _readBuffer;
    LynxLib::LynxList<char> _writeBuffer;

    LynxLib::LynxRingBuffer<int> _errorList = LynxLib::LynxRingBuffer<int>(10, LynxLib::LynxRingBuffer<int>::eAllowOverflow);

    bool _open = false;
    LynxLib::LynxID _tempID;
    int _lynxIndex;
    bool _shuffleBytes = false;

    int _transferBytes = 0;
    int _readSize = 0;

    int _receivedBytes = 0;
    int _sentBytes = 0;

	LynxLib::LynxList<LynxLib::LynxID> _idList;

    LynxLib::LynxHandler* _lynxHandler = LYNX_NULL;

#ifdef QT_LYNX
public:
    bool open(QSerialPortInfo port, int baudRate);
    // void connecUpdatetInterrupt(QObject* targetObject);
    // void onNewData(BackEnd* target);
    void connectNewDataInterrupt(QObject* targetObject);
private:
    // BackEnd* _backEnd = LYNX_NULL;
    QSerialPort serialPort;

    InterruptObject _interruptObject;

#endif //QT_LYNX

#ifdef TI
public:
    LynxLib::LynxRingBuffer <char>txBuffer;
    LynxLib::LynxRingBuffer <char>rxBuffer;
    void init(SCI_Handle _sciHandle,CLK_Handle _clkHandle);
    //UartHandler(SCI_Handle _sciHandle,CLK_Handle _clkHandle);
private:
    SCI_Handle sciHandle;
    CLK_Handle clkHandle;
    // int writeBytes(const char* buffer, int size,SCI_Handle sciHandle);
#endif //TI
};


