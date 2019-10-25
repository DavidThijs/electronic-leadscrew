// Clough42 Electronic Leadscrew
// https://github.com/clough42/electronic-leadscrew
//
// MIT License
//
// Copyright (c) 2019 James Clough
//
// Modified by Denis Tikhonov to use an additional MPG input and a left/right lever
// for lathes that have no half-nuts and the nut is directly coupled to the apron.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef __MPG_H
#define __MPG_H

#include "F28x_Project.h"
#include "Configuration.h"




#define LEVER_LEFT_PIN GPIO12
#define LEVER_RIGHT_PIN GPIO34
//#define LEVER_LEFT_PIN GPIO12
//#define LEVER_LEFT_PIN GPIO12

#define GPIO_SET(pin) GpioDataRegs.GPASET.bit.pin = 1
#define GPIO_CLEAR(pin) GpioDataRegs.GPACLEAR.bit.pin = 1
#define GPIO_GET_A(pin) GpioDataRegs.GPADAT.bit.pin
#define GPIO_GET_B(pin) GpioDataRegs.GPBDAT.bit.pin


class MPG
{
private:
    Uint32 previous;
    Uint16 rpm;

public:
    MPG( void );
    void initHardware( void );

    bool isLeverLeft();
    bool isLeverRight();

    Uint16 getRPM( void );
    Uint32 getPosition( void );
    Uint32 getMaxCount( void );
};


inline Uint32 MPG :: getPosition(void)
{
    return EQep2Regs.QPOSCNT;
}

inline Uint32 MPG :: getMaxCount(void)
{
    return _MPG_MAX_COUNT;
}

inline bool MPG :: isLeverLeft()
{
    return GPIO_GET_A(LEVER_LEFT_PIN);
}
inline bool MPG :: isLeverRight()
{
    return GPIO_GET_B(LEVER_RIGHT_PIN);
}
#endif // __MPG_H
