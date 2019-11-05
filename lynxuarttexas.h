/*
 * lynxuarttexas.h
 *
 *  Created on: 5. nov. 2019
 *      Author: CNC
 */

#ifndef LYNXUARTTEXAS_H_
#define LYNXUARTTEXAS_H_
#define BUFFER_SIZE 50

#include "lynxiodevice.h"

class LynxUartTexas: public LynxIoDevice
{
public:
    LynxUartTexas(LynxManager * const lynx = LYNX_NULL);
    ~LynxUartTexas();

    bool open(int port, unsigned long baudRate);
    void close();

    LynxRingBuffer rxBuffer;

private:
    /// Must read count number of bytes from the port to the read-buffer
    int read(int count = 1);
    /// Must write the write-buffer to the port
    void write();
    /// Must return number of bytes waiting at port
    int bytesAvailable() const;
    // Must return a relative timestamp in milliseconds
    uint32_t getMillis() const;
};

#endif /* LYNXUARTTEXAS_H_ */
