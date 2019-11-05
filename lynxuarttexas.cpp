/*
 * lynxuarttexas.cpp
 *
 *  Created on: 5. nov. 2019
 *      Author: CNC
 */

#include "lynxuarttexas.h"

LynxUartTexas::LynxUartTexas(LynxManager * const lynx) : LynxIoDevice(lynx),rxBuffer(BUFFER_SIZE,LynxLib::eFixedOverwrite)
{
    // TODO Auto-generated constructor stub


}

LynxUartTexas::~LynxUartTexas()
{
    // TODO Auto-generated destructor stub
}

/// Must read count number of bytes from the port to the read-buffer
int LynxUartTexas::read(int count)
{
    return 0;

}
/// Must write the write-buffer to the port
void LynxUartTexas::write()
{

}
/// Must return number of bytes waiting at port
int LynxUartTexas::bytesAvailable() const
{
    return 0;
}
// Must return a relative timestamp in milliseconds
uint32_t LynxUartTexas::getMillis() const
{
    return 0;
}
