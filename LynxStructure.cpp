// #include "D:\Visual Studio Projects\Lynx Test V2\Lynx Test V2\pch.h"
#include "lynxstructure.h"

namespace LynxLib
{ 
#ifdef LYNX_INCLUDE_EXCEPTIONS
    const char * LynxMessages::outOfBoundsMsg = "Index out of bounds";
#endif // LYNX_INCLUDE_EXCEPTIONS
	
	//-----------------------------------------------------------------------------------------------------------
	//------------------------------------------ LynxString -----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	LynxString::LynxString()
	{
		_string = LYNX_NULL;
		_count = 0;
		_reservedCount = 0;
	}

	LynxString::LynxString(int size) : LynxString()
	{
		this->reserve(size + 1);
	}

	LynxString::LynxString(const char * const other, int maxLength) : LynxString()
	{
		int copyCount = findTermChar(other, maxLength);

		if (copyCount < 0)
			copyCount = maxLength;
		
		// Removed to make it possible to create empty string
		// if (copyCount < 1) // nothing to copy
		// 	return;

		this->reserve(copyCount + 1);

		if (copyCount > 0)
			memcpy(_string, other, copyCount);
		
		_string[copyCount] = '\0';
		_count = copyCount + 1;
	}

	LynxString::LynxString(const LynxString & other) : LynxString()
	{
		*this = other;
	}

	LynxString::~LynxString()
	{
		if (_string != LYNX_NULL)
		{
			delete[] _string;
			_string = LYNX_NULL;
		}
	}

	const char & LynxString::at(int index) const
	{
		return _string[index];
	}

	char & LynxString::first()
	{
		return _string[0];
	}

	const char & LynxString::first() const
	{
		return _string[0];
	}

	char & LynxString::last()
	{
		return _string[_count - 2];
	}

	const char & LynxString::last() const
	{
		return _string[_count - 2];
	}

	int LynxString::count() const
	{
		if (_count < 1)
			return 0;

		return (_count - 1);
	}
	
	void LynxString::clear()
	{
		this->reserve(1);
		_string[0] = '\0';
		_count = 1;
	}

	const LynxString & LynxString::operator=(const LynxString & other)
	{
		if (&other == this)
			return *this;
		
		if (other._count < 2)
		{ 
			this->reserve(1);
			_string[0] = '\0';
			_count = 1;
		}
		else
		{
			this->reserve(other._count);
			memcpy(_string, other._string, other._count);
			_count = other._count;
		}
		
		return *this;
	}

	const LynxString & LynxString::operator=(const char * const other)
	{
		int copySize = findTermChar(other) + 1;

		if (copySize < 1)
		{
			this->reserve(1);
			_string[0] = '\0';
			_count = 1;
		}
		else
		{ 
			this->reserve(copySize);
			memcpy(_string, other, copySize);
			_count = copySize;
		}

		return *this;
	}

	void LynxString::operator+=(const char & other)
	{
		this->append(other);
	}

	void LynxString::operator+=(const LynxString & other)
	{
		this->append(other);
	}

	void LynxString::operator+=(const char * const other)
	{
		this->append(other);
	}

	LynxString LynxString::operator+(const char & other)
	{
		LynxString temp(*this);
		temp.append(other);
		return temp;
	}

	LynxString LynxString::operator+(const LynxString & other)
	{
		LynxString temp(*this);
		temp.append(other);
		return temp;
	}

	LynxString LynxString::operator+(const char * const other)
	{
		LynxString temp(*this);
		temp.append(other);
		return temp;
	}

	LynxString::operator const char*const() const
	{
		if (_count < 2)
			return "";

		return _string;
	}

	void LynxString::reserve(int size)
	{
		_count = 0;

		if (size <= _reservedCount)
			return;

		if (_string != LYNX_NULL)
		{ 
			delete[] _string;
			_string = LYNX_NULL;
		}

		_reservedCount = size;
		_string = new char[_reservedCount];
	}

	void LynxString::resize(int size)
	{
		if ((size + 1) <= _reservedCount)
			return;

		_reservedCount = size + 1;
		if (_count < 2) // Nothing to copy
		{
			if (_string != LYNX_NULL)
				delete[] _string;
			
			_string = new char[_reservedCount];
		}
		else // Data must be copied
		{ 
			char * oldString = _string;

			_string = new char[_reservedCount];

			memcpy(_string, oldString, _count);

			delete[] oldString;
			oldString = LYNX_NULL;
		}
	}

	LynxString LynxString::subString(int startIndex, int endIndex)
	{
		if ((startIndex < 0) || (endIndex >= (_count - 1)) || (startIndex > endIndex))
			return LynxString();

		int copySize = endIndex - startIndex + 1;

		LynxString temp(copySize);
		memcpy(temp._string, &(_string[startIndex]), copySize);
		temp._string[copySize] = '\0';
		temp._count = copySize + 1;

		return temp;
	}

	void LynxString::append(const char & other)
	{
		if (_count == 0)
			_count++;
		this->resize(_count);
	
		_string[_count - 1] = other;
		_string[_count] = '\0';
		_count++;
	}

	void LynxString::append(const LynxString & other)
	{
		if (other._count < 2) // nothing to copy
			return;

		if (_count == 0)
			_count++;

		this->resize(_count + other._count - 2);
		memcpy(&(_string[_count - 1]), other._string, other._count);
		_count = _count + other._count - 1;
	}

	void LynxString::append(const char * const other, int maxLength)
	{
		int copyCount = findTermChar(other, maxLength);
		if (copyCount < 0)
			copyCount = maxLength;

		if (copyCount < 1) // nothing to copy
			return;

		if (_count == 0)
			_count++;

		this->resize(_count + copyCount - 1);
		memcpy(&(_string[_count - 1]), other, copyCount);
		_string[_count + copyCount - 1] = '\0';
		_count = _count + copyCount;
	}

	void LynxString::reverse(int indexFrom, int indexTo)
	{
		if (indexFrom < 0)
			indexFrom = 0;
		if (indexTo < 0)
			indexTo = _count - 2;

		if ((indexFrom >= indexTo) || (indexTo > (_count - 2)))
			return;

		int revCount = (indexTo - indexFrom + 1);
		if (revCount < 2)
			return;
		
		int halfCount = revCount / 2;

		char tempChar;
		for (int i = 0; i < halfCount; i++)
		{
			tempChar = _string[indexFrom + i];
			_string[indexFrom + i] = _string[indexTo - i];
			_string[indexTo - i] = tempChar;
		}

	}

	LynxString LynxString::number(int64_t num, int base)
	{
		LynxString temp(64);
		LynxString::numberInt<int64_t>(num, temp, base);
		return temp;
	}

	LynxString LynxString::number(uint64_t num, int base)
	{
		LynxString temp(64);
		LynxString::numberInt<uint64_t>(num, temp, base);
		return temp;
	}

	LynxString LynxString::number(int32_t num, int base)
	{
		LynxString temp(32);
		LynxString::numberInt<int64_t>(int64_t(num), temp, base);
		return temp;
	}

	LynxString LynxString::number(uint32_t num, int base)
	{
		LynxString temp(32);
		LynxString::numberInt<int64_t>(int64_t(num), temp, base);
		return temp;
	}

	LynxString LynxString::number(double num, int precision)
	{
		LynxString tempStr(99);
		double numAbs = fabs(num);

		if ((numAbs > 1000) || (numAbs < 0.001))
		{
			LynxString::engNumber(num, tempStr, precision);
		}
		else 
		{
			LynxString::decimalNumber(num, tempStr, precision);
		}
		
		return tempStr;
	}

	void LynxString::remove(int indexFrom, int indexTo)
	{
		if (indexTo < 0)
			indexTo = indexFrom;

		if ((indexFrom < 0) || (indexFrom > (_count - 2)) || (indexTo > (_count - 2)) || (indexTo < indexFrom))
			return;
		
		memmove((_string + indexFrom), (_string + indexTo + 1), (_count - indexTo - 1));

		_count -= (indexTo - indexFrom + 1);
	}

	void LynxString::removeFrom(int index)
	{
		if ((index < 0) || (index > (_count - 2)))
			return;

		_string[index] = '\0';
		_count = index + 1;
	}

	void LynxString::removeTo(int index)
	{
		if (index < 0)
			return;

		this->remove(0, index);
	}

	const char * LynxString::toCharArray() const
	{
		if (_count < 1)
			return "";

		return _string;
	}

	int LynxString::findTermChar(const char * str, int maxLength)
	{
		for (int i = 0; i < maxLength; i++)
		{
			if (str[i] == '\0')
			{
				return i;
			}
		}

		return -1;
	}

	void LynxString::decimalNumber(double num, LynxString & strRef, int precision)
	{
		bool sign = (num < 0);
		int64_t intPart = int64_t(fabs(num));
		double fracPart = fabs(num - double(intPart));

		if (sign)
			strRef += '-';

		LynxString::numberInt<int64_t>(intPart, strRef);
		precision -= strRef.count();

		if (sign)
		{ 
			precision++;
			if (strRef.at(1) == '0')
				precision++;
		}
		else if (strRef.at(0) == '0')
				precision++;

		if (precision > 0)
		{
			strRef += '.';

			if (fracPart == 0)
			{
				while (precision > 0)
				{
					strRef += '0';
					precision--;
				}
			}
			else
			{ 
				double targetPrecision = pow(10, (precision - 1));

				while (fracPart < targetPrecision)
				{
					fracPart *= 10;
					if (fracPart < 1)
					{
						strRef += '0';
						// targetPrecision *= 10;
					}
				}

				LynxString::numberInt<int64_t>(int64_t(fracPart), strRef);
			}
		}
	}

	void LynxString::engNumber(double num, LynxString & strRef, int precision)
	{
		int logNum = int(log10(fabs(num)));
		if (logNum < 0)
			logNum--;

		double dispNum = num / pow(10, logNum);

		LynxString::decimalNumber(dispNum, strRef, precision);

		strRef += 'e';
		LynxString::numberInt<int>(logNum, strRef);
	}

	char & LynxString::operator[](int index)
	{
		return _string[index];
	}


	//-----------------------------------------------------------------------------------------------------------
	//-------------------------------------------- LynxType -----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	E_Endianness LynxType::_endianness = eNotSet;

	LynxType::LynxType()
	{
		if (_endianness == eNotSet)
		{
			LynxUnion endiannessCheck;

			endiannessCheck._var_i64 = 1;
			if (endiannessCheck.bytes[0] == 1)
				_endianness = eLittleEndian;
			else
				_endianness = eBigEndian;
		}

		_dataType = eInvalidType;
		_var = LYNX_NULL;
		_str = LYNX_NULL;
	}

	LynxType::LynxType(E_LynxDataType dataType) : LynxType()
	{
		this->init(dataType);
	}

	LynxType::~LynxType()
	{
		if (_var != LYNX_NULL)
			delete _var;

		if (_str != LYNX_NULL)
			delete _str;
	}

	void LynxType::init(E_LynxDataType dataType)
	{
		_dataType = dataType;
		if (_dataType > eInvalidType)
		{
			if (_dataType < eString)
				_var = new LynxUnion();
			else if (_dataType == eString)
				_str = new LynxString("");
		}
	}

    int LynxType::toArray(LynxByteArray & buffer, E_LynxState & state) const
	{
        int transferSize = this->transferSize();

        LynxByteArray tempBuffer(transferSize);

		if ((_dataType > eInvalidType) && (_dataType < eString))
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
				state = eEndiannessNotSet;
				return 0;
			}

			splitArray(tempBuffer, transferSize);

			if (tempBuffer.count() != transferSize)
			{
				state = eSplitArrayFailed;
				return 0;
			}
		}
		else if (_dataType == eString)
		{
			tempBuffer.append(char(transferSize - 1));				// Add the size
			tempBuffer.append(_str->toCharArray(), _str->count());	// Add the string
		}
		else
			return 0;
		

        buffer.append(tempBuffer);

        return transferSize;
	}

    int LynxType::fromArray(const LynxByteArray & buffer, int startIndex, E_LynxState & state)
	{        	
		int transferSize;

		if ((_dataType > eInvalidType) && (_dataType < eString))
		{
			transferSize = this->transferSize();
			int localSize = this->localSize();

			LynxByteArray tempBuffer(transferSize);
			buffer.subList(tempBuffer, startIndex, (startIndex + transferSize - 1));

			int count = mergeArray(tempBuffer, localSize);

			if (count != localSize)
			{
				state = eMergeArrayFailed;
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
				state = eEndiannessNotSet;
				return 0;
			}
		}
		else if (_dataType == eString)
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
		if ((_dataType > eInvalidType) && (_dataType < eString))
			return LynxLib::localSize(_dataType);
		else if (_dataType == eString)
			return _str->count();
		else
			return 0;
	}

	int LynxType::transferSize() const 
	{
		if ((_dataType > eInvalidType) && (_dataType < eString))
			return LynxLib::transferSize(_dataType);
		else if (_dataType == eString)
			if (_str->count() > 255) // maximum string size
				return 256;
			else
				return (_str->count() + 1); // Add one for the size specifier
		else
			return 0;
	}

	//-----------------------------------------------------------------------------------------------------------
	//-------------------------------------- Static Functions ---------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

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

	LynxString operator+(const char * const otherCharArray, const LynxString & otherString)
	{
		LynxString temp(otherCharArray);
		temp.append(otherString);
		return temp;
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

	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------- LynxStructure ----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	LynxStructure::LynxStructure(int size) : LynxList(size)
	{
		// _transferSize = 0;
		// _localSize = 0;
		_structId = -1;
	}

	// LynxByteArray LynxStructure::toArray(int variableIndex) const
	// {
	// 	LynxByteArray temp;
	// 	this->toArray(temp, variableIndex);
	// 
	// 	return temp;
	// }

	E_LynxState LynxStructure::toArray(LynxByteArray & buffer, int variableIndex) const
	{
		if (_count < 1)
			return eNoStructuresInList;
		else if (variableIndex >= _count)
			return eVariableIndexOutOfBounds;

        E_LynxState state = eDataCopiedToBuffer;

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

	E_LynxState LynxStructure::toArray(char * buffer, int maxSize, int & copiedSize, int variableIndex) const
	{
		LynxByteArray temp;
		E_LynxState state = this->toArray(temp, variableIndex);

		if (state != eDataCopiedToBuffer)
		{
			copiedSize = 0;
			return state;
		}

		copiedSize = temp.toCharArray(buffer, maxSize);
		if (copiedSize < 0)
		{
			copiedSize = 0;
			return eBufferTooSmall;
		}

		return state;
	}

    void LynxStructure::fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo)
	{
        int bufferIndex = LYNX_HEADER_BYTES;

        lynxInfo.state = eNewDataReceived;

        if (lynxInfo.lynxId.variableIndex < 0) // All variables
		{
			for (int i = 0; i < _count; i++)
			{
                bufferIndex += _data[i].fromArray(buffer, bufferIndex, lynxInfo.state);
			}
		}
        else if (lynxInfo.lynxId.variableIndex > _count) // Invalid index
		{
            lynxInfo.state = eVariableIndexOutOfBounds;
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

	LynxId LynxStructure::addVariable(int structIndex, E_LynxDataType dataType)
	{
		this->append();
		this->last().init(dataType);

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
	//--------------------------------------- LynxByteArray -----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	LynxByteArray::LynxByteArray(const char * charArray, int size) : LynxList<char>(size)
	{
		for (int i = 0; i < size; i++)
		{
			_data[i] = charArray[i];
		}

		_count = size;
	}

	int LynxByteArray::toCharArray(char * buffer, int maxSize) const
	{
		if (_count > maxSize)
			return -1;

		for (int i = 0; i < _count; i++)
		{
			buffer[i] = _data[i];
		}

		return _count;
	}

	//-----------------------------------------------------------------------------------------------------------
	//----------------------------------------- LynxManager -----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	LynxManager::LynxManager(char deviceId, int size) : LynxList(size) { _deviceId = deviceId; }

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
			(_data[target.structIndex][target.variableIndex].dataType() > eInvalidType) &&
			(_data[target.structIndex][target.variableIndex].dataType() < eString) &&
			(_data[source.structIndex][source.variableIndex].dataType() > eInvalidType) &&
			(_data[source.structIndex][source.variableIndex].dataType() < eString)
		)
			_data[target.structIndex][target.variableIndex].var_i64() = _data[source.structIndex][source.variableIndex].var_i64();
		
		if
		(
			(_data[target.structIndex][target.variableIndex].dataType() == eString) &&
			(_data[source.structIndex][source.variableIndex].dataType() == eString)
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

	E_LynxState LynxManager::toArray(LynxByteArray & buffer, const LynxId & lynxId) const
	{
		if ((lynxId.structIndex < 0) || (lynxId.structIndex >= _count))
			return eStructIndexOutOfBounds;

		int dataLength = _data[lynxId.structIndex].transferSize(lynxId.variableIndex);

		if (dataLength < 1)
			return eDataLengthNotFound;

		buffer.reserve(dataLength + LYNX_HEADER_BYTES + LYNX_CHECKSUM_BYTES);
		buffer.append(LYNX_STATIC_HEADER);
		buffer.append(_data[lynxId.structIndex].structId());
		buffer.append(static_cast<char>(lynxId.variableIndex + 1));
		buffer.append(static_cast<char>(dataLength));
		buffer.append(_deviceId);

		E_LynxState state = _data[lynxId.structIndex].toArray(buffer, lynxId.variableIndex);

		if (state != eDataCopiedToBuffer)
			return state;

		char checksum = 0;
		for (int i = 0; i < buffer.count(); i++)
		{
			checksum += buffer.at(i);
		}

		buffer.append(checksum & char(0xff));

		return state;
	}

	E_LynxState LynxManager::toArray(char * buffer, int maxSize, int & copiedSize, const LynxId & lynxId) const
	{
		LynxByteArray temp;
		E_LynxState state = this->toArray(temp, lynxId);

		if (state != eDataCopiedToBuffer)
		{
			copiedSize = 0;
			return state;
		}

		copiedSize = temp.toCharArray(buffer, maxSize);
		if (copiedSize < 0)
		{
			copiedSize = 0;
			return eBufferTooSmall;
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
			lynxInfo.state = eStructIdNotFound;
			return;
		}

		// Check the variable index
		if (lynxInfo.lynxId.variableIndex > _data[lynxInfo.lynxId.structIndex].count())
		{
			lynxInfo.state = eVariableIndexOutOfBounds;
			return;
		}

		// Check the total size
		int totalSize = lynxInfo.dataLength + LYNX_HEADER_BYTES + LYNX_CHECKSUM_BYTES;
		if (buffer.count() < totalSize)
		{
			lynxInfo.state = eBufferTooSmall;
			return;
		}

		// Calculate the checksum
		int checksumIndex = totalSize - 1;
		char checksum = 0;
		for (int i = 0; i < checksumIndex; i++)
		{
			checksum += buffer.at(i);
		}
		// Check the checksum
		if ((checksum & 0xff) != (buffer.at(checksumIndex) & 0xff))
		{
			lynxInfo.state = eWrongChecksum;
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

	LynxId LynxManager::addStructure(char structId, int size)
	{
		LynxId temp;
		temp.structIndex = this->append();
		this->last().reserve(size);
		this->last().setStructId(structId);

		return temp;
	}

    LynxId LynxManager::addVariable(const LynxId & parentStruct, E_LynxDataType dataType)
	{
         if ((parentStruct.structIndex < 0) || (parentStruct.structIndex > _count))
            return (LynxId(-1, -1));
		 	
         return _data[parentStruct.structIndex].addVariable(parentStruct.structIndex, dataType);
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
	// LynxManager Lynx;
}
