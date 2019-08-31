#ifndef LYNX_STRUCTURE_H
#define LYNX_STRUCTURE_H
//-----------------------------------------------------------------------------------------------------------
//------------------------------------------ LynxStructure V2.0 ---------------------------------------------
//-----------------------------------------------------------------------------------------------------------

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

#define SIZE_64 sizeof(int64_t) // The local size of a 64 bit integer

#define LYNX_STATIC_HEADER 'A'	// Static header for Lynx datagrams (always the first byte of a datagram)
#define LYNX_HEADER_BYTES 5		// Number of header bytes
#define LYNX_CHECKSUM_BYTES 1	// Number of checksum bytes

#if !defined(ARDUINO) && !defined(TI)
#define LYNX_INCLUDE_EXCEPTIONS
#endif // !ARDUINO && !TI

#include <string.h>
// #include <math.h>

namespace LynxLib
{
	class LynxVar;

#ifdef LYNX_INCLUDE_EXCEPTIONS
	struct LynxMessages
	{
        static const char * outOfBoundsMsg;
	};
#endif // LYNX_INCLUDE_EXCEPTIONS

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
			if (_data != LYNX_NULL)
			{
				delete[] _data;
				_data = LYNX_NULL;
			}
		}

		LynxList & operator = (const LynxList & other)
		{
			if (&other == this)
				return *this;
			
			_count = 0;

			if (other._count < 1) // If the other list is empty, then copying it is pointless
				return *this;

			this->reserve(other._count);

			for (int i = 0; i < other._count; i++)
			{
				_data[i] = other._data[i];
			}

			_count = other._count;
			return *this;
		}

		T & operator [] (int index)
		{
#ifdef LYNX_INCLUDE_EXCEPTIONS
			if (index >= _count)
				throw LynxMessages::outOfBoundsMsg;
#endif // LYNX_INCLUDE_EXCEPTIONS

			return _data[index];
		}

		void clear() { _count = 0; }

		int count() const { return _count; }

		void reserve(int size)
		{
			_count = 0;

			if (size <= _reservedCount)
				return;

			if (_data != LYNX_NULL)
			{
				delete[] _data;
				_data = LYNX_NULL;
			}

			_reservedCount = size;
			_data = new T[_reservedCount];
		}

		void resize(int size)
		{
			if (size <= _reservedCount)
				return;

			if (_data == LYNX_NULL)
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

			delete[] oldData;
			oldData = LYNX_NULL;
		}

		int append()
		{
			this->resize(_count + 1);
			_data[_count] = T();
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

			_count += other._count;

			return (_count - 1);
		}

		void remove(int index)
		{
			if ((index < 0) || (index >= _count))
				return;

			for (int i = index; i < (_count - 1); i++)
			{
				_data[i] = _data[i + 1];
			}

			_count--;
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
#ifdef LYNX_INCLUDE_EXCEPTIONS
			if (index >= _count)
				throw LynxMessages::outOfBoundsMsg;
#endif // LYNX_INCLUDE_EXCEPTIONS

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

		const char & at(int index) const;
		char & operator [] (int index);

		char & first();
		const char & first() const;
		char & last();
		const char & last() const;

		int count() const;
		void clear() { _count = 0; }

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

		void resize(int size);

		LynxString subString(int startIndex, int endIndex);

		void append(const char & other);
		void append(const LynxString & other);
		void append(const char * const other, int maxLength = 255);

		void reverse();

		static LynxString number(int64_t num, int base = 10);
		static LynxString number(uint64_t num, int base = 10);
		static LynxString number(int32_t num, int base = 10);
		static LynxString number(uint32_t num, int base = 10);


		const char * toCharArray() const;
	private:
		char * _string;
		int _count;
		int _reservedCount;

		void reserve(int size);
		static int findTermChar(const char * str, int maxLength = 255);

		template <class T>
		static void numberTemplateInt(T num, LynxString & strRef, int base = 10)
		{
			strRef._count = 0;

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
			
			// This is quite pointless, since it's just a temporary return value anyway
			// only useful if you have extremely limited memory available
			// reserve space for largest possible represented number
			//switch (base)
			//{
			//case 2: // -111111111111111111111111111111111111111111111111111111111111111 64
			//	strRef.resize(64);
			//	break;
			//case 3: // -2021110011022210012102010021220101220221 61
			//	strRef.resize(61);
			//	break;
			//case 4: // -13333333333333333333333333333333 33
			//	strRef.resize(33);
			//	break;
			//case 5: // -1104332401304422434310311212 29
			//	strRef.resize(29);
			//	break;
			//case 6: // -1540241003031030222122211 26
			//	strRef.resize(26);
			//	break;
			//case 7: // -22341010611245052052300 24
			//	strRef.resize(24);
			//	break;
			//case 8: // -777777777777777777777 22
			//	strRef.resize(22);
			//	break;
			//case 9: // -67404283172107811827 21
			//	strRef.resize(21);
			//	break;
			//case 10: // -9223372036854775807 20
			//	strRef.resize(20);
			//	break;
			//case 11: // -1728002635214590697 20
			//	strRef.resize(20);
			//	break;
			//case 12: // -41a792678515120367 19
			//	strRef.resize(19);
			//	break;
			//case 13: // -10b269549075433c37 19
			//	strRef.resize(19);
			//	break;
			//case 14: // -4340724c6c71dc7a7 18
			//	strRef.resize(18);
			//	break;
			//case 15: // -160e2ad3246366807 18
			//	strRef.resize(18);
			//	break;
			//case 16: // -7fffffffffffffff 17
			//	strRef.resize(17);
			//	break;
			//}
			
			// Reserve space for largest possible represented number, 64 bit base 2
			strRef.resize(64);

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

			strRef.reverse();
		}

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

	};

	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------------- Types ------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

    enum E_LynxState
    {
        eNoChange = 0,
        eNewDataReceived,
		eDataCopiedToBuffer,
        eOutOfSync,
        eStructIdNotFound,
        eVariableIndexOutOfBounds,
        eBufferTooSmall,
        eWrongChecksum,
        eWrongStaticHeader,
        eWrongDataLength,
		eDataLengthNotFound,
		eNoStructuresInList,
		eStructIndexOutOfBounds,
		eSplitArrayFailed,
		eMergeArrayFailed,
        eEndiannessNotSet,
		eUnknownError
    };

	enum E_LynxDataType
	{
		eInvalidType = 0,
		eInt8,
		eUint8,
		eInt16,
		eUint16,
		eInt32,
		eUint32,
		eInt64,
		eUint64,
		eFloat,
		eDouble
	};

	enum E_Endianness
	{
		eNotSet = 0,
		eBigEndian,
		eLittleEndian
	};

	class LynxType
	{
	public:
		LynxType();
		LynxType(E_LynxDataType dataType);
        LynxType(const LynxType & other) { *this = other; }

        int8_t & var_i8() { return _data._var_i8; }
        uint8_t & var_u8() { return _data._var_u8; }
        int16_t & var_i16() { return _data._var_i16; }
        uint16_t & var_u16() { return _data._var_u16; }
        int32_t & var_i32() { return _data._var_i32; }
        uint32_t & var_u32() { return _data._var_u32; }
        float & var_float() { return _data._var_float; }
        int64_t & var_i64() { return _data._var_i64; }
        uint64_t & var_u64() { return _data._var_u64; }
        double & var_double() { return _data._var_double; }

        const int8_t & var_i8() const { return _data._var_i8; }
        const uint8_t & var_u8() const { return _data._var_u8; }
        const int16_t & var_i16() const { return _data._var_i16; }
        const uint16_t & var_u16() const { return _data._var_u16; }
        const int32_t & var_i32() const { return _data._var_i32; }
        const uint32_t & var_u32() const { return _data._var_u32; }
        const float & var_float() const { return _data._var_float; }
        const int64_t & var_i64() const { return _data._var_i64; }
        const uint64_t & var_u64() const { return _data._var_u64; }
        const double & var_double() const { return _data._var_double; }

		E_LynxDataType dataType() const { return _dataType; }
		int localSize() const;
		int transferSize() const;

		// static int splitVariable(LynxByteArray & buffer, int desiredSize);

        int toArray(LynxByteArray & buffer, E_LynxState & state) const;
        int fromArray(const LynxByteArray & buffer, int startIndex, E_LynxState & state);

		// If the program assumes the wrong endianness it can be set manually with this function
		static void setEndianness(E_Endianness endianness) { LynxType::_endianness = endianness; }

		static E_Endianness endianness() { return _endianness; }

		const LynxType & operator = (const LynxType & other) 
		{
			if (&other == this)
				return *this;

			_data._var_i64 = other._data._var_i64;
			_dataType = other._dataType;

			return *this;
        }

	private:
		union LynxUnion
		{
			char bytes[SIZE_64];
			int8_t _var_i8;
			uint8_t _var_u8;
			int16_t _var_i16;
			uint16_t _var_u16;
			int32_t _var_i32;
			uint32_t _var_u32;
			float _var_float;
			int64_t _var_i64;
			uint64_t _var_u64;
			double _var_double;
		}_data;

        E_LynxDataType _dataType;
        static E_Endianness _endianness;

	};

	struct LynxId
	{
		// LynxId() : structIndex(-1), variableIndex(-1) {}
		LynxId(int _structIndex = -1, int _variableIndex = -1) : structIndex(_structIndex), variableIndex(_variableIndex) {}

		bool operator == (const LynxId & other) const 
		{ 
			return ((structIndex == other.structIndex) && (variableIndex == other.variableIndex)); 
		}

		bool operator != (const LynxId & other) const 
		{
			return ((structIndex != other.structIndex) || (variableIndex != other.variableIndex));
		}


		int structIndex;
		int variableIndex;
	};

	struct LynxInfo
	{
       LynxInfo() : deviceId(0), lynxId(), dataLength(0), state(eNoChange) {}

		char deviceId;
		LynxId lynxId;
		int dataLength;
        E_LynxState state;

	};

	//-----------------------------------------------------------------------------------------------------------
	//------------------------------------- Static Functions ----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

    int splitArray(LynxByteArray & buffer, int desiredSize);

    int mergeArray(LynxByteArray & buffer, int desiredSize);

    char sizeMask(int shiftSize);

	int localSize(E_LynxDataType dataType);

	int transferSize(E_LynxDataType dataType);

	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------- LynxStructure ----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	class LynxStructure : LynxList<LynxType>
	{
	public:
		LynxStructure(int size = 0);

		const LynxStructure & operator = (const LynxStructure & other)
		{
			LynxList::operator=(other);
			_structId = other._structId;
			_localSize = other._localSize;
			_transferSize = other._transferSize;

			return *this;
		}

		using LynxList::operator[];
		using LynxList::at;
		using LynxList::count;
		using LynxList::reserve;

		// Creates a buffer with the desired information, and returns it
		// LynxByteArray toArray(int variableIndex = -1) const;

		// Copies the desired information to the provided buffer
		E_LynxState toArray(LynxByteArray & buffer, int variableIndex = -1) const;

		// Copies the required information to the char array
		E_LynxState toArray(char * buffer, int maxSize, int & copiedSize, int variableIndex = -1) const;

		// Copies information from the provided buffer
        void fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo);

		// Copies information from char array, and returns number of bytes copied
        void fromArray(const char * buffer, int size, LynxInfo & lynxInfo);

		// Manually add a variable to the variable list
		LynxId addVariable(int structIndex, E_LynxDataType dataType);

		// Returns the transfersize of requested data (not including header and checksum)
		int transferSize(int variableIndex = -1) const;

		// Returns the local size of requested data (not including header and checksum)
		int localSize(int variableIndex = -1) const;

        void setStructId(char structId) { _structId = structId; }
		char structId() const { return _structId; }

	private:
		char _structId;
		int _localSize;
		int _transferSize;
	};

	class LynxManager : private LynxList<LynxStructure>
	{
	public:
		LynxManager(char deviceId = char(0xff), int size = 0);

		using LynxList::count;

		void setDeviceId(char deviceId) { _deviceId = deviceId; }

        char structId(const LynxId & lynxId);

		LynxType & variable(const LynxId & lynxId);
		const LynxType & variable(const LynxId & lynxId) const;

		void copy(const LynxId & source, const LynxId & target);

		// Creates a buffer with the desired information, and returns it
		// LynxByteArray toArray(const LynxId & lynxId) const;

		// Copies the desired information to the provided buffer
		E_LynxState toArray(LynxByteArray & buffer, const LynxId & lynxId) const;

		// Copies the required information to the char array, and returns number of bytes copied
		E_LynxState toArray(char * buffer, int maxSize, int & copiedSize, const LynxId & lynxId) const;

		// Copies information from the provided buffer
		void fromArray(const LynxByteArray & buffer, LynxInfo & lynxInfo);

		// Copies information from char array, and returns number of bytes copied
		void fromArray(const char * buffer, int size, LynxInfo & lynxInfo);

		int transferSize(const LynxId & lynxId) const;
		int localSize(const LynxId & lynxId) const;

		LynxId addStructure(char structId, int size = 0);
		LynxVar addVariable(const LynxId & lynxId, E_LynxDataType dataType);

		// Returns number of variables in struct. Returns 0 if out of bounds
		int structVariableCount(int structIndex);

		int findId(char structId);

	private:
		char _deviceId;
		LynxType _dummyVariable;
	};

	//-----------------------------------------------------------------------------------------------------------
	//---------------------------------------- LynxDataTypes ----------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	class LynxVar
	{
	public:
        LynxVar(LynxManager * const lynxManager, const LynxId & lynxId) : _lynxId(lynxId), _lynxManager(lynxManager) {}
		const LynxId & lynxId() const { return _lynxId; }

		const LynxVar & operator = (const LynxVar & other)
		{
			_lynxManager->copy(other._lynxId, this->_lynxId);
			return *this;
		}

	protected:
		const LynxId _lynxId;
		LynxManager * const _lynxManager;
	};

	class LynxVar_i8 : public LynxVar
	{
	public:
		LynxVar_i8(const LynxVar & other) : LynxVar(other) {}
		
		operator const int8_t&() const { return _lynxManager->variable(_lynxId).var_i8(); }

		const int8_t & operator = (const int8_t & other) 
		{
			return (_lynxManager->variable(_lynxId).var_i8() = other);
		}
	};


	class LynxVar_u8 : public LynxVar
	{
	public:
		LynxVar_u8(const LynxVar & other) : LynxVar(other) {}

		operator const uint8_t&() const { return _lynxManager->variable(_lynxId).var_u8(); }

		const uint8_t & operator = (const uint8_t & other)
		{
			return (_lynxManager->variable(_lynxId).var_u8() = other);
		}
	}; 

	class LynxVar_i16 : public LynxVar
	{
	public:
		LynxVar_i16(const LynxVar & other) : LynxVar(other) {}

		operator const int16_t&() const { return _lynxManager->variable(_lynxId).var_i16(); }

		const int16_t & operator = (const int16_t & other)
		{
			return (_lynxManager->variable(_lynxId).var_i16() = other);
		}
	};

	class LynxVar_u16 : public LynxVar
	{
	public:
		LynxVar_u16(const LynxVar & other) : LynxVar(other) {}

		operator const uint16_t&() const { return _lynxManager->variable(_lynxId).var_u16(); }

		const uint16_t & operator = (const uint16_t & other)
		{
			return (_lynxManager->variable(_lynxId).var_u16() = other);
		}
	};

	class LynxVar_i32 : public LynxVar
	{
	public:
		LynxVar_i32(const LynxVar & other) : LynxVar(other) {}

		operator const int32_t&() const { return _lynxManager->variable(_lynxId).var_i32(); }

		const int32_t & operator = (const int32_t & other)
		{
			return (_lynxManager->variable(_lynxId).var_i32() = other);
		}
	};

	class LynxVar_u32 : public LynxVar
	{
	public:
		LynxVar_u32(const LynxVar & other) : LynxVar(other) {}

		operator const uint32_t&() const { return _lynxManager->variable(_lynxId).var_u32(); }

		const uint32_t & operator = (const uint32_t & other)
		{
			return (_lynxManager->variable(_lynxId).var_u32() = other);
		}
	};

	class LynxVar_i64 : public LynxVar
	{
	public:
		LynxVar_i64(const LynxVar & other) : LynxVar(other) {}

		operator const int64_t&() const { return _lynxManager->variable(_lynxId).var_i64(); }

		const int64_t & operator = (const int64_t & other)
		{
			return (_lynxManager->variable(_lynxId).var_i64() = other);
		}
	};

	class LynxVar_u64 : public LynxVar
	{
	public:
		LynxVar_u64(const LynxVar & other) : LynxVar(other) {}

		operator const uint64_t&() const { return _lynxManager->variable(_lynxId).var_u64(); }

		const uint64_t & operator = (const uint64_t & other)
		{
			return (_lynxManager->variable(_lynxId).var_u64() = other);
		}
	};

	class LynxVar_float : public LynxVar
	{
	public:
		LynxVar_float(const LynxVar & other) : LynxVar(other) {}

		operator const float&() const { return _lynxManager->variable(_lynxId).var_float(); }

		const float & operator = (const float & other)
		{
			return (_lynxManager->variable(_lynxId).var_float() = other);
		}
	};

	class LynxVar_double : public LynxVar
	{
	public:
		LynxVar_double(const LynxVar & other) : LynxVar(other) {}

		operator const double&() const { return _lynxManager->variable(_lynxId).var_double(); }

		const double & operator = (const double & other)
		{
			return (_lynxManager->variable(_lynxId).var_double() = other);
		}
	};

	// extern LynxManager Lynx;
}
#endif // !LYNX_STRUCTURE_H