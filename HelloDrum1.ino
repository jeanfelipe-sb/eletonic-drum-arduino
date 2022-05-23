/*
  EXAMPLE - Simple Piezo Sensing
  With this sample code, you will make snare and hiHat using two piezo.
  https://github.com/RyoKosaka/HelloDrum-arduino-Library
*/

///////////////////////////////   SETTING VALUE   ///////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

//byte SNARE[6] = {    
//    85, //sensitivity (1-100)
//    40,  //threshold (1-100)
//    1,  //scan time (1-)
//    50,  //mask time (1-)
//    38,  //note (0-127)
//    1    //curve type (0-4)
//};
byte SNARE[6] = {
    90, //sensitivity (1-100)
    30,  //threshold (1-100)
    1,  //scan time (1-)
    10,  //mask time (1-)
    38,  //note (0-127)
    1    //curve type (0-4)
};

byte HIHAT[7] = {
    100, //sensitivity
    10,  //threshold
    8,  //scan time
    30,  //mask time
    0,   //curve type
    26,  //46,  //note of open
    42   //note of close
};

byte HIHAT_PEDAL[7] = {
    80, //90 sensitivity (Hold the pedal at the position where you want to send 127(full open) in the MIDI CC, and read the Analog Read value in the TCRT5000. Divide the value by 10 and enter the value.)
    30, //threshold (Hold the pedal at the position where you want to send 0(close) in the MIDI CC, and read the Analog Read value in the TCRT5000. Divide the value by 10 and enter the value.)
    70, //scan start (When the analogRead value reaches this value * 10, the scan will start and the time will be recorded.)
    90, //scan end (When the analogRead value reaches this value * 10, the scan is finished and the time taken for the scan is referenced to determine the velocity.Then noteOn will be sent.The default value is 90, the same as Sensitivity. This means that the pedal is closed at the point where MIDI CC is 127.)
    10, //pedal sensitivity (This value multiplied by 100 is the time used to determine the velocity of the pedal. If the time taken from the scan start position to the scan end position is more than 1000 milliseconds, the velocity will be set to 1. So, if you feel that the velocity is low, either increase pedalSensitivity or increase the value of scan start.)
    0,  //curve type
    21  //44  //note of pedal
};

byte KICK[6] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    10,  //scan time (1-)
    30,  //mask time (1-)
    36,  //note (0-127)
    1    //curve type (0-4)
};

byte TOM1[6] = {
    90, //sensitivity (1-100)
    10,  //threshold (1-100)
    10,  //scan time (1-)
    30,  //mask time (1-)
    47,  //note (0-127)
    1    //curve type (0-4)
};

byte TOM2[6] = {
    70, //sensitivity (1-100)
    20,  //threshold (1-100)
    5,  //scan time (1-)
    25,  //mask time (1-)
    43,  //note (0-127)
    1    //curve type (0-4)
};

byte CRASH[6] = {
    92, //sensitivity (1-100)
    10,  //threshold (1-100)
    10,  //scan time (1-)
    30,  //mask time (1-)
    49,  //note (0-127)
    1    //curve type (0-4)
};

byte RIDE[6] = {
    80, //sensitivity (1-100)
    10,  //threshold (1-100)
    5,  //scan time (1-)
    10,  //mask time (1-)
    52,//51,  //note (0-127)
    1    //curve type (0-4)
};

/////////////////////////////////////////////////////////////////////////////////////

#include <hellodrum.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//unsigned char PadNote[6] = {42,39,47,49,38,36};         // Notas MIDI - 0 a 127
//Nota 39 - Clamps (Number 1 hits)
//Nota 47 - Tom Long (Number 1 hits)
//Nota 42 - Chimbal fechado
//Nota 49 - Prato Ataque
//Nota 38 - Caixa
//Nota 36 - Bumbo 
//Please name your piezo.
//The piezo named snare is connected to the A0 pin and the piezo named hiHat is connected to the A1 pin.
HelloDrum tom2(0);
HelloDrum hihat(1);
HelloDrum tom1(2);
HelloDrum crash(3);
HelloDrum snare(4);
HelloDrum kick(5);
HelloDrum ride(6);
HelloDrum hihatPedal(7);

void setup()
{
  MIDI.begin();
  Serial.begin(115200);//38400

  //Set Curve Type
  snare.setCurve(SNARE[5]);
  hihat.setCurve(HIHAT[4]);
  hihatPedal.setCurve(HIHAT_PEDAL[5]);
  kick.setCurve(KICK[5]);
  tom1.setCurve(TOM1[5]);
  tom2.setCurve(TOM2[5]);
  crash.setCurve(CRASH[5]);
  ride.setCurve(RIDE[5]);
}

void loop()
{
  //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
  //For each piezo, one line is required.
  snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);
  kick.singlePiezo(KICK[0], KICK[1], KICK[2], KICK[3]);
  tom1.singlePiezo(TOM1[0], TOM1[1], TOM1[2], TOM1[3]);
  tom2.singlePiezo(TOM2[0], TOM2[1], TOM2[2], TOM2[3]);
  crash.singlePiezo(CRASH[0], CRASH[1], CRASH[2], CRASH[3]);
  ride.singlePiezo(RIDE[0], RIDE[1], RIDE[2], RIDE[3]);
  hihat.HH(HIHAT[0], HIHAT[1], HIHAT[2], HIHAT[3]);
  hihatPedal.hihatControl(HIHAT_PEDAL[0], HIHAT_PEDAL[1], HIHAT_PEDAL[2], HIHAT_PEDAL[3], HIHAT_PEDAL[4]); //hihatControl(byte sens, byte thre, byte scanStart, byte scanEnd, byte pedalSens);

  //MIDI signals are transmitted with this IF statement.
  if (hihat.hit == true)
  {
    //check open or close
    //1.open
    if (hihatPedal.openHH == true)
    {
      MIDI.sendNoteOn(HIHAT[5], hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(HIHAT[5], 0, 10);
    }
    //2.close
    else if (hihatPedal.closeHH == true)
    {
      MIDI.sendNoteOn(HIHAT[6], hihat.velocity, 10); //(note, velocity, channel)
      MIDI.sendNoteOff(HIHAT[6], 0, 10);
    }
  }

  //when pedal is closed
  if (hihatPedal.hit == true)
  {
    MIDI.sendNoteOn(HIHAT_PEDAL[6], hihatPedal.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(HIHAT_PEDAL[6], 0, 10);
  }

  //sending state of pedal with controll change
  if (hihatPedal.moving == true)
  {
    MIDI.sendControlChange(4, hihatPedal.pedalCC, 10);
  }  

  //MIDI signals are transmitted with this IF statement.
  //For each piezo, one IF statement is required
  if (snare.hit == true)
  {
    MIDI.sendNoteOn(SNARE[4], snare.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(SNARE[4], 0, 10);
  }

  if (kick.hit == true)
  {
    MIDI.sendNoteOn(KICK[4], kick.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(KICK[4], 0, 10);
  }

  if (tom1.hit == true)
  {
    MIDI.sendNoteOn(TOM1[4], tom1.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(TOM1[4], 0, 10);
  }

  if (tom2.hit == true)
  {
    MIDI.sendNoteOn(TOM2[4], tom2.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(TOM2[4], 0, 10);
  }

  if (crash.hit == true)
  {
    MIDI.sendNoteOn(CRASH[4], crash.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(CRASH[4], 0, 10);
  }

  if (ride.hit == true)
  {
    MIDI.sendNoteOn(RIDE[4], ride.velocity, 10); //(note, velocity, channel)
    MIDI.sendNoteOff(RIDE[4], 0, 10);
  }
}
