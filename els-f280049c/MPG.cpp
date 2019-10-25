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


    /*
     * DAMN IT
     * HOW THE FUCK AM I SUPPOSED TO GUESS THE REGISTER VALUES?!!!!!
     * THIS IS NOWHERE ON THE INTERNET - FORUMS, TI SUPPORT ETC ALL TALK ABOUT ALL KINDS OF BULLSHIT BUT THIS
     * THIS IS NOWHERE IN THE OFFLINE DOCUMENTATION
     * SOME SHIT LIKE THIS APPEARED ONCE IN CONTROLSUITE SO I DOWNLOADED 2 GIGS INSTALLED AND WHAT? RIGHT, ITS FUCKING OBSOLETE AND THEY TELL YOU TO USE C2000WARE
     * SO I INSTALLED C2000WARE AND WHAT? GO FUCK YOURSELF IS THE ANSWER!!! HOW THE FUCK AM I SUPPOSED TO UNDERSTAND SHIT LIKE "0x00061E0AU"????
     *
     * HOW THE FUCK DOES A "10" TRANSLATE TO "2"???!!!
     * HOW THE FUCK DOES A "9" TRANSLATE TO "1"???!!!
     * HOW THE FUCK DOES A "11" TRANSLATE TO "3"???
     * I DONT FUCKING KNOW!!!
     * */


    /*
     * Just FYI, this is what's in the "C:\ti\c2000\C2000Ware_2_00_00_03\libraries\boot_rom\f28004x\rev0\rom_sources\F28004x_ROM\driverlib\pin_map.h file:
     * #define GPIO_14_EQEP2A         0x00061C0AU
     * #define GPIO_15_EQEP2B         0x00061E0AU
     * #define GPIO_26_EQEP2I         0x00081402U
     *
     * How the fuck do I understand this?
     *
     * And this is in the SPRS945D document from TI, page 39-41:
     *
     4.4 Pin Multiplexing
        4.4.1 GPIO Muxed Pins
    Table 4-6 lists the GPIO muxed pins. The default mode for each GPIO pin is the GPIO function, except
    GPIO35 and GPIO37, which default to TDI and TDO, respectively. Secondary functions can be selected
    by setting both the GPyGMUXn.GPIOz and GPyMUXn.GPIOz register bits. The GPyGMUXn register
    should be configured before the GPyMUXn to avoid transient pulses on GPIOs from alternate mux
    selections. Columns that are not shown and blank cells are reserved GPIO Mux settings.
     *
     *EQEP2_A on GPIO14: Mux = 10
     *EQEP2_B on GPIO15: Mux = 10
     *EQEP2_I on GPIO26: MUX = 2
     *
     * How does all this translate to GPAMUX and GPAGMUX values below?
     * No fucking idea but now it works.
     *
     */



#include "MPG.h"
#include "Configuration.h"


MPG :: MPG( void )
{
    this->previous = 0;
    this->rpm = 0;
}

void MPG :: initHardware(void)
{
    EALLOW;

        // Configure GPIO pins for physical switch inputs
        // GPIO31 = Left
        // GPIO34 = Right

        GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;   // Enable pullup on GPIO31
        GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;   // Enable pullup on GPIO34

        GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3; // asynch input
        GpioCtrlRegs.GPBQSEL1.bit.GPIO34 = 3; // asynch input

        GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0; // Set all button inputs to Multi-purpose GPIO
        GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;

        GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
        GpioCtrlRegs.GPBDIR.bit.GPIO34 = 0;

         GpioCtrlRegs.GPACTRL.bit.QUALPRD3=1;  // Qual period = SYSCLKOUT/2
         GpioCtrlRegs.GPAQSEL1.bit.GPIO12=2;   // 6 samples
         GpioCtrlRegs.GPBQSEL1.bit.GPIO34=2;   // 6 samples



    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;     // Enable pull-up on GPIO20 (EQEP2A)
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;     // Enable pull-up on GPIO21 (EQEP2B)

    // Dropped the "I" pin, I haven't figured out how to express "2" in the Mux and the MPG doesn't have that anyway

    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 0;   // Sync to SYSCLKOUT GPIO20 (EQEP2A)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 0;   // Sync to SYSCLKOUT GPIO21 (EQEP2B)

    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;    // Configure GPIO20 as EQEP1A
    GpioCtrlRegs.GPAGMUX1.bit.GPIO14 = 2;
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;    // Configure GPIO20 as EQEP1B
    GpioCtrlRegs.GPAGMUX1.bit.GPIO15 = 2;

    EDIS;

// Seems to work without the "I" stuff.
    // It even reads RPM without the "I"

    EQep2Regs.QDECCTL.bit.QSRC = 0;         // QEP quadrature count mode
  //  EQep2Regs.QDECCTL.bit.IGATE = 1;        // gate the index pin
    EQep2Regs.QDECCTL.bit.QAP = 1;          // invert A input
    EQep2Regs.QDECCTL.bit.QBP = 1;          // invert B input
   // EQep2Regs.QDECCTL.bit.QIP = 1;          // invert index input
    EQep2Regs.QEPCTL.bit.FREE_SOFT = 2;     // unaffected by emulation suspend
    EQep2Regs.QEPCTL.bit.PCRM = 1;          // position count reset on maximum position
    EQep2Regs.QPOSMAX = _MPG_MAX_COUNT;  // Max position count

    EQep2Regs.QUPRD = CPU_CLOCK_HZ / RPM_CALC_RATE_HZ; // Unit Timer latch at RPM_CALC_RATE_HZ Hz
    EQep2Regs.QEPCTL.bit.UTE=1;             // Unit Timeout Enable
    EQep2Regs.QEPCTL.bit.QCLM=1;            // Latch on unit time out

    EQep2Regs.QEPCTL.bit.QPEN=1;            // QEP enable

}
 // Don't need RPM for MPG
Uint16 MPG :: getRPM(void)
{
    if(EQep2Regs.QFLG.bit.UTO==1)       // If unit timeout (one 10Hz period)
    {
        Uint32 current = EQep2Regs.QPOSLAT;
        Uint32 count = (current > previous) ? current - previous : previous - current;

        // deal with over/underflow
        if( count > _MPG_MAX_COUNT/2 ) {
            count = _MPG_MAX_COUNT - count; // just subtract from max value
        }

        rpm = count * 60 * RPM_CALC_RATE_HZ / MPG_RESOLUTION;

        previous = current;
        EQep2Regs.QCLR.bit.UTO=1;       // Clear interrupt flag
    }

    return rpm;
}

