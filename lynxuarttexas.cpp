/*
 * lynxuarttexas.cpp
 *
 *  Created on: 5. nov. 2019
 *      Author: CNC
 */

#include "lynxuarttexas.h"

LynxUartTexas::LynxUartTexas(LynxManager * const lynx) : LynxIoDevice(lynx),rxBuffer(BUFFER_SIZE,LynxLib::eFixedOverwrite),txBuffer(BUFFER_SIZE,LynxLib::eFixedOverwrite)
{
    // TODO Auto-generated constructor stub


}
bool LynxUartTexas::open(int port,int baudrate,SCI_Handle _sciHandle,CLK_Handle _clkHandle)
{
    if (_open)
        this->close();_port = port;

    _port=0;
    //Map the handles
    this->sciHandle = _sciHandle;
    this->clkHandle=_clkHandle;
    // enable the SCI-A clock
    CLK_enableSciaClock(clkHandle);
    SCI_disableParity(sciHandle);
    SCI_setNumStopBits(sciHandle, SCI_NumStopBits_One);
    SCI_setCharLength(sciHandle, SCI_CharLength_8_Bits);

    // enable TX, RX, internal SCICLK,
    // Disable RX ERR, SLEEP, TXWAKE
    SCI_enableTx(sciHandle);
    SCI_enableRx(sciHandle);
    SCI_enableTxInt(sciHandle);
    SCI_enableRxInt(sciHandle);

    //SCI_enableLoopBack(sciHandle);

    // SCI BRR = LSPCLK/(SCI BAUDx8) - 1
    SCI_setBaudRate(sciHandle, SCI_BaudRate_e(baudrate));

    SCI_enable(sciHandle);

    SCI_enableFifoEnh(sciHandle);
    SCI_resetTxFifo(sciHandle);
    SCI_clearTxFifoInt(sciHandle);
    SCI_resetChannels(sciHandle);
    SCI_setTxFifoIntLevel(sciHandle, SCI_FifoLevel_4_Words);
    SCI_enableTxFifoInt(sciHandle);

    SCI_resetRxFifo(sciHandle);
    SCI_clearRxFifoInt(sciHandle);
    SCI_setRxFifoIntLevel(sciHandle, SCI_FifoLevel_4_Words);
    SCI_enableRxFifoInt(sciHandle);

    return _open=true;//success
}
void LynxUartTexas::close()
{
    if (!_open)
        return;
    _open = false;
}
LynxUartTexas::~LynxUartTexas()
{
    // TODO Auto-generated destructor stub
}

/// Must read count number of bytes from the port to the read-buffer
int LynxUartTexas::read(int count)
{
    if (!_open)
            return 0;

    //int temp;

    rxBuffer.read(_readBuffer,count);
//    for (int i = 0; i < count; i++)
//    {
//        temp = rxBuffer.read();
//        if (temp < 0 || rxBuffer.count()==0)
//            return i;
//
//        _readBuffer.append(char(temp));
//    }

    return count;

}
/// Must write the write-buffer to the port
void LynxUartTexas::write()
{
    if (!_open)
            return;
//    for (int i = 0; i < _writeBuffer.count(); ++i)
//    {
        txBuffer.write(_writeBuffer);
        //txBuffer.write(char(reinterpret_cast<const uint8_t*>(_writeBuffer.data())));
        //txBuffer.write(_writeBuffer.data());
//    }


}
/// Must return number of bytes waiting at port
int LynxUartTexas::bytesAvailable() const
{
    return rxBuffer.count();
}

