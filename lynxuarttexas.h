/*
 * lynxuarttexas.h
 *
 *  Created on: 5. nov. 2019
 *      Author: CNC
 */

#ifndef LYNXUARTTEXAS_H_
#define LYNXUARTTEXAS_H_
#define BUFFER_SIZE 200
#include "DSP28x_Project.h"
#include "common/include/clk.h"
#include "common/include/sci.h"
#include "lynxiodevice.h"

class LynxUartTexas: public LynxIoDevice
{
public:
    LynxUartTexas(LynxManager * const lynx = LYNX_NULL);
    ~LynxUartTexas();

    bool open(int port, int baudRate,SCI_Handle _sciHandle,CLK_Handle _clkHandle);
    void close();
    void setMicros(long microsecs) { _micros = microsecs; }
    LynxRingBuffer rxBuffer;
    LynxRingBuffer txBuffer;

private:
    long _micros;
    /// Must read count number of bytes from the port to the read-buffer
    int read(int count = 1);
    /// Must write the write-buffer to the port
    void write();
    /// Must return number of bytes waiting at port
    int bytesAvailable() const;
    // Must return a relative timestamp in milliseconds
    uint32_t getMillis() const {return uint32_t(_micros/1000); };

    int _port;
    SCI_Handle sciHandle;
    CLK_Handle clkHandle;
};

#endif /* LYNXUARTTEXAS_H_ */
