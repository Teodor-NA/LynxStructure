#include "lynxiodevice.h"
namespace LynxLib
{
    LynxIoDevice::LynxIoDevice(LynxManager & lynx) :
        _state(eFindHeader),
        _open(false),
        _lynx(&lynx)
	{
	}


    LynxIoDevice::~LynxIoDevice()
	{
	}


    const LynxInfo & LynxIoDevice::update()
    {
        _updateInfo.state = eNoChange;

		if (_state == eFindHeader)
		{
            // _lastID = LynxID();

            while (this->bytesAvailable() && (_state == eFindHeader))
			{
				_readBuffer.clear();
                this->read();
				if (_readBuffer.at(0) == LYNX_STATIC_HEADER)
                    _state = eFindStructId;
			}
		}

        if(_state == eFindStructId)
        {
            if(this->bytesAvailable())
            {
                this->read();

                _updateInfo.lynxId.structIndex = _lynx->findId(_readBuffer.at(1));

                if (_updateInfo.lynxId.structIndex < 0)
                {
					_state = eFindHeader;
					_updateInfo.state = eStructIdNotFound;
					return _updateInfo;
                }
                else
                {
					_state = eGetInfo;
                }
            }
        }

        if (_state == eGetInfo)
		{
            if (this->bytesAvailable() >= 3)
			{
                this->read();
                _updateInfo.lynxId.variableIndex = int(_readBuffer.at(2)) - 1;
                if(_updateInfo.lynxId.variableIndex > _lynx->structVariableCount(_updateInfo.lynxId.structIndex))
                {
                    _state = eFindHeader;
                    _updateInfo.state = eVariableIndexOutOfBounds;
                    return _updateInfo;
                }

                this->read();
                _updateInfo.dataLength = int(_readBuffer.at(3));
                if(_updateInfo.dataLength != _lynx->transferSize(_updateInfo.lynxId))
                {
                    _state = eFindHeader;
                    _updateInfo.state = eWrongDataLength;
                    return _updateInfo;
                }

                this->read();
                _updateInfo.deviceId = _readBuffer.at(4);

				_transferLength = _updateInfo.dataLength + LYNX_CHECKSUM_BYTES;

                _state = eGetData;
			}
		}

        if (_state == eGetData)
		{
			if (this->bytesAvailable() >= _transferLength)
			{
				_readBuffer.resize(_readBuffer.count() + _transferLength);

                read(_transferLength);

                _lynx->fromArray(_readBuffer, _updateInfo);

				_state = eFindHeader;
			}
		}

		return _updateInfo;
	}

    E_LynxState LynxIoDevice::send(const LynxId & lynxId)
	{
        E_LynxState state = _lynx->toArray(_writeBuffer, lynxId);
	
		if (state != eDataCopiedToBuffer)
			return state;

        this->write();

		return state;
	}
}
