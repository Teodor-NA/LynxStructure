#include "lynxuartarduino.h"

bool LynxUartArduino::open(int port, unsigned long baudRate)
{
	if (_open)
		this->close();

	switch (port)
	{
	case 0:
#ifdef HAVE_HWSERIAL0
	{
		Serial.begin(baudRate);
		_port = 0;
		_open = true;
	}
#endif // HAVE_HWSERIAL0
		break;
	case 1:
#ifdef HAVE_HWSERIAL1
	{
		Serial1.begin(baudRate);
		_port = 1;
		_open = true;
	} 
#endif // HAVE_HWSERIAL1
		break;
	case 2:
#ifdef HAVE_HWSERIAL2
	{
		Serial2.begin(baudRate);
		_port = 2;
		_open = true;
	}
#endif // HAVE_HWSERIAL2
		break;
	case 3:
#ifdef HAVE_HWSERIAL3
	{
		Serial3.begin(baudRate);
		_port = 3;
		_open = true;
	}
#endif // HAVE_HWSERIAL3
		break;
	default:
		break;
	}

	return _open;
}

void LynxUartArduino::close()
{
	if (!_open)
		return;

	switch (_port)
	{
	case 0:
	{
#ifdef HAVE_HWSERIAL0
		Serial.end();
#endif // HAVE_HWSERIAL0
	}	break;
	case 1:
	{
#ifdef HAVE_HWSERIAL1
		Serial1.end();
#endif // HAVE_HWSERIAL1
	}	break;
	case 2:
	{
#ifdef HAVE_HWSERIAL2
		Serial2.end();
#endif // HAVE_HWSERIAL2
	}	break;
	case 3:
	{
#ifdef HAVE_HWSERIAL3
		Serial3.end();
#endif // HAVE_HWSERIAL3
	}	break;
	default:
		break;
	}

	_open = false;
}

int LynxUartArduino::read(int count)
{
	if (!_open)
		return 0;

	int temp;

	switch (_port)
	{
	case 0:
#ifdef HAVE_HWSERIAL0
		for (int i = 0; i < count; i++)
		{
			temp = Serial.read();
			if (temp < 0)
				return i;

			_readBuffer.append(char(temp));
		}
		return count;
#else
		break;
#endif // HAVE_HWSERIAL0
	case 1:
#ifdef HAVE_HWSERIAL1
		for (int i = 0; i < count; i++)
		{
			temp = Serial1.read();
			if (temp < 0)
				return i;

			_readBuffer.append(char(temp));
		}
		return count;
#else
		break;
#endif // HAVE_HWSERIAL1
	case 2:
#ifdef HAVE_HWSERIAL2
		for (int i = 0; i < count; i++)
		{
			temp = Serial2.read();
			if (temp < 0)
				return i;

			_readBuffer.append(char(temp));
		}
		return count;
#else
		break;
#endif // HAVE_HWSERIAL2
	case 3:
#ifdef HAVE_HWSERIAL3
		for (int i = 0; i < count; i++)
		{
			temp = Serial3.read();
			if (temp < 0)
				return i;

			_readBuffer.append(char(temp));
		}
		return count;
#else
		break;
#endif // HAVE_HWSERIAL3
	default:
		break;
	}

	return 0;
}

//void LynxUartArduino::writeSerial()
//{
//	if (!_open)
//		return;
//
//	switch (_port)
//	{
//	case 0:
//#ifdef HAVE_HWSERIAL0
//		Serial.write(_writeBuffer, _writeBuffer.count());
//#endif // HAVE_HWSERIAL0
//		break;
//	case 1:
//#ifdef HAVE_HWSERIAL1
//		Serial1.write(data);
//#endif // HAVE_HWSERIAL1
//		break;
//	case 2:
//#ifdef HAVE_HWSERIAL2
//		Serial2.write(data);
//#endif // HAVE_HWSERIAL2
//		break;
//	case 3:
//#ifdef HAVE_HWSERIAL3
//	Serial3.write(data);
//#endif // HAVE_HWSERIAL3
//		break;
//	default:
//		break;
//	}
//
//}

void LynxUartArduino::write()
{
	if (!_open)
		return;

	switch (_port)
	{
	case 0:
#ifdef HAVE_HWSERIAL0
		Serial.write(reinterpret_cast<const uint8_t*>(_writeBuffer.data()), _writeBuffer.count());
#endif // HAVE_HWSERIAL0
		break;
	case 1:
#ifdef HAVE_HWSERIAL1
		Serial1.write(reinterpret_cast<const uint8_t*>(_writeBuffer.data()), _writeBuffer.count());
#endif // HAVE_HWSERIAL1
		break;
	case 2:
#ifdef HAVE_HWSERIAL2
		Serial2.write(reinterpret_cast<const uint8_t*>(_writeBuffer.data()), _writeBuffer.count());
#endif // HAVE_HWSERIAL2
		break;
	case 3:
#ifdef HAVE_HWSERIAL3
		Serial3.write(reinterpret_cast<const uint8_t*>(_writeBuffer.data()), _writeBuffer.count());
#endif // HAVE_HWSERIAL3
		break;
	default:
		break;
	}
}

int LynxUartArduino::bytesAvailable() const
{
	if (!_open)
		return 0;

	switch (_port)
	{
	case 0:
#ifdef HAVE_HWSERIAL0
		return Serial.available();
#else
		break;
#endif // HAVE_HWSERIAL0
	case 1:
#ifdef HAVE_HWSERIAL1
		return Serial1.available();
#else
		break;
#endif // HAVE_HWSERIAL1
	case 2:
#ifdef HAVE_HWSERIAL2
		return Serial2.available();
#else
		break;
#endif // HAVE_HWSERIAL2
	case 3:
#ifdef HAVE_HWSERIAL3
		return Serial3.available();
#else
		break;
#endif // HAVE_HWSERIAL3
	default:
		break;
	}

	return 0;
}