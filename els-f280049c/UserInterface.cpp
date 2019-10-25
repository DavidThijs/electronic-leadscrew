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




// Clough42 Electronic Leadscrew
// https://github.com/clough42/electronic-leadscrew
//
// MIT License
//
// Copyright (c) 2019 James Clough
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


#include "UserInterface.h"


const MESSAGE STARTUP_MESSAGE_2 =
{
  .message = { LETTER_E, LETTER_L, LETTER_S, DASH, TWO | POINT, ONE | POINT, THREE, BLANK },
  .displayTime = UI_REFRESH_RATE_HZ * 0.2
};
// Have to save space
/*
const MESSAGE STARTUP_MESSAGE_1 =
{
 .message = { LETTER_C, LETTER_L, LETTER_O, LETTER_U, LETTER_G, LETTER_H, FOUR, TWO },
 .displayTime = UI_REFRESH_RATE_HZ * 0.2,
 .next = &STARTUP_MESSAGE_2
};

const MESSAGE SETTINGS_MESSAGE_2 =
{
 .message = { LETTER_S, LETTER_E, LETTER_T, LETTER_T, LETTER_I, LETTER_N, LETTER_G, LETTER_S },
 .displayTime = UI_REFRESH_RATE_HZ * .2
};

const MESSAGE SETTINGS_MESSAGE_1 =
{
 .message = { BLANK, BLANK, BLANK, LETTER_N, LETTER_O, BLANK, BLANK, BLANK },
 .displayTime = UI_REFRESH_RATE_HZ * .3,
 .next = &SETTINGS_MESSAGE_2
};
*/
UserInterface :: UserInterface(ControlPanel *controlPanel, Core *core, FeedTableFactory *feedTableFactory, MPG *mpg)
{
    this->controlPanel = controlPanel;
    this->core = core;
    this->feedTableFactory = feedTableFactory;
    this->mpg = mpg;

    this->metric = true; // start out with imperial
    this->thread = false;
    this->mpg_selected = true; // start out with feeds

    this->reverse = false; // start out going forward
    this->mode_selector = 1;    // Mode Lever Feed at start
    this->leverLeft = false;
    this->previousLeverLeft = false;

    this->leverRight = false;
    this->previousLeverRight = false;

    this->feedTable = NULL;

    this->keys.all = 0xff;

    setMessage(&STARTUP_MESSAGE_2);
}

const FEED_THREAD *UserInterface::loadFeedTable()
{
    this->feedTable = this->feedTableFactory->getFeedTable(this->metric, this->thread); // ,false); //this->mpg_selected);
    return this->feedTable->current();
}

LED_REG UserInterface::calculateLEDs(const FEED_THREAD *selectedFeed)
{
    // get the LEDs for this feed
      LED_REG leds = selectedFeed->leds;

      // and add a few other
      leds.bit.REVERSE = this->reverse;
      leds.bit.FORWARD = ! this->reverse;

      return leds;
}

void UserInterface :: setMessage(const MESSAGE *message)
{
    this->message = message;
    this->messageTime = message->displayTime;
}

void UserInterface :: overrideMessage( void )
{
    if( this->message != NULL )
    {
        if( this->messageTime > 0 ) {
            this->messageTime--;
            controlPanel->setMessage(this->message->message);
        }
        else {
            this->message = this->message->next;
            if( this->message == NULL )
                controlPanel->setMessage(NULL);
            else
                this->messageTime = this->message->displayTime;
        }
    }
}

void UserInterface :: loop( void )
{
    const FEED_THREAD *newFeed = NULL;

    // display an override message, if there is one
    overrideMessage();

    // just in case, initialize the first time through
    if( feedTable == NULL ) {
        newFeed = loadFeedTable();
    }

    // read keypresses from the control panel
    keys = controlPanel->getKeys();

    // respond to keypresses
    if( keys.bit.IN_MM )
    {
        this->metric = ! this->metric;
        newFeed = loadFeedTable();
    }
    if( keys.bit.FEED_THREAD )
    {
      // Mode switch - may be used for more than two modes but I ended up only needing two
      mode_selector++;
      if(mode_selector == 3) mode_selector = 1;
      switch(mode_selector){
        case 1:{
            this->mpg_selected = true;
            this->thread = false;
            this->reverse = false; // Lever is used to determine direction, see further in this file
            break;
        }
        case 2:{
            this->mpg_selected = false;
            this->thread = true;
            break;
        }
      }
        newFeed = loadFeedTable();
    }
    if( keys.bit.FWD_REV )
    {
        if(this->thread == true){
                   this->reverse = ! this->reverse;
               } else this->reverse = false; // Button switches to Reverse only in threading mode.
        // When using lever, the lever determines the direction. However, the MPG direction is NOT affected by this setting.
        // feed table hasn't changed, but we need to trigger an update
        newFeed = loadFeedTable();
    }
    if( keys.bit.UP )
    {
        newFeed = feedTable->next();
    }
    if( keys.bit.DOWN )
    {
        newFeed = feedTable->previous();
    }
    /*
    if( keys.bit.SET )
    {
       // setMessage(&SETTINGS_MESSAGE_1);
    }
*/
    leverLeft = !mpg->isLeverLeft();
    leverRight = !mpg->isLeverRight();

    if(leverRight != previousLeverRight){
    reverse = leverRight;
    newFeed = loadFeedTable();
    } //  if leverRight != previousLeverRight
    if(leverLeft != previousLeverLeft)    newFeed = loadFeedTable(); // If lever has changed state update feed table
    previousLeverLeft = leverLeft;
    previousLeverRight = leverRight;

    // if we have changed the feed
    if( newFeed != NULL ) {
        // update the display
        LED_REG leds = this->calculateLEDs(newFeed);
        controlPanel->setLEDs(leds);
        controlPanel->setValue(newFeed->display);

        // update the core
                core->setMPGActive(this->mpg_selected);
                core->setThreading(this->thread);
                core->setFeed(newFeed);
                core->setReverse(this->reverse);
    }//  if newFeed != NULL

    // update the RPM display
   controlPanel->setRPM(core->getRPM());

    // write data out to the display
    controlPanel->refresh();
}


