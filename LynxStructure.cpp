// #include "D:\Visual Studio Projects\Lynx Test V2\Lynx Test V2\pch.h"
#include "LynxStructure.h"

//-----------------------------------------------------------------------------------------------------------
//-------------------------------------------- Structures ---------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

LynxVersion::LynxVersion()
{
    for (int i = 0; i < 4; i++)
    {
        _version[i] = 0;
    }
}

LynxVersion::LynxVersion(char release, char major, char minor, char build)
{
	_version[0] = release;
	_version[1] = major;
	_version[2] = minor;
	_version[3] = build;
}


LynxVersion::LynxVersion(const LynxString & version) : LynxVersion()
{
	LynxList<int> separators = version.indexesOf('.');

	if (separators.count() != 3)
		return;

	// 1.1.1.1
	int startIndex = 0;
	int endIndex;

	char tmpChar;

	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
			endIndex = (version.count() - 1);
		else
			endIndex = (separators.at(i) - 1);

		if ((endIndex - startIndex) > 3)
			return;

		for (int j = 0; j <= (endIndex - startIndex); j++)
		{
			tmpChar = version.at(endIndex - j);

			if ((tmpChar < '0') || (tmpChar > '9'))
				return;

			switch (j)
			{
			case 0:
				_version[i] += (tmpChar - '0');
				break;
			case 1:
				_version[i] += (tmpChar - '0') * 10;
				break;
			case 2:
				_version[i] += (tmpChar - '0') * 100;
				break;
			default:
				break;
			}			
		}

		startIndex = endIndex + 2;
	}
}

LynxVersion::LynxVersion(uint32_t version)
{
	for (int i = 0; i < 4; i++)
	{
		_version[i] = static_cast<char>(version >> ((3 - i) * 8));
	}
}

LynxVersion::operator LynxString() const
{
	// 255.255.255.255 max 15 chars
	LynxString temp(15);

	for (int i = 0; i < 4; i++)
	{
		temp.append(LynxString::number((int(_version[i]) & 0xff)));
		if (i != 3)
			temp.append('.');
	}

	return temp;
}

LynxVersion::operator uint32_t() const
{
	uint32_t temp = 0;

	for (int i = 0; i < 4; i++)
	{
		temp |= ((static_cast<uint32_t>(_version[i]) & 0xff) << ((3 - i) * 8));
	}

	return temp;
}

//-----------------------------------------------------------------------------------------------------------
//-------------------------------------------- LynxType -----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

LynxLib::E_Endianness LynxType::_endianness = LynxLib::eNotSet;

LynxType::LynxType()
{
	if (_endianness == LynxLib::eNotSet)
	{
		LynxUnion endiannessCheck;

		endiannessCheck._var_i64 = 1;
		if (endiannessCheck.bytes[0] == 1)
			_endianness = LynxLib::eLittleEndian;
		else
			_endianness = LynxLib::eBigEndian;
	}

	_dataType = LynxLib::eNotInitialized;
	_var = LYNX_NULL;
	_str = LYNX_NULL;
	_description = LYNX_NULL;
}

LynxType::LynxType(LynxLib::E_LynxDataType dataType, const LynxString & description) : LynxType()
{
	this->init(dataType, &description);
}

LynxType::LynxType(const LynxType & other) : LynxType(other._dataType, *other._description) 
{ 
	*this = other; 
}

LynxType::~LynxType()
{
	if (_var != LYNX_NULL)
	{
		delete _var;
		_var = LYNX_NULL;
	}

	if (_str != LYNX_NULL)
	{
		delete _str;
		_str = LYNX_NULL;
	}

	if (_description != LYNX_NULL)
	{
		delete _description;
		_description = LYNX_NULL;
	}
}

void LynxType::init(LynxLib::E_LynxDataType dataType, const LynxString * const description)
{
	_dataType = dataType;
	LynxLib::E_LynxDataType tmpType = LynxLib::E_LynxDataType(dataType & 0x7f);

	if (tmpType > LynxLib::eNotInitialized)
	{
		if (tmpType == LynxLib::eString_RW)
		{
			if (_str != LYNX_NULL)
				delete _str;
			_str = new LynxString("");
		}
		else if (tmpType < LynxLib::eLynxType_RW_EndOfList)
		{
			if (_var != LYNX_NULL)
				delete _var;
			_var = new LynxUnion();
		}
	}

	if (_description != LYNX_NULL)
		delete _description;

	if (description == LYNX_NULL)
		return;
	if (description->isEmpty())
		return;

	_description = new LynxString(*description);
}

LynxString LynxType::description() const
{
	if (_description == LYNX_NULL)
		return "Not defined";
	else
		return *_description;
}

int LynxType::toArray(LynxByteArray & buffer, LynxLib::E_LynxState & state) const
{
    int transferSize = this->transferSize();

    LynxByteArray tempBuffer(transferSize);

	if ((_dataType == LynxLib::eString_RW) || (_dataType == LynxLib::eString_RO))
	{
		tempBuffer.append(char(transferSize - 1));				// Add the size
		tempBuffer.append(_str->toCharArray(), _str->count());	// Add the string
	}
	else
	{
		int localSize = this->localSize();
		if (localSize < 1) // Datatype not recognized
		{
			state = LynxLib::eDataTypeNotFound;
			return 0;
		}

		switch (LynxType::endianness())
		{
		case LynxLib::eBigEndian:
			for (int i = 0; i < localSize; i++)
			{
				tempBuffer.append(_var->bytes[int(SIZE_64) - i - 1]);
			}
			break;
		case LynxLib::eLittleEndian:
			for (int i = 0; i < localSize; i++)
			{
				tempBuffer.append(_var->bytes[i]);
			}
			break;
		default:
			state = LynxLib::eEndiannessNotSet;
			return 0;
		}

		LynxLib::splitArray(tempBuffer, transferSize);

		if (tempBuffer.count() != transferSize)
		{
			state = LynxLib::eSplitArrayFailed;
			return 0;
		}
	}

    buffer.append(tempBuffer);

    return transferSize;
}

int LynxType::fromArray(const LynxByteArray & buffer, int startIndex, LynxLib::E_LynxState & state)
{        	
	int transferSize;

	if ((_dataType == LynxLib::eString_RW) || (_dataType == LynxLib::eString_RO))
	{
		transferSize = int(buffer.at(startIndex)); // If it's a string the first byte should specify the size
		transferSize++; // Increment transfersize to include the size specifier

		if ((_dataType & 0x80) != 0) // Read only
			return transferSize;

		_str->clear();
		_str->append(&buffer.at(startIndex + 1), (transferSize - 1));
	}
	else
	{
		transferSize = this->transferSize();
		if ((_dataType & 0x80) != 0) // Read only
			return transferSize;

		if (transferSize < 1) // Datatype not recognized
		{
			state = LynxLib::eDataTypeNotFound;
			return 0;
		}

		int localSize = this->localSize();

		LynxByteArray tempBuffer(transferSize);
		buffer.subList(tempBuffer, startIndex, (startIndex + transferSize - 1));

		int count = LynxLib::mergeArray(tempBuffer, localSize);

		if (count != localSize)
		{
			state = LynxLib::eMergeArrayFailed;
			return transferSize;
		}

		switch (LynxType::endianness())
		{
		case LynxLib::eBigEndian:
			for (int i = 0; i < localSize; i++)
			{
				_var->bytes[int(SIZE_64) - i - 1] = tempBuffer.at(i);
			}
			break;
		case LynxLib::eLittleEndian:
			for (int i = 0; i < localSize; i++)
			{
				_var->bytes[i] = tempBuffer.at(i);
			}
			break;
		default:
			state = LynxLib::eEndiannessNotSet;
			break;
		}
	}

    return transferSize;
}

int LynxType::localSize() const 
{
	if ((_dataType == LynxLib::eString_RW) || (_dataType == LynxLib::eString_RO))
		return _str->count();
	else
		return LynxLib::localSize(_dataType);
}

int LynxType::transferSize() const 
{
	if ((_dataType == LynxLib::eString_RW) || (_dataType == LynxLib::eString_RO))
		if (_str->count() > 255) // maximum string size
			return 256;
		else
			return (_str->count() + 1); // Add one for the size specifier
	else
		return LynxLib::transferSize(_dataType);
}

//-----------------------------------------------------------------------------------------------------------
//----------------------------- Static Functions And Variables  ---------------------------------------------
//-----------------------------------------------------------------------------------------------------------

namespace LynxLib
{
	int localSize(LynxLib::E_LynxDataType dataType)
	{
		// Remove the access specifier (bit 7), since we only care about the size
		LynxLib::E_LynxDataType tmp = LynxLib::E_LynxDataType(dataType & 0x7f);

		switch (tmp)
		{
		case LynxLib::eInt8_RW:
			return sizeof(int8_t);
		case LynxLib::eUint8_RW:
			return sizeof(uint8_t);
		case LynxLib::eInt16_RW:
			return sizeof(int16_t);
		case LynxLib::eUint16_RW:
			return sizeof(uint16_t);
		case LynxLib::eInt32_RW:
			return sizeof(int32_t);
		case LynxLib::eUint32_RW:
			return sizeof(uint32_t);
		case LynxLib::eInt64_RW:
			return sizeof(int64_t);
		case LynxLib::eUint64_RW:
			return sizeof(uint64_t);
		case LynxLib::eFloat_RW:
			return sizeof(float);
		case LynxLib::eDouble_RW:
			return sizeof(double);
		case LynxLib::eBoolean_RW:
			return sizeof(bool);
		default:
			break;
		}

		return 0;
	}

	int transferSize(LynxLib::E_LynxDataType dataType)
	{
		// Remove the access specifier (bit 7), since we only care about the size
		LynxLib::E_LynxDataType tmp = LynxLib::E_LynxDataType(dataType & 0x7f);

		switch (tmp)
		{
		case LynxLib::eInt8_RW:
			return 1;
		case LynxLib::eUint8_RW:
			return 1;
		case LynxLib::eInt16_RW:
			return 2;
		case LynxLib::eUint16_RW:
			return 2;
		case LynxLib::eInt32_RW:
			return 4;
		case LynxLib::eUint32_RW:
			return 4;
		case LynxLib::eInt64_RW:
			return 8;
		case LynxLib::eUint64_RW:
			return 8;
		case LynxLib::eFloat_RW:
			return 4;
		case LynxLib::eDouble_RW:
			return 8;
		case LynxLib::eBoolean_RW:
			return 1;
		default:
			break;
		}

		return 0;
	}

	bool checkChecksum(const LynxByteArray & buffer)
	{
		char checksum = 0;
		for (int i = 0; i < (buffer.count() - 1); i++)
		{
			checksum += buffer.at(i);
		}

		return ((checksum & 0xff) == (buffer.last() & 0xff));
	}

	void addChecksum(LynxByteArray & buffer)
	{
		char checksum = 0;
		for (int i = 0; i < buffer.count(); i++)
		{
			checksum += buffer.at(i);
		}

		buffer.append(checksum);
	}

	void expandInt(int32_t input, LynxByteArray & buffer)
	{
		for (int i = 0; i < 4; i++)
		{
			buffer.append(char((input >> (8 * i)) & 0xff));
		}
	}

	int32_t combineInt(const LynxByteArray & buffer, int startIndex)
	{
		if ((buffer.count() - startIndex) < 4) 
			return 0;

		int32_t value = 0;
		for (int i = 0; i < 4; i++)
		{
			// char tmpChar = buffer.at(startIndex + i);
			// int32_t tmpI32 = int32_t(tmpChar);
			// tmpI32 = tmpI32 << (8 * i);
			// tmpI32 = tmpI32 & (int32_t(0xff) << (8 * i));

			value |= ((int32_t(buffer.at(startIndex + i)) << (8 * i)) & (int32_t(0xff) << (8 * i)));
		}

		return value;
	}



	int splitArray(LynxByteArray & buffer, int desiredSize)
	{
		if (buffer.count() == desiredSize)
			return buffer.count();
		else if (buffer.count() > desiredSize)
			return -1;

		LynxByteArray temp(desiredSize);

		int shiftSize;
		char mask;

		while (temp.count() < desiredSize)
		{
			temp.clear();

			shiftSize = (desiredSize / buffer.count() / 2);
			mask = sizeMask(shiftSize);

			for (int i = 0; i < buffer.count(); i++)
			{
				temp.append(buffer.at(i) & mask);
				temp.append((buffer.at(i) >> (shiftSize * 8)) & mask);
			}

			buffer = temp;
		}

		return buffer.count();
	}

	int mergeArray(LynxByteArray & buffer, int desiredSize)
	{
		if (buffer.count() == desiredSize)
			return buffer.count();
		else if (buffer.count() < desiredSize)
			return -1;

		LynxByteArray temp(buffer.count() / 2);

		int maxSize = buffer.count();
		int shiftSize;
		char mask;
		char tempVar;

		while (buffer.count() > desiredSize)
		{
			temp.clear();

			shiftSize = maxSize / buffer.count();
			mask = sizeMask(shiftSize);

			for (int i = 0; i < buffer.count(); i += 2)
			{
				tempVar = 0;
				tempVar |= (buffer.at(i) & mask);
				tempVar |= ((buffer.at(i + 1) & mask) << (shiftSize * 8));

				temp.append(tempVar);
			}

			buffer = temp;
		}

		return buffer.count();
	}

	char sizeMask(int shiftSize)
	{
		char mask = 0;

		for (int i = 0; i < shiftSize; i++)
		{
			mask |= (char(0xff) << i * 8);
		}

		return mask;
	}

    E_LynxAccessMode accessMode(E_LynxDataType dataType)
    {
        return ((dataType & 0x80) != 0) ? eReadOnly : eReadWrite;
    }
}

//-----------------------------------------------------------------------------------------------------------
//------------------------------------------- Text lists ----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------


const LynxString LynxTextList::_lynxStates[LynxLib::eLynxState_EndOfList] =
{
	"No change",
	"Update separator",
	"New data received",
	"New device info received",
	"Scan received",
	"Data copied to buffer",
	"Pull request received",
	"Periodic transmit started",
	"Periodic transmit stopped",
    "Device id updated",
	"Error separator",
	"Out of sync",
	"Struct id not found",
	"Variable index out of bounds",
	"Buffer too small",
	"Wrong checksum",
	"Wrong static header",
	"Wrong data length",
	"Data length not found",
	"No structures in list",
	"Struct index out of bounds",
	"Split array failed",
	"Merge array failed",
	"Endianness not set",
	"Unknown error",
	"Invalid struct id",
	"Invalid internal id",
    "Data type not recognized",
    "Invalid device id"
};

const LynxString LynxTextList::_lynxDataTypes[LynxLib::eLynxType_RW_EndOfList] =
{
	"Not initialized",
	"8 bit signed int",
	"8 bit unsigned int",
	"16 bit signed int",
	"16 bit unsigned int",
	"32 bit signed int",
	"32 bit unsigned int",
	"64 bit signed int",
	"64 bit unsigned int",
	"Float",
	"Double",
	"String",
	"Boolean"
};

LynxString LynxTextList::lynxState(LynxLib::E_LynxState state)
{
	if ((state < 0) || (state > LynxLib::eLynxState_EndOfList))
		return "N/A";

	return _lynxStates[state];
}

LynxString LynxTextList::lynxDataType(LynxLib::E_LynxDataType dataType)
{
	LynxLib::E_LynxDataType temp = LynxLib::E_LynxDataType(dataType & 0x7f);

	if ((temp < 0) || (temp > LynxLib::eLynxType_RW_EndOfList))
		return "N/A";

    return _lynxDataTypes[temp];
}

LynxString LynxTextList::lynxAccessSpecifier(LynxLib::E_LynxDataType dataType)
{
	LynxLib::E_LynxDataType temp = LynxLib::E_LynxDataType(dataType & 0x80);

	if (temp == 0)
		return "Read write";
	
	return "Read only";
}

//-----------------------------------------------------------------------------------------------------------
//---------------------------------------- LynxStructure ----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

LynxStructure::LynxStructure() : LynxList()
{
	_description = LYNX_NULL;
	_structId = -1;
	_enableReadOnly = false;
}

LynxStructure::~LynxStructure()
{
	if (_description != LYNX_NULL)
	{ 
		delete _description;
		_description = LYNX_NULL;
	}
}

void LynxStructure::init(char structId, const LynxString * const description, bool enableReadOnly, int size)
{
	_enableReadOnly = enableReadOnly;

	LynxList::reserve(size);

	_structId = structId;

	if (_description != LYNX_NULL)
		delete _description;
		
        if (description == LYNX_NULL)
        return;
	if (description->isEmpty())
		return;

	_description = new LynxString(*description);
}

void LynxStructure::getInfo(LynxStructInfo & structInfo) const
{
	structInfo.structId = _structId;
	structInfo.variableCount = _count;

	if (_description == LYNX_NULL)
		structInfo.description = "Not defined";
	else
		structInfo.description = *_description;

	structInfo.variables.reserve(_count);
	for (int i = 0; i < _count; i++)
	{
		structInfo.variables.append();
        structInfo.variables[i].index = char(i);
		structInfo.variables[i].dataType = _data[i].dataType();
		structInfo.variables[i].description = _data[i].description();
	}
}

LynxStructInfo LynxStructure::getInfo() const
{
	LynxStructInfo temp;
	this->getInfo(temp);
	return temp;
}

LynxLib::E_LynxState LynxStructure::toArray(LynxByteArray & buffer, int variableIndex) const
{
	if (_count < 1)
		return LynxLib::eNoStructuresInList;
	else if (variableIndex >= _count)
		return LynxLib::eVariableIndexOutOfBounds;

	LynxLib::E_LynxState state = LynxLib::eDataCopiedToBuffer;

	if (variableIndex < 0) // All variables
	{
		for (int i = 0; i < _count; i++)
		{
            _data[i].toArray(buffer, state);
		}
	}
	else // Single variable
	{
        _data[variableIndex].toArray(buffer, state);
	}

    return state;
}

LynxLib::E_LynxState LynxStructure::toArray(char * buffer, int maxSize, int & copiedSize, int variableIndex) const
{
	LynxByteArray temp;
	LynxLib::E_LynxState state = this->toArray(temp, variableIndex);

	if (state != LynxLib::eDataCopiedToBuffer)
	{
		copiedSize = 0;
		return state;
	}

	copiedSize = temp.toCharArray(buffer, maxSize);
	if (copiedSize < 0)
	{
		copiedSize = 0;
		return LynxLib::eBufferTooSmall;
	}

	return state;
}

void LynxStructure::fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo)
{
    int bufferIndex = LYNX_HEADER_BYTES;

    lynxInfo.state = LynxLib::eNewDataReceived;

    if (lynxInfo.lynxId.variableIndex < 0) // All variables
	{
		for (int i = 0; i < _count; i++)
		{
            bufferIndex += _data[i].fromArray(buffer, bufferIndex, lynxInfo.state);
		}
	}
    else if (lynxInfo.lynxId.variableIndex > _count) // Invalid index
	{
        lynxInfo.state = LynxLib::eVariableIndexOutOfBounds;
        return;
	}
	else // Single variable
	{
        bufferIndex += _data[lynxInfo.lynxId.variableIndex].fromArray(buffer, bufferIndex, lynxInfo.state);
	}
}

void LynxStructure::fromArray(const char * buffer, int size, LynxInfo & lynxInfo)
{
    this->fromArray(LynxByteArray(buffer, size), lynxInfo);
}

LynxId LynxStructure::addVariable(int structIndex, LynxLib::E_LynxDataType dataType, const LynxString & description)
{
	if (!_enableReadOnly)
		dataType = LynxLib::E_LynxDataType(dataType & 0x7f); // Remove read only specifier

	if (
		((dataType <= LynxLib::eNotInitialized) || (dataType >= LynxLib::eLynxType_RW_EndOfList)) &&
		((dataType <= LynxLib::eLynxType_RO_StartOfList) || (dataType >= LynxLib::eLynxType_RO_EndOfList))
		)
		return LynxId();

	this->append();
	this->last().init(dataType, &description);

	//_transferSize += this->last().transferSize();
	//_localSize += this->last().localSize();

	return LynxId(structIndex, (_count - 1));
}

int LynxStructure::transferSize(int index) const
{
	if (index < 0) // All variables
	{
		int tempSize = 0;

		for (int i = 0; i < _count; i++)
		{
			tempSize += _data[i].transferSize();
		}

        return tempSize;
	}
	else if (index >= _count) // Out of bounds
	{
		return 0;
	}
	else // Single variable
	{
		return _data[index].transferSize();
	}
}

int LynxStructure::localSize(int variableIndex) const
{
	if (variableIndex < 0) // All variables
	{
		int tempSize = 0;

		for (int i = 0; i < _count; i++)
		{
			tempSize += _data[i].localSize();
		}

		return tempSize;
	}
	else if (variableIndex >= _count) // Out of bounds
	{
		return 0;
	}
	else // Single variable
	{
		return _data[variableIndex].localSize();
	}
}

LynxString LynxStructure::description() const
{
	if(_description == LYNX_NULL)
		return LynxString();

	return *_description;
}

//-----------------------------------------------------------------------------------------------------------
//----------------------------------------- LynxManager -----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

LynxManager::LynxManager(char deviceId, const LynxString & description, int size) : LynxList(size), _version(LYNX_VERSION)
{ 
	_deviceId = deviceId;
		
	if (description.isEmpty())
		return;

	_description = new LynxString(description);
}

LynxManager::~LynxManager()
{
	if (_description != LYNX_NULL)
	{
		delete _description;
		_description = LYNX_NULL;
	}
}

void LynxManager::getInfo(LynxDeviceInfo & deviceInfo) const
{
	deviceInfo.deviceId = _deviceId;
	deviceInfo.structCount = _count;
	deviceInfo.lynxVersion = _version;
		
	if (_description == LYNX_NULL)
		deviceInfo.description = "Not defined";
	else
		deviceInfo.description = *_description;

	deviceInfo.structs.reserve(_count);
	for (int i = 0; i < _count; i++)
	{
		deviceInfo.structs.append();
		_data[i].getInfo(deviceInfo.structs[i]);
	}
}

LynxDeviceInfo LynxManager::getInfo() const
{
	LynxDeviceInfo temp;
	this->getInfo(temp);
	return temp;
}

char LynxManager::structId(const LynxId & lynxId) const
{
    if((lynxId.structIndex < 0) || (lynxId.structIndex > _count))
        return LYNX_INVALID_DATAGRAM;

    return _data[lynxId.structIndex].structId();
}

LynxString LynxManager::getStructName(const LynxId & lynxId)
{
    if ((lynxId.structIndex < 0) || (lynxId.structIndex >= _count))
        return LynxString();

    return _data[lynxId.structIndex].description();
}

LynxString LynxManager::getVariableName(const LynxId & lynxId)
{
    if ((lynxId.structIndex < 0) || (lynxId.structIndex >= _count))
        return LynxString();

    if ((lynxId.variableIndex < 0) || (lynxId.variableIndex >= _data[lynxId.structIndex].count()))
        return LynxString();

    return _data[lynxId.structIndex].at(lynxId.variableIndex).description();
}

LynxType & LynxManager::variable(const LynxId & lynxId)
{
	return (*this)[lynxId.structIndex][lynxId.variableIndex];
}

const LynxType & LynxManager::variable(const LynxId & lynxId) const
{
	return this->at(lynxId.structIndex).at(lynxId.variableIndex);
}

void LynxManager::copy(const LynxId & source, const LynxId & target)
{
	if ((source.variableIndex < 0) || (target.variableIndex < 0))
		return;
		
	_data[target.structIndex][target.variableIndex] = _data[source.structIndex].at(source.variableIndex);
}

LynxLib::E_LynxState LynxManager::toArray(LynxByteArray & buffer, const LynxId & lynxId) const
{
	// |  Description   |    Size    |     Index    | Contents |
	// ---------------------------------------------------------
	// | Static header  |     1      |       0      |   'A'    |
	// |   Struct Id    |     1      |       1      | 0 -> 254 |
	// | Variable index |     1      |       2      | 0 -> 255 |
	// |  Data length   |     1      |       3      | 0 -> 255 |
	// |   Device Id    |     1      |       4      | 0 -> 255 |
	// |     Data       | dataLength | 5 -> (n - 2) |    -     |
	// |   Checksum     |     1      |    (n - 1)   | 0 -> 255 |

	// n = 5 + dataLength + 1

	if ((lynxId.structIndex < 0) || (lynxId.structIndex >= _count))
		return LynxLib::eStructIndexOutOfBounds;

	int dataLength = _data[lynxId.structIndex].transferSize(lynxId.variableIndex);

	if (dataLength < 1)
		return LynxLib::eDataLengthNotFound;

	buffer.reserve(dataLength + LYNX_HEADER_BYTES + LYNX_CHECKSUM_BYTES);
	buffer.append(LYNX_STATIC_HEADER);
	buffer.append(_data[lynxId.structIndex].structId());
	buffer.append(static_cast<char>(lynxId.variableIndex + 1));
	buffer.append(static_cast<char>(dataLength));
	buffer.append(_deviceId);

	LynxLib::E_LynxState state = _data[lynxId.structIndex].toArray(buffer, lynxId.variableIndex);

	if (state != LynxLib::eDataCopiedToBuffer)
		return state;

	LynxLib::addChecksum(buffer);

	return state;
}

LynxLib::E_LynxState LynxManager::toArray(char * buffer, int maxSize, int & copiedSize, const LynxId & lynxId) const
{
	LynxByteArray temp;
	LynxLib::E_LynxState state = this->toArray(temp, lynxId);

	if (state != LynxLib::eDataCopiedToBuffer)
	{
		copiedSize = 0;
		return state;
	}

	copiedSize = temp.toCharArray(buffer, maxSize);
	if (copiedSize < 0)
	{
		copiedSize = 0;
        return LynxLib::eBufferTooSmall;
	}

	return state;
}

void LynxManager::fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo)
{
	lynxInfo.lynxId.structIndex = this->findId(buffer.at(1));
	lynxInfo.lynxId.variableIndex = (int(buffer.at(2)) & 0xff) - 1;
	lynxInfo.dataLength = int(buffer.at(3)) & 0xff;
	lynxInfo.deviceId = buffer.at(4);

	// Check the struct ID
	if (lynxInfo.lynxId.structIndex < 0)
	{
		lynxInfo.state = LynxLib::eStructIdNotFound;
		return;
	}

	// Check the variable index
	if (lynxInfo.lynxId.variableIndex > _data[lynxInfo.lynxId.structIndex].count())
	{
		lynxInfo.state = LynxLib::eVariableIndexOutOfBounds;
		return;
	}

	// Check the total size
	int totalSize = lynxInfo.dataLength + LYNX_HEADER_BYTES + LYNX_CHECKSUM_BYTES;
	if (buffer.count() < totalSize)
	{
		lynxInfo.state = LynxLib::eBufferTooSmall;
		return;
	}
		
	// Check the checksum
	if (!LynxLib::checkChecksum(buffer))
	{
		lynxInfo.state = LynxLib::eWrongChecksum;
		return;
	}

	// Copy the data
    _data[lynxInfo.lynxId.structIndex].fromArray(buffer, lynxInfo);
}

void LynxManager::fromArray(const char * buffer, int size, LynxInfo & lynxInfo)
{
	LynxByteArray temp(buffer, size);

	this->fromArray(temp, lynxInfo);
}

int LynxManager::transferSize(const LynxId & lynxId) const
{
	return _data[lynxId.structIndex].transferSize(lynxId.variableIndex);
}

int LynxManager::localSize(const LynxId & lynxId) const
{
	return _data[lynxId.structIndex].localSize(lynxId.variableIndex);
}

LynxId LynxManager::addStructure(char structId, const LynxString & description, bool enableReadOnly, int size)
{
	for (int i = 0; i < _count; i++)
	{
		if ((_data[i].structId() == structId) || (_data[i].description() == description))
			return LynxId();
	}

	LynxId temp;
	temp.structIndex = this->append();
	this->last().init(structId, &description, enableReadOnly, size);

	return temp;
}

LynxDynamicId LynxManager::addStructure(const LynxStructInfo & structInfo, bool enableReadOnly)
{
	for (int i = 0; i < _count; i++)
	{
		if ((_data[i].structId() == structInfo.structId) || (_data[i].description() == structInfo.description))
			return LynxDynamicId();
	}

	LynxDynamicId tempId;
    tempId.structId = structInfo.structId;
	tempId.variableIds.reserve(structInfo.variables.count());

	tempId.structLynxId = this->addStructure(structInfo.structId, structInfo.description, enableReadOnly, structInfo.variables.count());

	for (int i = 0; i < structInfo.variables.count(); i++)
	{
		tempId.variableIds.append(this->addVariable(tempId.structLynxId, structInfo.variables.at(i).dataType, structInfo.variables.at(i).description));
	}

	return tempId;
}

LynxId LynxManager::addVariable(const LynxId & parentStruct, LynxLib::E_LynxDataType dataType, const LynxString & description)
{
        if ((parentStruct.structIndex < 0) || (parentStruct.structIndex > _count))
        return (LynxId());
		 	
        return _data[parentStruct.structIndex].addVariable(parentStruct.structIndex, dataType, description);
}

LynxLib::E_LynxDataType LynxManager::dataType(const LynxId & lynxId) const
{
	if ((lynxId.structIndex < 0) || (lynxId.structIndex >= _count))
		return LynxLib::eNotInitialized;
	else if ((lynxId.variableIndex < 0) || (lynxId.variableIndex >= _data[lynxId.structIndex].count()))
		return LynxLib::eNotInitialized;
	
	return _data[lynxId.structIndex].at(lynxId.variableIndex).dataType();
}

LynxLib::E_LynxSimplifiedType LynxManager::simplifiedType(const LynxId & lynxId) const
{
    if ((lynxId.structIndex < 0) || (lynxId.structIndex >= _count))
        return LynxLib::eNotInit;
    else if ((lynxId.variableIndex < 0) || (lynxId.variableIndex >= _data[lynxId.structIndex].count()))
        return LynxLib::eNotInit;

    LynxLib::E_LynxDataType tempType = LynxLib::E_LynxDataType(_data[lynxId.structIndex].at(lynxId.variableIndex).dataType() & 0x7f);

    if (tempType == LynxLib::eString_RW)
        return LynxLib::eString;
    else if (tempType == LynxLib::eBoolean_RW)
        return LynxLib::eBool;
    else if ((tempType > LynxLib::eNotInitialized) && (tempType < LynxLib::eLynxType_RW_EndOfList))
        return LynxLib::eNumber;

    return LynxLib::eNotInit;
}

bool LynxManager::outOfBounds(const LynxId & lynxId) const
{
	if ((lynxId.structIndex < 0) || (lynxId.structIndex >= _count))
		return true;
	
	if ((lynxId.variableIndex < 0) || (lynxId.variableIndex >= _data[lynxId.structIndex].count()))
		return true;

	return false;
}

void LynxManager::setValue(double value, const LynxId & lynxId)
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return;

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	switch (dataType)
	{
	case LynxLib::eInt8_RW:
		this->variable(lynxId).var_i8() = int8_t(value);
		break;
	case LynxLib::eUint8_RW:
		this->variable(lynxId).var_u8() = uint8_t(value);
		break;
	case LynxLib::eInt16_RW:
		this->variable(lynxId).var_i16() = int16_t(value);
		break;
	case LynxLib::eUint16_RW:
		this->variable(lynxId).var_u16() = uint16_t(value);
		break;
	case LynxLib::eInt32_RW:
		this->variable(lynxId).var_i32() = int32_t(value);
		break;
	case LynxLib::eUint32_RW:
		this->variable(lynxId).var_u32() = uint32_t(value);
		break;
	case LynxLib::eInt64_RW:
		this->variable(lynxId).var_i64() = int64_t(value);
		break;
	case LynxLib::eUint64_RW:
		this->variable(lynxId).var_u64() = uint64_t(value);
		break;
	case LynxLib::eFloat_RW:
		this->variable(lynxId).var_float() = float(value);
		break;
	case LynxLib::eDouble_RW:
		this->variable(lynxId).var_double() = value;
		break;
	case LynxLib::eBoolean_RW:
		this->variable(lynxId).var_bool() = (value != 0.0);
		break;
	default:
		break;
	}
}

double LynxManager::getValue(const LynxId & lynxId) const
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return 0.0;

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	switch (dataType)
	{
	case LynxLib::eInt8_RW:
		return double(this->variable(lynxId).var_i8());
	case LynxLib::eUint8_RW:
		return double(this->variable(lynxId).var_u8());
	case LynxLib::eInt16_RW:
		return double(this->variable(lynxId).var_i16());
	case LynxLib::eUint16_RW:
		return double(this->variable(lynxId).var_u16());
	case LynxLib::eInt32_RW:
		return double(this->variable(lynxId).var_i32());
	case LynxLib::eUint32_RW:
		return double(this->variable(lynxId).var_u32());
	case LynxLib::eInt64_RW:
		return double(this->variable(lynxId).var_i64());
	case LynxLib::eUint64_RW:
		return double(this->variable(lynxId).var_u64());
	case LynxLib::eFloat_RW:
		return double(this->variable(lynxId).var_float());
	case LynxLib::eDouble_RW:
		return this->variable(lynxId).var_double();
	case LynxLib::eBoolean_RW:
        return (this->variable(lynxId).var_bool() ? 1.0 : 0.0);
	default:
		break;
	}

	return 0.0;
}

void LynxManager::setString(const LynxString & str, const LynxId & lynxId)
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return;

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	if (dataType == LynxLib::eString_RW) 
	{
		this->variable(lynxId).var_string() = str;
	}
}

LynxString LynxManager::getString(const LynxId & lynxId) const
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return LynxString();

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	if (dataType != LynxLib::eString_RW)
		return LynxString();

	return this->variable(lynxId).var_string();
}

void LynxManager::setBool(bool value, const LynxId & lynxId)
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return;

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	if (dataType != LynxLib::eBoolean_RW)
		return;

	this->variable(lynxId).var_bool() = value;
}

bool LynxManager::getBool(const LynxId & lynxId) const
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return false;

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	if (dataType != LynxLib::eBoolean_RW)
		return false;

	return this->variable(lynxId).var_bool();
}

void LynxManager::setBit(int bit, bool value, const LynxId & lynxId)
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return;

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	if (dataType == LynxLib::eString_RW)
		return;

	int64_t & temp = this->variable(lynxId).var_i64();

	if (value)
	{
		temp |= (int64_t(1) << bit);
	}
	else
	{
		temp &= ~(int64_t(1) << bit);
	}
}

bool LynxManager::getBit(int bit, const LynxId & lynxId) const
{
	// Check for "Out of bounds"
	if (this->outOfBounds(lynxId))
		return false;

	// Remove the access specifier (bit 7)
	LynxLib::E_LynxDataType dataType = LynxLib::E_LynxDataType(this->dataType(lynxId) & 0x7f);

	if (dataType == LynxLib::eString_RW)
		return false;

	return (this->variable(lynxId).var_i64() & (int64_t(1) << bit));
}

int LynxManager::structVariableCount(int structIndex)
{
	if ((structIndex < 0) || (structIndex >= _count))
		return 0;

	return _data[structIndex].count();
}

int LynxManager::findId(char structId)
{
	for (int i = 0; i < _count; i++)
	{
		if (_data[i].structId() == structId)
			return i;
	}

	return -1;
}

bool LynxVar::getBit(int bit) const
{
	return (_lynxManager->variable(_lynxId).var_i64() & (int64_t(1) << bit));
}

void LynxVar::setBit(int bit, bool value)
{
	int64_t & temp = _lynxManager->variable(_lynxId).var_i64();

	if (value)
	{
		temp |= (int64_t(1) << bit);
	}
	else
	{
		temp &= ~(int64_t(1) << bit);
	}
}
