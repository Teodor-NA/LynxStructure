#include "lynxiodevice.h"

LynxIoDevice::LynxIoDevice(LynxManager & lynx) :
    _state(LynxLib::eFindHeader),
    _open(false),
    _lynx(&lynx)
{
}


LynxIoDevice::~LynxIoDevice()
{
}


const LynxInfo & LynxIoDevice::update()
{
    _updateInfo.state = LynxLib::eNoChange;

	if (this->bytesAvailable() < 1)
		return _updateInfo;

	if (_state == LynxLib::eFindHeader)
	{
        // _lastID = LynxID();

        while (this->bytesAvailable() && (_state == LynxLib::eFindHeader))
		{
			_readBuffer.clear();
            this->read();
			if (_readBuffer.at(0) == LYNX_STATIC_HEADER)
                _state = LynxLib::eFindStructId;
		}
	}

    if(_state == LynxLib::eFindStructId)
    {
        if(this->bytesAvailable())
        {
            this->read();

			if (_readBuffer.at(1) == LYNX_INTERNALS_HEADER)
			{
				_state = LynxLib::eInternals;
			}
			else
			{
				_updateInfo.lynxId.structIndex = _lynx->findId(_readBuffer.at(1));

				if (_updateInfo.lynxId.structIndex < 0)
				{
					_state = LynxLib::eFindHeader;
					_updateInfo.state = LynxLib::eStructIdNotFound;
					return _updateInfo;
				}
				else
				{
					_state = LynxLib::eGetInfo;
				}
			}
        }
    }

	if (_state == LynxLib::eInternals)
	{
		if (this->bytesAvailable() > 0)
		{
			this->read();

			switch (_readBuffer.at(2))
			{
			case LynxLib::eDeviceInfo:
				_state = LynxLib::eGetDeviceInfo;
				break;
			case LynxLib::eScan:
				_state = LynxLib::eGetScan;
				break;
			default:
				_updateInfo.state = LynxLib::eStructIdNotFound;
				_state = LynxLib::eFindHeader;
				return _updateInfo;
			}
		}
	}

	if (_state == LynxLib::eGetScan)
	{
		if (this->bytesAvailable() > 0)
		{
			this->read();
			if (LynxLib::checkChecksum(_readBuffer))
			{
				this->sendDeviceInfo();
				_updateInfo.state = LynxLib::eScanReceived;
			}
			else
			{
				_updateInfo.state = LynxLib::eWrongChecksum;
			}
			_state = LynxLib::eFindHeader;
			return _updateInfo;
		}
	}

	if (_state == LynxLib::eGetDeviceInfo)
	{
		// ------------------------ Frame ----------------------------
		// -----------------------------------------------------------
		// |  Description   |    Size    |     Index    |  Contents  |
		// -----------------------------------------------------------
		// | Static header  |     1      |       0      |    'A'     |
		// |   Struct Id    |     1      |       1      |    255     |
		// |  Int. data id  |     1      |       2      |     1      |
		// |  Data length   |     2      |     3 -> 4   | 0 -> 65535 |
		// |  Device Data   |     a      | 5 -> (n - 2) |     -      |
		// |   Checksum     |     1      |    (n - 1)   |  0 -> 255  |
		// -----------------------------------------------------------
		// a = Data length
		// n = 6 + a + 1 (total length)

		if (this->bytesAvailable() >= 2)
		{
			this->read(2);

			int low = (int(_readBuffer.at(3)) & 0xff);
			int high = int(_readBuffer.at(4));
			high = ((high << 8) & 0xff00);

			_updateInfo.dataLength = ( low | high );

			_transferLength = _updateInfo.dataLength + LYNX_CHECKSUM_BYTES;

			_state = LynxLib::eGetDeviceData;
		}

	}

	if (_state == LynxLib::eGetDeviceData)
	{
		if (this->bytesAvailable() >= _transferLength)
		{
			_readBuffer.resize(_readBuffer.count() + _transferLength);

			read(_transferLength);

			//char checksum = 0;
			//for (int i = 0; i < (_readBuffer.count() - 1); i++)
			//{
			//	checksum += _readBuffer.at(i);
			//}

			if (!LynxLib::checkChecksum(_readBuffer))
			{
				_updateInfo.state = LynxLib::eWrongChecksum;
				_state = LynxLib::eFindHeader;
				return _updateInfo;
			}

			this->readDeviceInfo();

			_updateInfo.deviceId = _deviceInfo.deviceId;
			_updateInfo.state = LynxLib::eNewDeviceInfoReceived;
			_state = LynxLib::eFindHeader;
		}
	}

    if (_state == LynxLib::eGetInfo)
	{
        if (this->bytesAvailable() >= 3)
		{
            this->read();
            _updateInfo.lynxId.variableIndex = int(_readBuffer.at(2)) - 1;
            if(_updateInfo.lynxId.variableIndex > _lynx->structVariableCount(_updateInfo.lynxId.structIndex))
            {
                _state = LynxLib::eFindHeader;
                _updateInfo.state = LynxLib::eVariableIndexOutOfBounds;
                return _updateInfo;
            }

            this->read();
            _updateInfo.dataLength = int(_readBuffer.at(3));
            //if(_updateInfo.dataLength != _lynx->transferSize(_updateInfo.lynxId))
            //{
            //    _state = eFindHeader;
            //    _updateInfo.state = eWrongDataLength;
            //    return _updateInfo;
            //}

            this->read();
            _updateInfo.deviceId = _readBuffer.at(4);

			_transferLength = _updateInfo.dataLength + LYNX_CHECKSUM_BYTES;

            _state = LynxLib::eGetData;
		}
	}

    if (_state == LynxLib::eGetData)
	{
		if (this->bytesAvailable() >= _transferLength)
		{
			_readBuffer.resize(_readBuffer.count() + _transferLength);

            read(_transferLength);

            _lynx->fromArray(_readBuffer, _updateInfo);

			_state = LynxLib::eFindHeader;
		}
	}

	return _updateInfo;
}

void LynxIoDevice::periodicTransmitUpdate()
{
	_currentTime = this->getMillis();

	int overtime;
	for (int i = 0; i < _periodicTransmits.count(); i++)
	{
		overtime = (_currentTime - _periodicTransmits.at(i).previousTimeStamp) - _periodicTransmits.at(i).timeInterval;
		if (overtime >= 0)
		{
			_periodicTransmits[i].previousTimeStamp = _currentTime - overtime;
			this->send(LynxId(_periodicTransmits.at(i)));
		}
	}
}

LynxLib::E_LynxState LynxIoDevice::send(const LynxId & lynxId)
{
	LynxLib::E_LynxState state = _lynx->toArray(_writeBuffer, lynxId);
	
	if (state != LynxLib::eDataCopiedToBuffer)
		return state;

    this->write();

	return state;
}

int LynxIoDevice::sendDeviceInfo()
{
	LynxDeviceInfo deviceInfo;

	_lynx->getInfo(deviceInfo);

	int dataLength = deviceInfo.description.count();		// Description (max 255)
	dataLength += deviceInfo.lynxVersion.count();			// Version (max 255 (should not happen though, but you never know...))
	dataLength += 4;										// Device Id + Description length + Version length + struct count;
		
	for (int i = 0; i < deviceInfo.structs.count(); i++) 
	{
		dataLength += deviceInfo.structs.at(i).description.count();			// Description (max 255)
		dataLength += 3;													// structId + variableCount + Description length
			
		for (int j = 0; j < deviceInfo.structs.at(i).variables.count(); j++)
		{																	
			dataLength += deviceInfo.structs.at(i).variables.at(j).description.count();	// Description (max 255)
			dataLength += 3;															// index + data type + desc. length
		}
	}

	_writeBuffer.reserve(dataLength + LYNX_HEADER_BYTES + LYNX_CHECKSUM_BYTES);

	// ---------------------------- Header --------------------------------------

	// ------------------------ Frame ----------------------------
	// -----------------------------------------------------------
	// |  Description   |    Size    |     Index    |  Contents  |
	// -----------------------------------------------------------
	// | Static header  |     1      |       0      |    'A'     |
	// |   Struct Id    |     1      |       1      |    255     |
	// |  Int. data id  |     1      |       2      |     1      |
	// |  Data length   |     2      |     3 -> 4   | 0 -> 65535 |
	// |  Device Data   |     a      | 5 -> (n - 2) |     -      |
	// |   Checksum     |     1      |    (n - 1)   |  0 -> 255  |
	// -----------------------------------------------------------
	// a = Data length
	// n = 6 + a + 1 (total length)

	_writeBuffer.append(LYNX_STATIC_HEADER);					// lynx header
	_writeBuffer.append(LYNX_INTERNALS_HEADER);					// Internal datagram
	_writeBuffer.append(char(LynxLib::eDeviceInfo));	// Internal type
	int low = dataLength & 0xff;
	int high = (dataLength >> 8) & 0xff;
	_writeBuffer.append(char(low));				// Data Length (Low)
	_writeBuffer.append(char(high));		// Data Length (High)

	// ---------------------------- Device --------------------------------------

	// ------------------------ Device Data -------------------------
	// --------------------------------------------------------------
	// |    Description    | Size |        Index       |  Contents  |
	// --------------------------------------------------------------
	// |   Device Id       |  1   |          5         |  0 -> 255  |
	// | Device desc. len  |  1   |          6         |  0 -> 255  |
	// |   Device desc.    |  b   |    7 -> (B - 1)    |     -      |
	// |   Version len     |  1   |          B         |  0 -> 255  |
	// |     Version       |  c   | (B + 1) -> (C - 1) |     -      |
	// |   Struct count    |  1   |          C         |  0 -> 255  |
	// |   Struct data     |  -   | (C + 1) -> (n - 2) |     -      |
	// --------------------------------------------------------------
	// b = Device desc. len
	// B = 7 + b
	// c = Version len
	// C = c + d

	_writeBuffer.append(deviceInfo.deviceId);					// Device id
	_writeBuffer.append(char(deviceInfo.description.count()));	// Device desc. length
	// deviceInfo.description.toByteArray(_writeBuffer);			// Device desc.
	_writeBuffer.fromCharArray(deviceInfo.description.toCharArray(), deviceInfo.description.count());
	_writeBuffer.append(char(deviceInfo.lynxVersion.count()));	// Version length
	// deviceInfo.lynxVersion.toByteArray(_writeBuffer);			// Version
	_writeBuffer.fromCharArray(deviceInfo.lynxVersion.toCharArray(), deviceInfo.lynxVersion.count());
	_writeBuffer.append(char(deviceInfo.structCount));			// Struct count
		
	// ---------------------------- Structs --------------------------------------
	for (int i = 0; i < deviceInfo.structs.count(); i++)
	{
		// ------------------------ Struct Data -------------------------
		// --------------------------------------------------------------
		// |    Description    | Size |        Index       |  Contents  |
		// --------------------------------------------------------------
		// |    Struct Id      |  1   |          k         |  0 -> 255  |
		// | Struct desc. len  |  1   |        k + 1       |  0 -> 255  |
		// |   Struct desc.    |  d   | (k + 2) -> (D - 1) |     -      |
		// |  Variable count   |  1   |       (D + 1)      |  0 -> 255  |
		// |  Variable data    |  -   |    (D + 2) -> E    |     -      |
		// --------------------------------------------------------------
		// k = C + 1 + i * struct size (variable) | where i is the struct indexer
		// d = Struct desc. len
		// D = k + 2 + d

		_writeBuffer.append(deviceInfo.structs.at(i).structId);						// Struct id
		_writeBuffer.append(char(deviceInfo.structs.at(i).description.count()));	// Struct desc. length
		// deviceInfo.structs.at(i).description.toByteArray(_writeBuffer);				// Struct desc.
		_writeBuffer.fromCharArray(deviceInfo.structs.at(i).description.toCharArray(), deviceInfo.structs.at(i).description.count());
		_writeBuffer.append(char(deviceInfo.structs.at(i).variableCount));			// Variable count
		
		// ---------------------------- Variables --------------------------------------
		for (int j = 0; j < deviceInfo.structs.at(i).variables.count(); j++)
		{
			// ---------------------- Variable Data -------------------------
			// --------------------------------------------------------------
			// |    Description    | Size |        Index       |  Contents  |
			// --------------------------------------------------------------
			// |  Variable Index   |  1   |          p         |  0 -> 255  |
			// |   Var desc. len   |  1   |        p + 1       |  0 -> 255  |
			// |     Var desc.     |  e   | (p + 2) -> (E - 1) |     -      |
			// |  Variable Type    |  1   |          E         |  0 -> 255  |
			// --------------------------------------------------------------
			// p = D + 2 + j * variable size (variable) | where j is the variable indexer
			// e = Var desc. len
			// E = p + 2 + e

			_writeBuffer.append(deviceInfo.structs.at(i).variables.at(j).index);						// Variable index
			_writeBuffer.append(char(deviceInfo.structs.at(i).variables.at(j).description.count()));	// Variable desc. length
			// deviceInfo.structs.at(i).variables.at(j).description.toByteArray(_writeBuffer);				// Variable desc.
			_writeBuffer.fromCharArray(deviceInfo.structs.at(i).variables.at(j).description.toCharArray(), deviceInfo.structs.at(i).variables.at(j).description.count());
			_writeBuffer.append(char(deviceInfo.structs.at(i).variables.at(j).dataType));				// Variable type
		}
	}

	// -------------------------- Checksum -----------------------------------
	//char checksum = 0;

	//for (int i = 0; i < _writeBuffer.count(); i++)
	//{
	//	checksum += _writeBuffer.at(i);
	//}

	//_writeBuffer.append(checksum);

	LynxLib::addChecksum(_writeBuffer);

	this->write();

	return _writeBuffer.count();
}

void LynxIoDevice::scan()
{
	// ------------------------ Frame ------------------------------
	// -------------------------------------------------------------
	// |    Description   |    Size    |     Index    |  Contents  |
	// -------------------------------------------------------------
	// |   Static header  |     1      |       0      |    'A'     |
	// |     Struct Id    |     1      |       1      |    255     |
	// | Internal data id |     1      |       2      |     2      |
	// |     Checksum     |     1      |       3      |  0 -> 255  |
	// -------------------------------------------------------------

	_writeBuffer.reserve(4);
	_writeBuffer.append(LYNX_STATIC_HEADER);
	_writeBuffer.append(LYNX_INTERNALS_HEADER);
	_writeBuffer.append(LynxLib::E_LynxInternals::eScan);
	LynxLib::addChecksum(_writeBuffer);

	this->write();

	//char checksum = 0;
	//for (int i = 0; i < _writeBuffer.count(); i++)
	//{
	//	checksum += _writeBuffer.at(i);
	//}
	//_writeBuffer.append(checksum);
}
	
void LynxIoDevice::periodicTransmitStart(const LynxId & lynxId, uint32_t interval)
{
	for (int i = 0; i < _periodicTransmits.count(); i++)
	{
		if (LynxId(_periodicTransmits.at(i)) == lynxId)
		{
			_periodicTransmits[i].timeInterval = interval;
			return;
		}
	}

	_periodicTransmits.append(LynxPeriodicTransmit(lynxId, interval, this->getMillis()));
	return;
}

void LynxIoDevice::periodicTransmitStop(const LynxId & lynxId)
{
	for (int i = 0; i < _periodicTransmits.count(); i++)
	{
		if (LynxId(_periodicTransmits.at(i)) == lynxId)
		{
			_periodicTransmits.remove(i);
			return;
		}
	}
}

LynxDeviceInfo LynxIoDevice::lynxDeviceInfo()
{
	LynxDeviceInfo temp = _deviceInfo;

	_deviceInfo.deleteInfo();
	
	return temp;
}

void LynxIoDevice::readDeviceInfo()
{
	// ------------------------ Device Data -------------------------
	// --------------------------------------------------------------
	// |    Description    | Size |        Index       |  Contents  |
	// --------------------------------------------------------------
	// |   Device Id       |  1   |          5         |  0 -> 255  |
	// | Device desc. len  |  1   |          6         |  0 -> 255  |
	// |   Device desc.    |  b   |    7 -> (B - 1)    |     -      |
	// |   Version len     |  1   |          B         |  0 -> 255  |
	// |     Version       |  c   | (B + 1) -> (C - 1) |     -      |
	// |   Struct count    |  1   |          C         |  0 -> 255  |
	// |   Struct data     |  -   | (C + 1) -> (n - 2) |     -      |
	// --------------------------------------------------------------
	// b = Device desc. len
	// B = 7 + b
	// c = Version len
	// C = c + d

	int readIndex = LYNX_HEADER_BYTES;
	_deviceInfo.deviceId = _readBuffer.at(readIndex);
	readIndex++;
	int readLength = _readBuffer.at(readIndex);
	readIndex++;
	_deviceInfo.description.clear();
	_deviceInfo.description.append(&_readBuffer.at(readIndex), readLength);
	readIndex += readLength;
	readLength = _readBuffer.at(readIndex);
	readIndex++;
	_deviceInfo.lynxVersion.clear();
	_deviceInfo.lynxVersion.append(&_readBuffer.at(readIndex), readLength);
	readIndex += readLength;
	_deviceInfo.structCount = _readBuffer.at(readIndex);
	readIndex++;

	_deviceInfo.structs.reserve(_deviceInfo.structCount);

	for (int i = 0; i < _deviceInfo.structCount; i++)
	{
		// ------------------------ Struct Data -------------------------
		// --------------------------------------------------------------
		// |    Description    | Size |        Index       |  Contents  |
		// --------------------------------------------------------------
		// |    Struct Id      |  1   |          k         |  0 -> 255  |
		// | Struct desc. len  |  1   |        k + 1       |  0 -> 255  |
		// |   Struct desc.    |  d   | (k + 2) -> (D - 1) |     -      |
		// |  Variable count   |  1   |       (D + 1)      |  0 -> 255  |
		// |  Variable data    |  -   |    (D + 2) -> E    |     -      |
		// --------------------------------------------------------------
		// k = C + 1 + i * struct size (variable) | where i is the struct indexer
		// d = Struct desc. len
		// D = k + 2 + d

		_deviceInfo.structs.append();

		_deviceInfo.structs[i].structId = _readBuffer.at(readIndex);
		readIndex++;
		readLength = _readBuffer.at(readIndex);
		readIndex++;
		_deviceInfo.structs[i].description.clear();
		_deviceInfo.structs[i].description.append(&_readBuffer.at(readIndex), readLength);
		readIndex += readLength;
		_deviceInfo.structs[i].variableCount = _readBuffer.at(readIndex);
		readIndex++;

		_deviceInfo.structs[i].variables.reserve(_deviceInfo.structs.at(i).variableCount);

		for (int j = 0; j < _deviceInfo.structs.at(i).variableCount; j++)
		{
			// ---------------------- Variable Data -------------------------
			// --------------------------------------------------------------
			// |    Description    | Size |        Index       |  Contents  |
			// --------------------------------------------------------------
			// |  Variable Index   |  1   |          p         |  0 -> 255  |
			// |   Var desc. len   |  1   |        p + 1       |  0 -> 255  |
			// |     Var desc.     |  e   | (p + 2) -> (E - 1) |     -      |
			// |  Variable Type    |  1   |          E         |  0 -> 255  |
			// --------------------------------------------------------------
			// p = D + 2 + j * variable size (variable) | where j is the variable indexer
			// e = Var desc. len
			// E = p + 2 + e

			_deviceInfo.structs[i].variables.append();

			_deviceInfo.structs[i].variables[j].index = _readBuffer.at(readIndex);
			readIndex++;
			readLength = _readBuffer.at(readIndex);
			readIndex++;
			_deviceInfo.structs[i].variables[j].description.clear();
			_deviceInfo.structs[i].variables[j].description.append(&_readBuffer.at(readIndex), readLength);
			readIndex += readLength;
			_deviceInfo.structs[i].variables[j].dataType = LynxLib::E_LynxDataType(_readBuffer.at(readIndex));
			readIndex++;
		}
	}
}

