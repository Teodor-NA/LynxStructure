#ifndef LYNXLISTCLASSES_H
#define LYNXLISTCLASSES_H

#ifdef TI
typedef uint16_t uint8_t
typedef int16_t int8_t
#else
#include <stdint.h>
#endif // TI

#ifndef LYNX_NULL
#ifdef TI
#define LYNX_NULL 0
#else
#define LYNX_NULL nullptr
#endif // TI
#endif // !LYNX_NULL

#include <string.h>
#include <math.h>

//-----------------------------------------------------------------------------------------------------------
//-------------------------------------------- LynxList -----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

template <class T>
class LynxList
{
public:
	LynxList()
	{
		_data = LYNX_NULL;
		_count = 0;
		_reservedCount = 0;
	}

	LynxList(int size) : LynxList()
	{
		this->reserve(size);
	}

	LynxList(const LynxList & other) : LynxList()
	{
		*this = other;
	}

	~LynxList()
	{
		this->deleteData();
	}

	void deleteData()
	{
		if (_data != LYNX_NULL)
		{
			delete[] _data;
			_data = LYNX_NULL;
		}

		_count = 0;
		_reservedCount = 0;
	}

	LynxList & operator = (const LynxList & other)
	{
		if (&other == this)
			return *this;

		_count = 0;

		if (other._count < 1) // If the other list is empty, then copying it is pointless
			return *this;

		this->reserve(other._count);

		// memcpy(_data, other._data, other._count * sizeof(T));

		for (int i = 0; i < other._count; i++)
		{
			_data[i] = other._data[i];
		}

		_count = other._count;
		return *this;
	}

	T & operator [] (int index)
	{
		return _data[index];
	}

	void clear() { _count = 0; }

	int count() const { return _count; }

	void reserve(int size)
	{
		_count = 0;

		if (size <= _reservedCount)
			return;

		this->deleteData();

		_reservedCount = size;
		_data = new T[_reservedCount];
	}

	void resize(int size)
	{
		if (size <= _reservedCount)
			return;

		if (_count < 1)
		{
			this->reserve(size);
			return;
		}

		T * oldData = _data;

		_reservedCount = size;

		_data = new T[_reservedCount];

		for (int i = 0; i < _count; i++)
		{
			_data[i] = oldData[i];
		}

		// memcpy(_data, oldData, _count * sizeof(T));

		delete[] oldData;
		oldData = LYNX_NULL;
	}

	int append()
	{
		this->resize(_count + 1);
		// _data[_count] = T();
		_count++;

		return (_count - 1);
	}

	int append(const T & other)
	{
		this->resize(_count + 1);
		_data[_count] = other;
		_count++;

		return (_count - 1);
	}

	int append(const LynxList<T> & other)
	{
		this->resize(other._count + _count);

		for (int i = 0; i < other._count; i++)
		{
			_data[_count + i] = other._data[i];
		}

		// memcpy(&_data[_count], other._data, (other._count * sizeof(T)));

		_count += other._count;

		return (_count - 1);
	}

	int append(const T * const other, int size)
	{
		this->resize(_count + size);

		for (int i = 0; i < size; i++)
		{
			_data[_count + i] = other[i];
		}

		_count += size;

		return (_count - 1);
	}

	void remove(int indexFrom, int indexTo = -1)
	{
		if (indexTo < 0)
			indexTo = indexFrom;

		if ((indexFrom < 0) || (indexFrom > indexTo) || (indexTo >= _count))
			return;

		int diff = indexTo - indexFrom + 1;
		int copySize = _count - indexTo - 1;

		for (int i = 0; i < copySize; i++)
		{
			_data[indexFrom + i] = _data[indexTo + i + 1];
		}
		// memmove(adrFrom, adrTo, copySize * sizeof(T));

		_count -= diff;
	}

	void subList(LynxList<T> & result, int startIndex, int endIndex) const
	{
		if ((startIndex < 0) || (endIndex >= _count) || (startIndex > endIndex))
			return;

		int diff = endIndex - startIndex + 1;
		result.reserve(diff);
		for (int i = 0; i < diff; i++)
		{
			result._data[i] = _data[startIndex + i];
		}

		// memcpy(result._data, _data + startIndex * sizeof(T), diff * sizeof(T));

		result._count = diff;
	}

	LynxList<T> subList(int startIndex, int endIndex) const
	{
		LynxList<T> temp;
		this->subList(temp, startIndex, endIndex);

		return temp;
	}

	const T & at(int index) const
	{
		return _data[index];
	}

	const T & first() const { return _data[0]; }

	T & first() { return _data[0]; }

	const T & last() const { return _data[_count - 1]; }

	T & last() { return _data[_count - 1]; }

protected:
	T * _data;
	int _count;
	int _reservedCount;
};

//-----------------------------------------------------------------------------------------------------------
//----------------------------------------- LynxByteArray ---------------------------------------------------
//-----------------------------------------------------------------------------------------------------------


class LynxByteArray : public LynxList<char>
{
public:
	LynxByteArray() : LynxList<char>() {}
	LynxByteArray(int size) : LynxList<char>(size) {}
	LynxByteArray(const char * charArray, int size);
	LynxByteArray(const LynxList<char> & other) : LynxList<char>(other) {}

	const char * data() const { return _data; }

	using LynxList::subList;
	LynxByteArray subList(int startIndex, int endIndex)
	{
		LynxByteArray temp;
		LynxList::subList(temp, startIndex, endIndex);
		return temp;
	}

	int toCharArray(char * buffer, int maxSize) const;
	int fromCharArray(const char * const buffer, int size);

};

//-----------------------------------------------------------------------------------------------------------
//------------------------------------------ LynxString -----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

class LynxString
{
public:
	LynxString();
	LynxString(int size);
	LynxString(const char * const other, int maxLength = 255);
	LynxString(const LynxString & other);

	~LynxString();

	void deleteData();

	const char & at(int index) const;
	char & operator [] (int index);

	char & first();
	const char & first() const;
	char & last();
	const char & last() const;

	int count() const;
	void clear();

	const LynxString & operator = (const LynxString & other);
	const LynxString & operator = (const char * const other);

	void operator += (const char & other);
	void operator += (const LynxString & other);
	void operator += (const char * const other);

	LynxString operator + (const char & other);
	LynxString operator + (const LynxString & other);
	LynxString operator + (const char * const other);

	friend LynxString operator +(const char * const otherCharArray, const LynxString & otherString);

	operator const char * const() const;

	bool operator == (const LynxString & other);
	bool operator != (const LynxString & other);

	void resize(int size);

	LynxString subString(int startIndex, int endIndex);

	bool compare(const LynxString & other) const;
	bool isEmpty() const { return (_count < 2); }

	void append(const char & other);
	void append(const LynxString & other);
	void append(const char * const other, int maxLength = 255);

	void reverse(int indexFrom = -1, int indexTo = -1);

	static LynxString number(int64_t num, int base = 10);
	static LynxString number(uint64_t num, int base = 10);
	static LynxString number(int32_t num, int base = 10);
	static LynxString number(uint32_t num, int base = 10);

	static LynxString number(double num, int precision = 5);

	// Remove characters between indexes. Only one character will be removed if indexTo is omitted 
	void remove(int indexFrom, int indexTo = -1);
	// All characters after and including index will be removed
	void removeFrom(int index);
	// All characters before and including index will be removed
	void removeTo(int index);

	// Does not include term char
	int toByteArray(LynxByteArray & byteArray) const;
	const char * toCharArray() const;

private:
	char * _string;
	int _count;
	int _reservedCount;

	void reserve(int size);
	static int findTermChar(const char * str, int maxLength = 255);

	static void decimalNumber(double num, LynxString & strRef, int precision);
	static void engNumber(double num, LynxString & strRef, int precision);

	// Appends num to strRef
	template <class T>
	static void numberInt(T num, LynxString & strRef, int base = 10)
	{
		if ((base < 2) || (base > 16))
			return;

		if (num == 0)
		{
			strRef += '0';
			return;
		}

		bool sign = (num < 0);
		// Absolute vlaue
		if (sign)
			num = ~num + 1;

		T tempNumSub = num;
		char tempNum;

		int currentCount = strRef.count();

		while (tempNumSub > 0)
		{
			tempNumSub /= base;
			tempNum = char(num - tempNumSub * base);

			if (tempNum < 10)
				strRef += tempNum + '0';
			else
				strRef += tempNum - char(10) + 'a';

			num = tempNumSub;
		}

		if (sign)
			strRef += '-';

		strRef.reverse(currentCount);
	}

};

//-----------------------------------------------------------------------------------------------------------
//---------------------------------------- LynxRingBuffer ---------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
namespace LynxLib
{ 
	enum E_LynxRingBufferMode
	{
		eAutogrow = 0,		// Automatically grow the buffer if it is full
		eFixedOverwrite,	// Buffer has fixed size determined on construction (or with reserve()/resize()), old data is overwritten if it is full
		eFixedRefuse		// Buffer has fixed size determined on construction (or with reserve()/resize()), new data is ignored if it is full
	};
}

class LynxRingBuffer
{
public:
	LynxRingBuffer(int size = 0, LynxLib::E_LynxRingBufferMode mode = LynxLib::eAutogrow);
	~LynxRingBuffer();

	void deleteData();

	void reserve(int size);
	void resize(int size);

	char read();
	int read(LynxByteArray & buffer, int size = -1);
	void write(char data);
	int write(const LynxByteArray & buffer, int size = -1, int startIndex = 0);

	int count() const { return _count; }

private:
	char * _data;
	int _readIndex;
	int _writeIndex;
	int _reservedCount;
	int _count;
	const LynxLib::E_LynxRingBufferMode _mode;
};


#endif // !LYNXLISTCLASSES_H