#ifndef LYNX_IO_DEVICE
#define LYNX_IO_DEVICE

#include "lynxstructure.h"

namespace LynxLib
{ 
	enum E_SerialState
	{
		eFindHeader = 0,
        eFindStructId,
        eGetInfo,
        eGetData
	};

	class LynxIoDevice
	{
	public:
		LynxIoDevice(LynxManager & lynx);
        virtual ~LynxIoDevice();

		const LynxInfo & update();
        E_LynxState send(const LynxId & lynxId);
        bool opened() { return _open; }

        virtual bool open(int port, unsigned long baudRate) = 0;
        virtual void close() = 0;

        const LynxByteArray & readBuffer() const { return _readBuffer; }
        const LynxByteArray & writeBuffer() const { return _writeBuffer; }

	protected:
		E_SerialState _state;
		LynxInfo _updateInfo;
		int _transferLength;
        bool _open;

		LynxManager * const _lynx;

        virtual int read(int count = 1) = 0;
        virtual void write() = 0;
        virtual int bytesAvailable() const = 0;

		LynxByteArray _readBuffer;
		LynxByteArray _writeBuffer;
    };
}
#endif // !LYNX_IO_DEVICE
