#ifndef LYNX_IO_DEVICE_H
#define LYNX_IO_DEVICE_H

#include "lynxstructure.h"

namespace LynxLib
{
	enum E_SerialState
	{
		eFindHeader = 0,
		eFindStructId,
		eGetInfo,
		eInternals,
		eGetPeriodicStart,
		eGetPeriodicStop,
		eGetPullRequest,
		eGetScan,
        eGetDeviceId,
		eGetDeviceInfo,
		eGetDeviceData,
		eGetData
	};
}

struct LynxPeriodicTransmit : public LynxId
{
    LynxPeriodicTransmit() : LynxId(), timeInterval(0), previousTimeStamp(0) {}

	LynxPeriodicTransmit(const LynxId & lynxId, uint32_t _timeInterval, uint32_t _previousTimeStamp) : 
		LynxId(lynxId), 
		timeInterval(_timeInterval),
		previousTimeStamp(_previousTimeStamp)
    {}

	uint32_t timeInterval; /// Time in milliseconds
	uint32_t previousTimeStamp;
};

class LynxIoDevice
{
public:
    LynxIoDevice(LynxManager * const lynx = nullptr);
    virtual ~LynxIoDevice();

    void init(LynxManager * const lynx = nullptr) { _lynx = lynx; }

	/// Run this to receive data on the uart port.
	/// Should run as often as possible, or on interrupt when new data is waiting at the port.
	const LynxInfo & update();

	/// Must be run as often as possible, or on timer interrupts if possible.
	LynxLib::E_LynxState periodicUpdate();

    LynxLib::E_LynxState send(const LynxId & lynxId);
    bool isOpen() const { return _open; }

	int sendDeviceInfo();

//    virtual bool open(int port, unsigned long baudRate) = 0;
//    virtual void close() = 0;

	// Scan the bus for devices
	void scan();
	// Pull datagram from device
	void pullDatagram(const LynxId & lynxId);

    const LynxByteArray & readBuffer() const { return _readBuffer; }
    const LynxByteArray & writeBuffer() const { return _writeBuffer; }

	/// Interval in milliseconds
	void periodicStart(const LynxId & lynxId, uint32_t interval);
	void periodicStop(const LynxId & lynxId);

	void remotePeriodicStart(const LynxId & lynxId, uint32_t interval);
	void remotePeriodicStop(const LynxId & lynxId);

    void changeRemoteDeviceId(char deviceId);

	LynxDeviceInfo lynxDeviceInfo();

protected:
	LynxLib::E_SerialState _state;
	LynxInfo _updateInfo;
	int _transferLength;
    bool _open;

    LynxManager * _lynx;

	/// Must read count number of bytes from the port to the read-buffer
    virtual int read(int count = 1) = 0;
	/// Must write the write-buffer to the port
    virtual void write() = 0;
	/// Must return number of bytes waiting at port
    virtual int bytesAvailable() const = 0;
	/// Must return a relative timestamp in milliseconds
	virtual uint32_t getMillis() const = 0;

	void readDeviceInfo();

	LynxDeviceInfo _deviceInfo;

	LynxByteArray _readBuffer;
	LynxByteArray _writeBuffer;

	LynxList<LynxPeriodicTransmit> _periodicTransmits;
	uint32_t _currentTime;
};

#endif // !LYNX_IO_DEVICE_H
