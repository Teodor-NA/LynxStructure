//-------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------- Version 1.4.0.2 ----------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------

#ifndef LYNX_STRUCTURE
#define LYNX_STRUCTURE

#ifndef LYNX_NULL
#ifdef TI
#define LYNX_NULL 0
#else
#define LYNX_NULL nullptr
#endif // TI
#endif // !LYNX_NULL

#ifdef TI
#include "DSP28x_Project.h"
typedef int16_t int8_t;
#endif //TI

#include <stdint.h>
#include <math.h>
#define LYNX_VERSION { 1, 4, 0, 3 }

#define LYNX_INTERNAL_DATAGRAM char(255)

#define LYNX_ID_BYTES 3
#define LYNX_INDEXER_BYTES 2
#define LYNX_CHECKSUM_BYTES 1

//---------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------- Macros --------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------

#define LYNX_WRAPPER_ELEMENT_INIT(name, type) \
ptr_##name = reinterpret_cast<type*>(lynxHandler->getDataPointer(lynxID, name));



namespace LynxLib
{

	//---------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------- Enums ---------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------

	enum LynxDataType
	{
		eEndOfList = 0,
		eNoStorage,
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

	enum LynxStructMode
	{
		eStructureMode = 1,
		eArrayMode
	};

    enum InternalStructIDs
	{
		eSsInvalidID = 0,
		eLynxRequest,
		eLynxResponse,
        eLynxString,
		eSsEndOfList
	};

	enum RequestIDs
	{
		eRqInvalidID = 0,
		eRqDeviceInfo,
		eRqStructInfo
	};

	//---------------------------------------------------------------------------------------------------------------------------
	//---------------------------------------------------- LynxList -------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------

	template <class T>
	class LynxList
	{
	public:
		LynxList()
		{
			_list = LYNX_NULL;
			_count = 0;
			_reservedSize = 0;
		}

		LynxList(int size) : LynxList()
		{
			this->reserve(size);
		}

		LynxList(const LynxList<T>& list) : LynxList()
		{ 
			*this = list;
		}

		LynxList(const T* source, int size) : LynxList()
		{
			this->append(source, size);
		}

		~LynxList()
		{
			if (_list != LYNX_NULL)
			{
				delete[] _list;
				_list = LYNX_NULL;
			}
		}

		LynxList<T>& operator = (const LynxList<T>& other)
		{
			if (this == &other)
				return *this;

			this->clear();
			this->append(other);

			return *this;
		}

		bool operator == (const LynxList<T>& other) const
		{
			if (this->count() != other.count())
				return false;

			for (int i = 0; i < this->count(); i++)
			{
				if (this->_list[i] != other.at(i))
					return false;
			}

			return true;
		}

		T& operator [] (int index) { return _list[index]; }

		// Returns a const safe reference to member at index
		const T& at(int index) const { return _list[index]; }

		// Destructive reserve. Allocates new memory, and deletes all data.
		// Will shrink if size is less than reserved size.
		void reserve(int size)
		{
			if (_list != LYNX_NULL)
			{
				delete[] _list;
				_list = LYNX_NULL;
			}

			_count = 0;
			_reservedSize = size;
			_list = new T[size];
		}

		// Non-destructive reserve. Allocates new memory and copies "copysize" number of old data.
		// Copies all old data if "copySize" is negative or not in argument list.
		// Will not shrink if size is less than reserved size!
		// Returns true if new reserve was neccessary, otherwise false
		bool reserveAndCopy(int size, int copySize = -1)
		{
			if (_reservedSize >= size)
				return false;

			_reservedSize = size;

			if (_list != LYNX_NULL)
			{
				T* temp = _list;
				_list = new T[_reservedSize];

				if (copySize >= 0)
					_count = copySize;

				// _count = _count < _reservedSize ? _count : _reservedSize;
				for (int i = 0; i < _count; i++)
				{
					_list[i] = temp[i];
				}

				delete[] temp;
				temp = LYNX_NULL;
			}
			else
			{
				_count = 0;
				_list = new T[_reservedSize];
			}

			return true;
		}

		// Adds a new item at the end of the list and populates it with "item".
		void append(const T& item)
		{
			if (_count >= _reservedSize)
				this->reserveAndCopy(_count + 1);

			_list[_count] = item;
			_count++;
		}

		// Adds a new empty instance at the end of the list.
		void append() { this->append(T()); }

		// Appends list at the end of the current list, expands allocated memory if neccessary
        void append(const LynxList<T>& other)
		{
            int prevCount = _count;

            if ((other.count() + prevCount) > _reservedSize)
                this->reserveAndCopy(other.count() + prevCount);

            for (int i = 0; i < other.count(); i++)
			{
                this->_list[prevCount + i] = other.at(i);
				_count++;
			}
		}

		// Appends array at the end of the current list, expands allocated memory if neccessary
		void append(const T* source, int size)
		{
            int prevCount = _count;

            if ((prevCount + size) > _reservedSize)
                this->reserveAndCopy(prevCount + size);

            for (int i = 0; i < size; i++)
			{
                this->_list[prevCount + i] = source[i];
				_count++;
			}
		}


		// Removes the indexed item and shifts the remaining items to fill
		// Returns number of remaining items if successful, -1 if failed
		int remove(int index)
		{
			if (index < this->_count)
			{
				_count--;
				if (_count <= 0)
				{
					return this->_count;
				}

				for (int i = index; i < _count; i++)
				{
					_list[i] = _list[i + 1];
				}
				return this->_count;
			}

			return -1;
		}

		// Clears the list and frees the allocated memory
		void deleteList()
		{
			_count = 0;
			_reservedSize = 0;
			if (_list != LYNX_NULL)
			{
				delete[] _list;
				_list = LYNX_NULL;
			}
		}

		// Clears the list but does not free allocated memory
		void clear() { _count = 0; }

		// Returns number of elements in list
		int count() const { return _count; }

		int reservedSize() const { return _reservedSize; }

	private:
		T* _list;

		int _count;
		int _reservedSize;
	};

	//---------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------- LynxString -------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------

	class LynxString : public LynxList<char>
	{
		int checkCstringSize(const char * cstring) const;
		void completeClear() { LynxList::clear(); }

		//template <class T>
		//static LynxString & numToStr(LynxString & str, T num, int base)
		//{
		//	if (num == 0)
		//	{ 
		//		str = '0';
		//		return str;
		//	}

		//	T tmp = num;
		//	int charsNeeded = 0;

		//	while (tmp != 0)
		//	{
		//		tmp /= base;
		//		charsNeeded++;
		//	}

		//	bool negNum;
		//	if (num < 0)
		//	{
		//		negNum = true;
		//		charsNeeded++;
		//		tmp = -num;
		//	}
		//	else
		//	{
		//		negNum = false;
		//		tmp = num;
		//	}

		//	str.reserveAndCopy(charsNeeded, 0);
		//	// str.clear();
		//	// LynxString tmpString(charsNeeded);

		//	while (tmp != 0)
		//	{
		//		str += static_cast<char>(tmp % base) + '0';
		//		tmp /= base;
		//	}

		//	if (negNum)
		//		str.append('-');

		//	str.reverse();

		//	return str;
		//}

		//template <class T>
		//static LynxString & fNumToStr(LynxString & str, T num, int precision)
		//{
		//	bool sign = num < 0; // negative = true, positive = false

		//	T iPart = fabs(trunc(num));
		//	T fPart = fabs(num) - iPart;

		//	LynxString iStr = LynxString::number(static_cast<long long>(iPart));

		//	int fSize = precision - iStr.count();

		//	LynxString fStr;  
		//	if (fSize >= 1)
		//	{ 
		//		fStr.reserve(fSize);
		//		while (fStr.count() < fSize)
		//		{
		//			fPart *= 10.0;
		//			fStr += LynxString::number(static_cast<int>(fPart));
		//			fPart = fPart - static_cast<int>(fPart);
		//		}
		//	}

		//	str.reserve(iStr.count() + fStr.count() + 2);

		//	if (sign)
		//		str += '-';

		//	str += iStr;
		//	if(fSize >= 1)
		//	{
		//		str += '.';
		//		str += fStr;
		//	}
		//	return str;
		//}

	public:
		LynxString();
		LynxString(int size);
		LynxString(char character);
		LynxString(const char * cstring, int size = 0);

		const LynxString & operator += (char character) { this->append(character); return *this; }
		const LynxString & operator += (const char * cstring) { this->append(cstring); return *this; }
		const LynxString & operator += (const LynxString & other) { this->append(other); return *this; }

		LynxString operator + (char character)
		{
			LynxString newString(this->count() + 1);
			newString = *this;
			newString.append(character);
			return newString;
		}

		LynxString operator + (const char * cstring)
		{
			int cstrSize = this->checkCstringSize(cstring);
			if (cstrSize < 1)
				return *this;

			LynxString newString(this->count() + cstrSize);
			newString = *this;
			newString.append(cstring);
			return newString;
		}

		LynxString operator + (const LynxString & other)
		{
			LynxString newString(this->count() + other.count());
			newString = *this;
			newString.append(other);
			return newString;
		}

		void append(char character);
		void append(const char * cstring, int size = 0);
		void append(const LynxString & other);

	//	static LynxString number(int num, int base = 10) { LynxString tmp; return LynxString::numToStr<int>(tmp, num, base); }
	//  static LynxString number(long long num, int base = 10) { LynxString tmp; return LynxString::numToStr<long long>(tmp, num, base); }
	//  static LynxString number(float num, int precision = 5) { LynxString tmp; return LynxString::fNumToStr<float>(tmp, num, precision); }
	//  static LynxString number(double num, int precision = 5) { LynxString tmp; return LynxString::fNumToStr<double>(tmp, num, precision); }

        const char * toCstr() const { return &this->at(0); }

		const LynxString & reverse();

		void remove(int index) { if(index < this->count()) LynxList::remove(index);  }

		int count() const { return (LynxList::count() < 1) ? 0 : (LynxList::count() - 1); }

		void reserve(int size);

		bool reserveAndCopy(int size, int copySize = -1);

		void clear() { LynxList::clear(); LynxList::append('\0'); }
	};

	//---------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------- Structures ------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------


	struct LynxID
	{
		LynxID()
		{
			deviceID = 0;
			structTypeID = 0;
			structInstanceID = 0;
		}

		LynxID(uint8_t _deviceID, uint8_t _structTypeID, uint8_t _structInstanceID)
		{
			deviceID = _deviceID;
			structTypeID = _structTypeID;
			structInstanceID = _structInstanceID;
		}

		LynxID& operator = (const LynxID other)
		{
			this->deviceID = other.deviceID;
			this->structTypeID = other.structTypeID;
			this->structInstanceID = other.structInstanceID;

			return *this;
		}

		bool operator == (const LynxID& other) const
		{
			if ((this->structTypeID == other.structTypeID) && (this->structInstanceID == other.structInstanceID))
				return true;

			return false;
		}

		bool operator != (const LynxID& other) const { return !(*this == other); }

		uint8_t deviceID;			// Identifies the current machine
		uint8_t structTypeID;		// Identifies the type of struct
		uint8_t structInstanceID;	// Identifies the instance of the struct
	};

	struct StructItem
	{
        StructItem() { name = LYNX_NULL; dataType = eEndOfList; }
		StructItem(const char* _name, LynxDataType _dataType)
			: name(_name), dataType(_dataType) {}
		const char* name;
		LynxDataType dataType;
	};

	struct StructDefinition
	{
		StructDefinition(const char* _structName, const LynxStructMode _structMode, const StructItem* _structItems, int _size = 0);

		const char* structName;
		const LynxStructMode structMode;
		LynxList<StructItem> structItems;
		int size;							// Number of elements in the list
		int transferSize = 0;				// Total data transfer size in bytes (not including identifiers and checksum)
		int localSize = 0;					// Total data storage size in bytes
	};

	struct LynxIpAddress
	{
		union Data
		{
			uint32_t data_32;
			unsigned char data_8[4];
		}data;

		LynxIpAddress() { data.data_32 = 0; }

		LynxIpAddress(uint32_t _data) { data.data_32 = _data; }

		LynxIpAddress(const unsigned char _data[4])
		{
			for (int i = 0; i < 4; i++)
			{
				data.data_8[i] = _data[i];
			}
		}

		LynxIpAddress(unsigned char ip0, unsigned char ip1, unsigned char ip2, unsigned char ip3)
		{
			data.data_32 = 1;

			if (data.data_8[0] == 1)	// Little endian
			{
				data.data_8[0] = ip3;
				data.data_8[1] = ip2;
				data.data_8[2] = ip1;
				data.data_8[3] = ip0;
			}
			else						// Big endian
			{
				data.data_8[0] = ip0;
				data.data_8[1] = ip1;
				data.data_8[2] = ip2;
				data.data_8[3] = ip3;
			}

		}

	};

	struct LynxDeviceInfo
	{
		LynxDeviceInfo()
		{
			deviceName[0] = '\0';
			deviceID = 0;
			for (int i = 0; i < 4; i++)
			{
				lynxVersion[i] = 0;
			}
			ipAddress = LynxIpAddress();
			newDevice = false;
		}

		char deviceName[20];
		uint8_t deviceID;
		char lynxVersion[4];
		LynxIpAddress ipAddress;
		bool newDevice;
	};

	//---------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------- LynxRingBuffer ---------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------

	template <class T>
	class LynxRingBuffer
	{
	private:
		void writeElement(T element)
		{
			_buffer[_writeIndex] = element;

			_writeIndex++;

			if (_writeIndex >= _reservedSize)
				_writeIndex = 0;
		}

	public:
		enum E_RingBufferMode
		{
			eAllowOverflow = 0,
			ePreventOverflow,
			eRingBufferMode_EndOfList
		};

		LynxRingBuffer(E_RingBufferMode overflowMode = ePreventOverflow)
		{
			if (overflowMode < eRingBufferMode_EndOfList)
				_overflowMode = overflowMode;
			else
				_overflowMode = ePreventOverflow;

			_buffer = LYNX_NULL;
			_reservedSize = 0;
			_writeIndex = 0;
			_readIndex = 0;
			_count = 0;
		}

		LynxRingBuffer(int size, E_RingBufferMode overflowMode = ePreventOverflow)
		{
			if (overflowMode < eRingBufferMode_EndOfList)
				_overflowMode = overflowMode;
			else
				_overflowMode = ePreventOverflow;

			_buffer = LYNX_NULL;

			this->init(size);
		}

		~LynxRingBuffer()
		{
			if (_buffer)
			{
				delete[] _buffer;
				_buffer = LYNX_NULL;
			}
		}

		// Initializer, reserves space for data. If init has been run before, previous data will be deleted. 
		// Returns number of elements reserved (should be the same as "size"), or -1 if error.
		int init(int size)
		{
			if (_buffer)
			{
				delete[] _buffer;
				_buffer = LYNX_NULL;
			}

			_buffer = new T[size];

			if (_buffer)
			{
				_count = 0;
				_writeIndex = 0;
				_readIndex = 0;
				_reservedSize = size;

				return _reservedSize;
			}

			return -1;
		}

		bool setOverflowMode(E_RingBufferMode overflowMode)
		{
			if (overflowMode >= eRingBufferMode_EndOfList)
				return false;

			_overflowMode = overflowMode;
			return true;
		}

		E_RingBufferMode getOverflowMode()
		{
			return _overflowMode;
		}

		// Returns number of elements currently stored in buffer.
        int count() const
		{
			return _count;
		}

		// Returns free space in buffer.
        int freeCount() const
		{
			return (_reservedSize - _count);
		}

        int reservedSize() const
		{
			return _reservedSize;
		}

		// Returns one element from the buffer. If there is nothing in the buffer, 0 is returned.
        T read()
		{
			if (_count <= 0)
				return T();

			T temp = _buffer[_readIndex];

			_count--;
			_readIndex++;
			if (_readIndex >= _reservedSize)
				_readIndex = 0;

			return temp;
		}

		// Reads "count" elements from the buffer and places it in "target".
		// If "count" is greater then number of elements in buffer, available elements will be read.
		// Returns number of elements read. 
        int read(T* target, int count)
		{
			int size;
			if (count > _count)
				size = _count;
			else
				size = count;

			for (int i = 0; i < size; i++)
			{
				target[i] = this->read();
			}

			return size;
		}
        int read(LynxLib::LynxList<T>& target, int count)
                       {
                           target.reserveAndCopy(count);

                           for (int i = 0; i < count; i++)
                           {
                               target.append(this->read());
                           }

                           return count;
                       }
        int read(LynxLib::LynxList<T>& target)
		{
            int size = this->count();
            target.append(this->_buffer, size);

            for (int i = 0; i < size; i++)
			{
				_count--;
				_readIndex++;
				if (_readIndex >= _reservedSize)
					_readIndex = 0;
			}

            return size;
		}

		// Writes one element "source" to the buffer
		// Overflow allowed:
		//		Returns true if the buffer is overflowing (data will be overwritten), otherwise false
		// Overflow prevented:
		//		Returns true if the buffer is full (data will not be written), otherwise false
		bool write(T source)
		{
			if (_count < _reservedSize)
			{
				writeElement(source);
				_count++;
				return false;
			}

			switch (_overflowMode)
			{
			case eAllowOverflow:
				writeElement(source);
				break;
			case ePreventOverflow:
				break;
            default:
                break;
			}

			return true;

		}

		// Writes "count" elements from "source" to the buffer.
		// Overflow allowed:
		//		Returns -1 * elements written if the buffer is overflowing (data will be overwritten), otherwise it returns elements written.
		// Overflow prevented:
		//		Returns -1 * elements written if the buffer is full (data above max index will not be written), otherwise it returns elements written
		int write(const T* source, int count)
		{
			bool overflow;

			for (int i = 0; i < count; i++)
			{
				overflow = this->write(source[i]);

				if (overflow && (_overflowMode == ePreventOverflow))
					break;
			}

			if (overflow)
				return (~count + 1);

			return count;
		}

	private:
		T* _buffer;

		int _count;
		int _reservedSize;

		int _writeIndex;
		int _readIndex;

		E_RingBufferMode _overflowMode;
	};

	//---------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------- LynxStructure v1.4 --------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------


	class LynxStructure
	{
	public:

		LynxStructure()
		{
			_data = LYNX_NULL;
			_structDefinition = LYNX_NULL;
		}

		LynxStructure(const StructDefinition& structDefinition, const LynxID& lynxID) : LynxStructure()
		{
			this->init(structDefinition, lynxID);
		}

		~LynxStructure()
		{
			if (_data != LYNX_NULL)
			{
				delete[] _data;
				_data = LYNX_NULL;
			}
		}

		LynxStructure& operator = (const LynxStructure& other)
		{
			if (other.getStructDefinition() == LYNX_NULL)
				return *this;

			this->init(*(other.getStructDefinition()), other.getID());

			return *this;
		}

		// Initializes current LynxStructure.
		// Not neccessary if the constructor with arguments is used.
		// WARNING: Use of uninitialized LynxStructure will lead to null pointer reference exception!
		void init(const StructDefinition& structDefinition, const LynxID& lynxID);

        // Prepares data for transmission in "dataBuffer"
        // Returns size of copied data if success, negative error code if failure
		int toBuffer(LynxList<char>& dataBuffer, int subindex = -1) const;

        // Attempts to copy data from dataBuffer to Lynx
        // Returns size of copied data if success, negative error code if failure
		int fromBuffer(const LynxList<char>& dataBuffer);

		// Sets all elements in structure to 0 (on storage level)
        void clear();

		// Returns a const safe pointer to struct def
		const StructDefinition* getStructDefinition() const { return _structDefinition; }

		// Returns number of elements in struct
		int getSize() const { return this->_structDefinition->size; }

		// Returns the size of the datapackage in bytes. Not including identification bytes and checksum.
		int getTransferSize(int subIndex = -1) const;

		// Returns the size of the data in storage.
		int getLocalSize(int subIndex = -1) const;

		// Returns a pointer to data
		void* getDataPointer(int subIndex = -1);

		// Returns a const safe pointer to storage data
		const void* getConstDataPointer(int subIndex = -1) const;

		// Reads from the vairiable pointed to by subIndex
		template <class T>
		const T& getData(int subIndex) const
		{
			return *reinterpret_cast<const T*>(_dataPointerList.at(subIndex));
		}

		// Writes dataIn to the vairiable pointed to by subIndex
		template <class T>
		void setData(int subIndex, const T& dataIn)
		{
			*(reinterpret_cast<T*>(_dataPointerList[subIndex])) = dataIn;
		}

		// Returns the boolean state of the bits in bitmask
		// If bitmask has more than one high bit, true will be returned if any of the corresponding bits in storage are high
		template <class T>
		bool getBit(int subIndex, T bitMask) const
		{
			return ((this->getData<T>(subIndex) & bitMask) != 0);
		}

        // Sets all high bits in the bitmask to the state provided
		template <class T>
		void setBit(int subIndex, T bitMask, bool state)
		{
            if (state)
			{
                this->setData<T>(subIndex, this->getData<T>(subIndex) | bitMask);
            }
            else
            {
                this->setData<T>(subIndex, this->getData<T>(subIndex) & ~bitMask);
			}
		}

		// Checks how much space the datatype takes in local storage
		static int checkLocalSize(LynxDataType dataType);
		// Checks how many bytes the datatype takes in transfer
		static int checkTransferSize(LynxDataType dataType);
		// Returns this datagram's LynxID
		const LynxID& getID() const { return _lynxID; }

	private:
		// Writes one variable from the lynx storage to "dataBuffer"
		int writeVarToBuffer(LynxList<char>& dataBuffer, int subIndex) const;
		// Writes one variable from "dataBuffer" to the lynx storage
		int writeVarFromBuffer(const LynxList<char>& dataBuffer, int bufferIndex, int subIndex);

		// Identifies current datagram
		LynxID _lynxID;

		// Stored data pointer
		char* _data;
		// List of pointers to variables
		LynxList<void*> _dataPointerList;
		// Pointer to the struct definition
		const StructDefinition* _structDefinition;

	};

	//---------------------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------- LynxHandler ------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------------------------

	class LynxHandler
	{
	public:
		// uint8_t deviceID;

		LynxHandler(uint8_t deviceID, const char* deviceName, int nStructs = 0)
		{
			this->_deviceInfo.deviceID = deviceID;

			char tempVersion[4] = LYNX_VERSION;
			for (int i = 0; i < 4; i++)
			{
				_deviceInfo.lynxVersion[i] = tempVersion[i];
			}

			for (int i = 0; i < 20; i++)
			{
				if ((deviceName[i] == '\0') || (i == 19))
				{
					this->_deviceInfo.deviceName[i] = '\0';
					break;
				}
				this->_deviceInfo.deviceName[i] = deviceName[i];
			}

			_newScanRequest = false;
			this->init(nStructs);
		}

		~LynxHandler()
		{
		}

		void init(int nStructs = 0);

        // Adds a structure with the definition provided to the handler's structure list
        // Returns a unique LynxID that can be used to interact with the structure
		LynxID addStructure(uint8_t _structType, uint8_t _structInstance, const StructDefinition& _structDefinition);

        // Send a scan request
        // NB: not tested with lynx 1.4, cannot guarantee results
		int scanRequest(char* dataBuffer);

        // Copies data from source to target
        // If subIndex is less than zero or empty all variables are copied, otherwise the respective subIndex is copied
        // Returns number of bytes copied, or a negative errorcode
		int copyData(const LynxID& source, const LynxID& target, int subIndex = -1);

        // Reads from the vairiable pointed to by subIndex in the struct pointed to by lynxID
		template <class T>
		T getData(const LynxID& lynxID, int subIndex) const
		{
			int index = this->indexFromID(lynxID);

			if (index >= 0)
			{
				return this->_structures.at(index).getData<T>(subIndex);
			}

			return T();
		}

        // Writes to the vairiable pointed to by subIndex in the struct pointed to by lynxID
		template <class T>
		void setData(const LynxID& lynxID, int subIndex, const T& data)
		{
			int index = this->indexFromID(lynxID);

			if (index >= 0)
			{
				this->_structures[index].setData<T>(subIndex, data);
			}
		}

        // Returns the boolean state of the bits in bitmask
        // If bitmask has more than one high bit, true will be returned if any of the corresponding bits in storage are high (compound OR)
		template <class T>
		bool getBit(const LynxID& lynxID, int subIndex, T bitMask) const
		{
			int index = this->indexFromID(lynxID);
			if (index < 0)
			{
				return false;
			}

			return this->_structures.at(index).getBit<T>(subIndex, bitMask);
		}

        // Sets all high bits in the bitmask to the state provided
		template <class T>
		void setBit(const LynxID& lynxID, int subIndex, T bitMask, bool state)
		{
			int index = this->indexFromID(lynxID);
			if (index < 0)
			{
				return;
			}

			this->_structures[index].setBit<T>(subIndex, bitMask, state);
		}

        // Prepares data for transmission in dataBuffer
        // Returns size of copied data if success, negative error code if failure
		int toBuffer(const LynxID& lynxID, LynxList<char>& dataBuffer, int subIndex = -1) const;

        // Attempts to copy data from dataBuffer to Lynx
        // Returns size of copied data if success, negative error code if failure
		int fromBuffer(const LynxList<char>& dataBuffer, LynxIpAddress ipAddress = LynxIpAddress());

        // Returns the transfer size of the struct pointed to by lynxID
        // If subIndex is less than zero, the total size of all elements is returned, otherwise only the variable pointed to by subIndex
        // Identification and checksum bytes are not included
        // Returns a negative error code if feilure
		int getTransferSize(const LynxID& lynxID, int subIndex = -1) const;

        // Returns the local storage size of the struct pointed to by lynxID (sizeof equivalent)
        // If subIndex is less than zero, the total size of all elements is returned, otherwise only the variable pointed to by subIndex
        // Returns a negative error code if feilure
        int getLocalSize(const LynxID& lynxID, int subIndex = -1) const;

        // Returns a pointer to the struct pointed to by lynxID
        // If subIndex is less than zero, a pointer to the first element is returned
        // Otherwise the element pointed to by subindex is returned
		void* getDataPointer(const LynxID& lynxID, int subIndex = -1);

        // Returns a const safe pointer to the struct pointed to by lynxID
        // If subIndex is less than zero, a pointer to the first element is returned
        // Otherwise the element pointed to by subindex is returned
		const void* getConstDataPointer(const LynxID& lynxID, int subIndex = -1) const;

        // Returns a list of all lynxID's that currently exists in the handler
		LynxList<LynxID> getIDs();

        // Populates list input with all the lynxID's that currently exists in the handler
		void getIDs(LynxList<LynxID>& list);

        // Returns number of remaining scan responses
        // NB: not tested with lynx 1.4, cannot guarantee results
        int newScanResponses();

        // Returns a scan response from the list, and removes it from the list
        // NB: not tested with lynx 1.4, cannot guarantee results
		LynxDeviceInfo getScanResponse();

        // Sends a scan request
        // NB: not tested with lynx 1.4, cannot guarantee results
		bool newScanRequest() { return _newScanRequest; }

        // Sends a scan response
        // NB: not tested with lynx 1.4, cannot guarantee results
		int sendScanResponse(char* dataBuffer);

        // Returns true if lynxID exists in the hendler, false if not
		bool isMember(const LynxID& lynxID);

        // Sets all elements in structure pointed to by lynxID to 0 (on storage level)
        // Returns zero if success, error code if faliure
        int clear(const LynxID & lynxID);

        // Gets last received string
        const LynxString & getReceivedString() const { return _receiveString; }

	private:
		LynxDeviceInfo _deviceInfo;

		bool _newScanRequest;

        LynxString _receiveString;

		LynxList<LynxDeviceInfo> _availableDevices;

		int checkAvailableDevices(LynxDeviceInfo& deviceName);

		LynxList<LynxStructure> _structures;

		int indexFromID(const LynxID& lynxID) const;

        int handleInternalDatagram(const LynxList<char> & dataBuffer, LynxIpAddress ipAddress);

		int handleRequest(const char* dataBuffer, LynxIpAddress ipAddress);

		int handleResponse(const char* dataBuffer, LynxIpAddress ipAddress);

        // Receives a string from the databuffer
        // Returns size of received data
        int receiveString(const LynxList<char> & dataBuffer);

		LynxDeviceInfo receiveScanResponse(const char* dataBuffer, LynxIpAddress ipAddress);

		int datagramFromBuffer(const LynxList<char>& dataBuffer);
	};

    //---------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------- LynxWrapper ------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------------

    template<class T>
    struct LynxWrapperElement
    {
        LynxWrapperElement () { _dataPointer = LYNX_NULL; }

        LynxWrapperElement (LynxHandler * lynxHandler, const LynxID lynxID, int subIndex) { this->connect(lynxHandler, lynxID, subIndex); }

        const T& operator = (const T& other)
        {
            *_dataPointer = other;
            return *_dataPointer;
        }

        operator T() { return *_dataPointer; }

        void connect(LynxHandler * lynxHandler, const LynxID lynxID, int subIndex)
        {
            _dataPointer = reinterpret_cast<T*>(lynxHandler->getDataPointer(lynxID, subIndex));
        }

    private:
        T * _dataPointer;
    };
}


#endif // LYNX_STRUCTURE
