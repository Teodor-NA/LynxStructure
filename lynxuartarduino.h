#ifndef LYNX_UART_ARDUINO
#define LYNX_UART_ARDUINO

#include "lynxiodevice.h"
#include <Arduino.h>

using namespace LynxLib;

class LynxUartArduino :
	public LynxIoDevice
{
public:
	LynxUartArduino(LynxManager & lynx) : LynxIoDevice(lynx) {};
	~LynxUartArduino() {};

	bool open(int port, unsigned long baudRate);
	void close();

private:
	int read(int count = 1);
	void write();
	int bytesAvailable() const;

	int _port;
};

#endif // !LYNX_UART_ARDUINO