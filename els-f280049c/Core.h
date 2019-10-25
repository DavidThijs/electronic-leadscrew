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


#ifndef __CORE_H
#define __CORE_H

#include "StepperDrive.h"
#include "Encoder.h"
#include "MPG.h"
#include "ControlPanel.h"
#include "Tables.h"

class Core
{
private:
    MPG *mpg;
    Encoder *encoder;
    StepperDrive *stepperDrive;

    float feed;
    float previousFeed;

    int16 feedDirection;
    int16 previousFeedDirection;
    
    bool MPG_ACTIVE;
    bool THREADING_ACTIVE;

    Uint32 previousSpindlePosition;
    int32 displayValue;
    Uint32 previousMPGPosition;

    int32 feedRatio(Uint32 count);

public:
    Core( Encoder *encoder, StepperDrive *stepperDrive );

    void setFeed(const FEED_THREAD *feed);
    void setReverse(bool reverse);
    void setThreading(bool threading);
    void setMPGActive(bool mpg_selected);
    Uint16 getRPM(void);
    bool isAlarm();

    void ISR( void );
};

inline void Core :: setFeed(const FEED_THREAD *feed)
{
    this->feed = (float)feed->numerator / feed->denominator;
}

inline Uint16 Core :: getRPM(void)
{
     //return displayValue; // For testing
    return encoder->getRPM();
}

inline bool Core :: isAlarm()
{
    return this->stepperDrive->isAlarm();
}

inline int32 Core :: feedRatio(Uint32 count)
{
    return ((float)count) * this->feed * feedDirection;
}

inline void Core :: ISR( void )
{
    if( this->feed != NULL ) {  // Feed also works in MPG mode so leave it alone

        // read the encoder
           int32 spindlePosition = encoder->getPosition(); // Overflows at/to 65535, max value of Uint16
           Uint32 MPGPosition = mpg->getPosition();
           int32 MPGDelta = 0;
           int32 desiredSteps = 0;

            desiredSteps = feedRatio(spindlePosition);
            displayValue = spindlePosition;


           if(THREADING_ACTIVE != true){   // If we are in threading mode skip lever and MPG calculation

                    if(mpg->isLeverLeft() != false && mpg->isLeverRight() != false) stepperDrive->setCurrentPosition(desiredSteps); // If lever is in center position stop movement based on spindle rotation

                     MPGDelta = MPGPosition - previousMPGPosition;

                     if( MPGPosition < previousMPGPosition && previousMPGPosition - MPGPosition > mpg->getMaxCount()/2) MPGDelta = MPGPosition + mpg->getMaxCount() - previousMPGPosition; // Small MPGPos + Max Count - Big PrevPos = distance from rollover point + distance to rollover point as a positive number
                     if( MPGPosition > previousMPGPosition && MPGPosition - previousMPGPosition > mpg->getMaxCount()/2) MPGDelta = MPGPosition - previousMPGPosition - mpg->getMaxCount(); // Same thing but negative

                     }// if THREADING_ACTIVE

          stepperDrive->incrementCurrentPosition(MPGDelta*MPG_FEED);// Works only because MPG_FEED = 1. Will break with values other than 1, 2, 3 etc.
         stepperDrive->setDesiredPosition(desiredSteps);

          // compensate for encoder overflow/underflow

        if( spindlePosition < previousSpindlePosition && previousSpindlePosition - spindlePosition > encoder->getMaxCount()/2 ) {
            stepperDrive->incrementCurrentPosition(-1 * feedRatio(encoder->getMaxCount()));
        }
        if( spindlePosition > previousSpindlePosition && spindlePosition - previousSpindlePosition > encoder->getMaxCount()/2 ) {
            stepperDrive->incrementCurrentPosition(feedRatio(encoder->getMaxCount()));
        }

        // if the feed or direction changed, reset sync to avoid a big step
        if( feed != previousFeed || feedDirection != previousFeedDirection){
            stepperDrive->setCurrentPosition(desiredSteps);
        }

        // remember values for next time
        previousSpindlePosition = spindlePosition;
        previousMPGPosition = MPGPosition;
        previousFeedDirection = feedDirection;
        previousFeed = feed;
        stepperDrive->ISR();
    }
}


#endif // __CORE_H
