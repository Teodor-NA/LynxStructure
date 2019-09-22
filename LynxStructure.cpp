// #include "D:\Visual Studio Projects\Lynx Test V2\Lynx Test V2\pch.h"
#include "lynxstructure.h"


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

	_dataType = LynxLib::eInvalidType;
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
	if (_dataType > LynxLib::eInvalidType)
	{
		if (_dataType < LynxLib::eString)
		{
			if (_var != LYNX_NULL)
				delete _var;
			_var = new LynxUnion();
		}
		else if (_dataType == LynxLib::eString)
		{
			if (_str != LYNX_NULL)
				delete _str;
			_str = new LynxString("");
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

LynxString LynxType::description()
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

	if ((_dataType > LynxLib::eInvalidType) && (_dataType < LynxLib::eString))
	{
		int localSize = this->localSize();

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
	else if (_dataType == LynxLib::eString)
	{
		tempBuffer.append(char(transferSize - 1));				// Add the size
		tempBuffer.append(_str->toCharArray(), _str->count());	// Add the string
	}
	else
		return 0;
		

    buffer.append(tempBuffer);

    return transferSize;
}

int LynxType::fromArray(const LynxByteArray & buffer, int startIndex, LynxLib::E_LynxState & state)
{        	
	int transferSize;

	if ((_dataType > LynxLib::eInvalidType) && (_dataType < LynxLib::eString))
	{
		transferSize = this->transferSize();
		int localSize = this->localSize();

		LynxByteArray tempBuffer(transferSize);
		buffer.subList(tempBuffer, startIndex, (startIndex + transferSize - 1));

		int count =LynxLib::mergeArray(tempBuffer, localSize);

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
			return 0;
		}
	}
	else if (_dataType == LynxLib::eString)
	{
		transferSize = int(buffer.at(startIndex)); // If it's a string the first byte should specify the size

		_str->clear();
		_str->append(&buffer.at(startIndex + 1), transferSize);
		transferSize++; // Increment transfersize for the return variable to be correct
	}
	else
		return 0;

    return transferSize;
}

int LynxType::localSize() const 
{
	if ((_dataType > LynxLib::eInvalidType) && (_dataType < LynxLib::eString))
		return LynxLib::localSize(_dataType);
	else if (_dataType == LynxLib::eString)
		return _str->count();
	else
		return 0;
}

int LynxType::transferSize() const 
{
	if ((_dataType > LynxLib::eInvalidType) && (_dataType < LynxLib::eString))
		return LynxLib::transferSize(_dataType);
	else if (_dataType == LynxLib::eString)
		if (_str->count() > 255) // maximum string size
			return 256;
		else
			return (_str->count() + 1); // Add one for the size specifier
	else
		return 0;
}

//-----------------------------------------------------------------------------------------------------------
//----------------------------- Static Functions And Variables  ---------------------------------------------
//-----------------------------------------------------------------------------------------------------------

namespace LynxLib
{
	const LynxString lynxStateTextList[LynxLib::eLynxState_EndOfList] =
	{
		"No change",
		"New data received",
		"New device info received",
		"Scan received",
		"Data copied to buffer",
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
		"Unknown error"
	};

	const LynxString lynxTypeTextList[LynxLib::eLynxType_EndOfList]
	{
		"Invalid type",
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
		"String"
	};

	int localSize(LynxLib::E_LynxDataType dataType)
	{
		switch (dataType)
		{
		case LynxLib::eInt8:
			return sizeof(int8_t);
		case LynxLib::eUint8:
			return sizeof(uint8_t);
		case LynxLib::eInt16:
			return sizeof(int16_t);
		case LynxLib::eUint16:
			return sizeof(uint16_t);
		case LynxLib::eInt32:
			return sizeof(int32_t);
		case LynxLib::eUint32:
			return sizeof(uint32_t);
		case LynxLib::eInt64:
			return sizeof(int64_t);
		case LynxLib::eUint64:
			return sizeof(uint64_t);
		case LynxLib::eFloat:
			return sizeof(float);
		case LynxLib::eDouble:
			return sizeof(double);
		default:
			break;
		}

		return 0;
	}

	int transferSize(LynxLib::E_LynxDataType dataType)
	{
		switch (dataType)
		{
		case LynxLib::eInt8:
			return 1;
		case LynxLib::eUint8:
			return 1;
		case LynxLib::eInt16:
			return 2;
		case LynxLib::eUint16:
			return 2;
		case LynxLib::eInt32:
			return 4;
		case LynxLib::eUint32:
			return 4;
		case LynxLib::eInt64:
			return 8;
		case LynxLib::eUint64:
			return 8;
		case LynxLib::eFloat:
			return 4;
		case LynxLib::eDouble:
			return 8;
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
}

//-----------------------------------------------------------------------------------------------------------
//---------------------------------------- LynxStructure ----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

LynxStructure::LynxStructure() : LynxList()
{
	_description = LYNX_NULL;
	_structId = -1;
}

LynxStructure::~LynxStructure()
{
	if (_description != LYNX_NULL)
	{ 
		delete _description;
		_description = LYNX_NULL;
	}
}

void LynxStructure::init(char structId, const LynxString * const description, int size)
{
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

//-----------------------------------------------------------------------------------------------------------
//----------------------------------------- LynxManager -----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

LynxManager::LynxManager(char deviceId, const LynxString & description, int size) : LynxList(size) 
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
	deviceInfo.lynxVersion = LYNX_VERSION;
		
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

char LynxManager::structId(const LynxId & lynxId)
{
    if((lynxId.structIndex < 0) || (lynxId.structIndex > _count))
        return char(0xff);

    return _data[lynxId.structIndex].structId();
}

LynxType & LynxManager::variable(const LynxId & lynxId)
{
	if ((lynxId.variableIndex < 0) || (lynxId.structIndex < 0))
	{
		_dummyVariable.var_i64() = 0;
		return _dummyVariable;
	}

	return (*this)[lynxId.structIndex][lynxId.variableIndex];
}

const LynxType & LynxManager::variable(const LynxId & lynxId) const
{
	if ((lynxId.variableIndex < 0) || (lynxId.structIndex < 0))
	{
		return _dummyVariable;
	}

	return this->at(lynxId.structIndex).at(lynxId.variableIndex);
}

void LynxManager::copy(const LynxId & source, const LynxId & target)
{
	if ((source.variableIndex < 0) || (target.variableIndex < 0))
		return;
		
	// Should probably find a better way to do this...
	if 
	(
		(_data[target.structIndex][target.variableIndex].dataType() > LynxLib::eInvalidType) &&
		(_data[target.structIndex][target.variableIndex].dataType() < LynxLib::eString) &&
		(_data[source.structIndex][source.variableIndex].dataType() > LynxLib::eInvalidType) &&
		(_data[source.structIndex][source.variableIndex].dataType() < LynxLib::eString)
	)
		_data[target.structIndex][target.variableIndex].var_i64() = _data[source.structIndex][source.variableIndex].var_i64();
		
	if
	(
		(_data[target.structIndex][target.variableIndex].dataType() == LynxLib::eString) &&
		(_data[source.structIndex][source.variableIndex].dataType() == LynxLib::eString)
	)
		_data[target.structIndex][target.variableIndex].var_string() = _data[source.structIndex][source.structIndex].var_string();
}

// LynxByteArray LynxManager::toArray(const LynxId & lynxId) const
// {
// 	LynxByteArray temp;
// 
// 	this->toArray(temp, lynxId);
// 
// 	return temp;
// }

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

	//char checksum = 0;
	//for (int i = 0; i < buffer.count(); i++)
	//{
	//	checksum += buffer.at(i);
	//}
	//
	//buffer.append(checksum & char(0xff));

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
	lynxInfo.lynxId.variableIndex = buffer.at(2) - 1;
	lynxInfo.dataLength = buffer.at(3);
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

	// Calculate the checksum
	//int checksumIndex = totalSize - 1;
	//char checksum = 0;
	//for (int i = 0; i < checksumIndex; i++)
	//{
	//	checksum += buffer.at(i);
	//}
		
	// Check the checksum
	if (!LynxLib::checkChecksum(buffer))
	{
		lynxInfo.state = LynxLib::eWrongChecksum;
		return;
	}



	// Make a temporary buffer and extract the data
    // LynxByteArray temp;
    // buffer.subList(temp, LYNX_HEADER_BYTES, LYNX_HEADER_BYTES + lynxInfo.dataLength - 1);

	// Merge the databuffer if the local size is different from the transfersize
    // int count = mergeArray(temp, _data[lynxInfo.lynxId.structIndex].localSize(lynxInfo.lynxId.variableIndex));

	// Check if the merge was successful
    // if (count < 0)
    // {
    // 	lynxInfo.state = eMergeArrayFailed;
    // 	return;
    // }

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

LynxId LynxManager::addStructure(char structId, const LynxString & description, int size)
{
	LynxId temp;
	temp.structIndex = this->append();
	this->last().init(structId, &description, size);
	// this->last().reserve(size);
	// this->last().setStructId(structId);

	return temp;
}

LynxId LynxManager::addVariable(const LynxId & parentStruct, LynxLib::E_LynxDataType dataType, const LynxString & description)
{
        if ((parentStruct.structIndex < 0) || (parentStruct.structIndex > _count))
        return (LynxId(-1, -1));
		 	
        return _data[parentStruct.structIndex].addVariable(parentStruct.structIndex, dataType, description);
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

