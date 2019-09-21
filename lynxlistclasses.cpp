#include "lynxlistclasses.h"

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

int LynxByteArray::fromCharArray(const char * const buffer, int size)
{
	if (size < 1)
		return 0;
	this->resize(_count + size);
	memcpy(&_data[_count], buffer, size);
	_count += size;
	return size;
}


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
	this->reserve(size);
}

LynxString::LynxString(const char * const other, int maxLength) : LynxString()
{
	int copyCount = findTermChar(other, maxLength);

	if (copyCount < 0)
		copyCount = maxLength;

	this->reserve(copyCount);

	if (copyCount > 0)
	{
		memcpy(_string, other, copyCount);

		_string[copyCount] = '\0';
		_count = copyCount + 1;
	}
}

LynxString::LynxString(const LynxString & other) : LynxString()
{
	*this = other;
}

LynxString::~LynxString()
{
	this->deleteData();
}

void LynxString::deleteData()
{
	if (_string != LYNX_NULL)
	{
		delete[] _string;
		_string = LYNX_NULL;
	}

	_count = 0;
	_reservedCount = 0;
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
	return (_count - 1);
}

void LynxString::clear()
{
	this->reserve(0);
}

const LynxString & LynxString::operator=(const LynxString & other)
{
	if (&other == this)
		return *this;

	if (other._count < 2)
	{
		this->clear();
	}
	else
	{
		this->reserve(other.count());
		memcpy(_string, other._string, other._count);
		_count = other._count;
	}

	return *this;
}

const LynxString & LynxString::operator=(const char * const other)
{
	int copySize = findTermChar(other);

	if (copySize < 1)
	{
		this->clear();
	}
	else
	{
		this->reserve(copySize);
		memcpy(_string, other, copySize);
		_string[copySize] = '\0';
		_count = copySize + 1;
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

LynxString operator+(const char * const otherCharArray, const LynxString & otherString)
{
	LynxString temp(otherCharArray);
	temp.append(otherString);
	return temp;
}

LynxString::operator const char*const() const
{
	if (_count < 1)
		return "";

	return _string;
}

bool LynxString::operator==(const LynxString & other)
{
	return this->compare(other);
}

bool LynxString::operator!=(const LynxString & other)
{
	return !this->compare(other);
}

void LynxString::reserve(int size)
{
	if ((size + 1) <= _reservedCount)
	{
		_string[0] = '\0';
		_count = 1;
		return;
	}
		
	this->deleteData();

	_reservedCount = size + 1;
	_string = new char[_reservedCount];
	_string[0] = '\0';
	_count = 1;
}

void LynxString::resize(int size)
{
	if ((size + 1) <= _reservedCount)
		return;

	if (_count < 2) // Nothing to copy
	{		
		this->reserve(size);
	}
	else // Data must be copied
	{
		char * oldString = _string;

		_reservedCount = size + 1;
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

bool LynxString::compare(const LynxString & other) const
{
	if ((_count < 2) && (other._count < 2)) // They are both empty or uninitialized
		return true;

	if (_count != other._count) // If the count is unequal then the strings are unequal
		return false;

	int cmp = strncmp(_string, other._string, _count); // Compare the strings

	// for (int i = 0; i < (_count - 1); i++) // Check each character in the string
	// {
	// 	if (_string[i] != other._string[i])
	// 		return false;
	// }

	return (cmp == 0);
}

void LynxString::append(const char & other)
{
	if (_count < 1)
		this->resize(1);
	else
		this->resize(_count);

	_string[_count - 1] = other;
	_string[_count] = '\0';
	_count++;
}

void LynxString::append(const LynxString & other)
{
	if (other._count < 2) // nothing to copy
		return;

	if (_count < 1)
		this->resize(other.count());
	else
		this->resize(this->count() + other.count());

	memcpy(&(_string[_count - 1]), other._string, other._count);
	_count += other.count();
}

void LynxString::append(const char * const other, int maxLength)
{
	int copyCount = findTermChar(other, maxLength);
	if (copyCount < 0)
		copyCount = maxLength;

	if (copyCount < 1) // nothing to copy
		return;

	if (_count < 1)
		this->resize(copyCount);
	else
		this->resize(this->count() + copyCount);
	
	memcpy(&(_string[_count - 1]), other, copyCount);
	_count += copyCount;
	_string[_count - 1] = '\0';
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

int LynxString::toByteArray(LynxByteArray & byteArray) const
{
	byteArray.resize(byteArray.count() + _count - 1);
	for (int i = 0; i < (_count - 1); i++)
	{
		byteArray.append(_string[i]);
	}

	return (_count - 1);
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
//---------------------------------------- LynxRingBuffer ---------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

LynxRingBuffer::LynxRingBuffer(int size, LynxLib::E_LynxRingBufferMode mode) : _mode(mode)
{
	_count = 0;
	_reservedCount = 0;
	_readIndex = 0;
	_writeIndex = 0;
	_data = LYNX_NULL;

	this->reserve(size);
}

LynxRingBuffer::~LynxRingBuffer()
{
	this->deleteData();
}

void LynxRingBuffer::deleteData()
{
	if (_data != LYNX_NULL)
	{
		delete[] _data;
		_data = LYNX_NULL;
	}

	_count = 0;
	_reservedCount = 0;
	_readIndex = 0;
	_writeIndex = 0;
}

void LynxRingBuffer::reserve(int size)
{
	_count = 0;

	if (size <= _reservedCount)
	{
		_writeIndex = 0;
		_readIndex = 0;
		return;
	}

	this->deleteData();

	_reservedCount = size;
	_data = new char[_reservedCount];
}

void LynxRingBuffer::resize(int size)
{
	if (size <= _reservedCount)
		return;

	if (_count < 1)
	{
		this->reserve(size);
		return;
	}

	// As the buffer may be fragmented, we will make a temporary buffer to transfer the data
	LynxByteArray temp(_count);
	while (_count > 0)
	{
		temp.append(this->read());
	}

	this->reserve(size);

	memcpy(_data, temp.data(), temp.count());

	_readIndex = 0;
	_writeIndex = _count = temp.count();
}

char LynxRingBuffer::read()
{
	if (_count < 1)
		return 0;

	if (_readIndex >= _reservedCount)
		_readIndex = 0;

	char temp = _data[_readIndex];

	_readIndex++;
	_count--;

	return temp;
}

int LynxRingBuffer::read(LynxByteArray & buffer, int size)
{
	int length;
	if (size < 0)
		length = _count;
	else
		length = (size > _count) ? _count : size;

	buffer.resize(buffer.count() + length);
	for (int i = 0; i < length; i++)
	{
		buffer.append(this->read());
	}

	return length;
}

void LynxRingBuffer::write(char data)
{
	if (_count >= _reservedCount)
	{
		switch (_mode)
		{
		case LynxLib::eAutogrow:
			this->resize(_count + 1);
			break;
		case LynxLib::eFixedOverwrite:
			break;
		case LynxLib::eFixedRefuse:
			return;
		default:
			return;
		}
	}

	if (_writeIndex >= _reservedCount)
		_writeIndex = 0;

	_data[_writeIndex] = data;

	_writeIndex++;
	_count++;
}

int LynxRingBuffer::write(const LynxByteArray & buffer, int size, int startIndex)
{
	if (startIndex >= buffer.count())
		return 0;

	int length;
	if (size < 0)
		length = buffer.count();
	else
		length = (size > (buffer.count() - startIndex)) ? (buffer.count() - startIndex) : size;

	switch (_mode)
	{
	case LynxLib::eAutogrow:
		this->resize(_count + length);
		break;
	default:
		break;
	}


	for (int i = 0; i < length; i++)
	{
		this->write(buffer.at(i + startIndex));
	}

	return length;
}
