#include <Arduino.h>
#include <XPLPro.h>

// The XPLDirect library is automatically installed by PlatformIO with XPLDevices
// Optional defines for XPLDirect can be set in platformio.ini
// This sample contains all the important defines. Modify or remove as needed

// This sample shows how to use 74HC4067 Multiplexers for the inputs as commonly used by SimVim

// A simple Pushbutton on MUX0 pin 0
Button btnStart(0, 0);

// An Encoder with push functionality. MUX1 pin 8&9 are the encoder pins, 10 the push pin.
// configured for an Encoder with 4 counts per mechanical notch, which is the standard
Encoder encHeading(1, 8, 9, 10, enc4Pulse);

// A simple On/Off switch on MUX0, pin 15
Switch swStrobe(0, 15);

// A handle for a DataRef
int drefStrobe;

void xpInit()
{
  // Register Command for the Button
  btnStart.setCommand(F("sim/starters/engage_starter_1"));

  // Register Commands for Encoder Up/Down/Push function.
  encHeading.setCommand(F("sim/autopilot/heading_up"),
                        F("sim/autopilot/heading_down"),
                        F("sim/autopilot/heading_sync"));

  // Register a DataRef for the strobe light. Subscribe to updates from XP, 100ms minimum Cycle time, no divider
  drefStrobe = XP.registerDataRef(F("sim/cockpit/electrical/strobe_lights_on"));
  XP.requestUpdates(drefStrobe, 100, 0);
}

void xpStop()
{
  // nothing to do on unload
}

void xpUpdate(int handle)
{
  if (handle == drefStrobe)
  { // Show the status of the Strobe on the internal LED
    digitalWrite(LED_BUILTIN, (XP.datarefReadInt() > 0));
  }
}

// Arduino setup function, called once
void setup() { 
  // setup interface
  Serial.begin(XPLDIRECT_BAUDRATE);
  XP.begin("Sample", &xpInit(), &xpStop(), &xpUpdate());

  // Connect MUX adress pins to Pin 22-25 (SimVim Pins)
  DigitalIn.setMux(22, 23, 24, 25);
  // Logical MUX0 on Pin 38
  DigitalIn.addMux(38);
  // Logical MUX1 on Pin 39
  DigitalIn.addMux(39);
}

// Arduino loop function, called cyclic
void loop() {
  // Handle XPlane interface
  XP.xloop();

  // handle all devices and automatically process commandsin background
  btnStart.handleXP();
  encHeading.handleXP();
  swStrobe.handleXP();
}