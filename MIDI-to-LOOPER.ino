/*#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include <frequencyToNote.h>*/       
#include <MIDIUSB_Defs.h>
#include <MIDIUSB.h>

#define NUMBER_OF_LEDS 3
#define NUMBER_OF_BUTTONS 1

#define NUMBER_OF_RGBLEDS 3

#define RED_LED 4
#define GREEN_LED 3
#define BLUE_LED 2



boolean debugMode = false;


int ledOutputs[ NUMBER_OF_LEDS ] = { 2, 3, 4 };

// Declarations regarding buttons
int digitalButtons[ NUMBER_OF_BUTTONS ] =                { A5 };
boolean digitalButtonsIsDepressed[1] =                                                                                                                                                         { false };
byte noteOnForButtons[ NUMBER_OF_BUTTONS ] =           { 48 };


// Declarations for the LEDs
int rgbLEDs[ NUMBER_OF_RGBLEDS ][ 3 ] = { { 4, 3, 2 } };

byte midiChannel = 15;                             

unsigned long lastAction = 0;
int holdFor = 50; // ms from one command to the next

/* Useless code?
int lastMidiValue = 0;
int midiBytes[3] = { 0, -1, -1 };
*/

void setup() {
  
  if ( debugMode ) {
    Serial.begin( 9600 );
  } else {
    Serial.begin( 115200 );
  }

  // Initalize the pins for the buttons defined in digitalButtons[]
  for ( int i = 0; i < NUMBER_OF_BUTTONS; i++ ) {
    pinMode( digitalButtons[ i ], INPUT_PULLUP );
  }

  // Initialize the pins for the LEDs defined in rgbLEDs[]
  for ( int i = 1; i < NUMBER_OF_RGBLEDS; i++ ) {
    // RED
    pinMode( rgbLEDs[ i ][ 0 ], OUTPUT );
    digitalWrite( rgbLEDs[ i ][ 0 ], LOW );

    // GREEN
    pinMode( rgbLEDs[ i ][ 1 ], OUTPUT );
    digitalWrite( rgbLEDs[ i ][ 1 ], LOW );

    // BLUE
    pinMode( rgbLEDs[ i ][ 2 ], OUTPUT );
    digitalWrite( rgbLEDs[ i ][ 2 ], LOW );
  }
}

void loop() {
  
  if ( ( millis() - holdFor ) > lastAction ) { // If lastAction hasn't changed for x ms
    lastAction = 0; // Reset it
  }
  
  checkButtons();
  
  listenForMIDI();
  
}


/**
 * This functions loops over the arrays containing the
 * inputs (digital and analog) used for buttons to see
 * what or which buttons are pressed.
 *
 * Any pressed button will get a TRUE value in its corresponding
 * place in the digitalButtonsIsDepressed array. Buttons not
 * being pressed will get a false.
 **/
void checkButtons() {
  int currentMidiChannel = 1;
  for ( int i = 0; i < NUMBER_OF_BUTTONS; i++ ) {
    
      
    // Digital
    if ( digitalRead( digitalButtons[ i ] ) == LOW ) {
      if ( digitalButtonsIsDepressed[ i ] == false ) { // If the button isn't already registrered as pressed

        if ( lastAction == 0 ) { // This means nothing has happened for a while, so go ahead
        
          lastAction = millis();
        
          digitalButtonsIsDepressed[ i ] = true;
          
          if ( debugMode ) {
            Serial.print( i );
            Serial.println( " is currently being pressed.");
          } else {
            sendMidi( i, true, midiChannel );
          }
        }
      }
    } else {
      
      if ( digitalButtonsIsDepressed[ i ] ) { // The button was just released
      
        if ( lastAction == 0 ) { // This means nothing has happened for a while, so go ahead
        
          lastAction = millis();
      
          digitalButtonsIsDepressed[ i ] = false;
        
          if ( debugMode ) {
            Serial.print( i );
            Serial.println( " was just released.");
          } else {
            sendMidi( i, false, midiChannel );
          }
          
        }
      }
    }
  }
}

void sendMidi( int buttonNumber, boolean isOn, int channel ) {
  midiEventPacket_t midiNote;
  
  if ( isOn ) {
    midiNote = {0x09, 0x90 | channel, noteOnForButtons[ buttonNumber ], 100};
  } else {
    midiNote = {0x08, 0x80 | channel, noteOnForButtons[ buttonNumber ], 0};
  }

  MidiUSB.sendMIDI( midiNote );
  MidiUSB.flush();
}

/* Old, unused code?
int getLedIndex( byte val ) {
  switch ( val ) {
    case 25:
      return 1;
      break;
    case 26:
      return 2;
      break;
    case 27:
      return 3;
      break;
    case 28:
      return 4;
      break;
    case 29:
      return 5;
      break;
    case 30:
      return 6;
      break;
    default:
      return 0;
  }
}
*/

void listenForMIDI() {
  
  midiEventPacket_t rx = MidiUSB.read();

  switch (rx.header) {
    case 0:
      break;
    case 11: // CC
      parseMIDIToLED( rx.byte2, rx.byte3 );

      break;

    default:

      if ( debugMode ) {
        Serial.print("Unhandled MIDI message: ");
        Serial.print(rx.header);
        Serial.print("-");
        Serial.print(rx.byte1);
        Serial.print("-");
        Serial.print(rx.byte2); // CC number
        Serial.print("-");
        Serial.println(rx.byte3); // CC value
      }
      
      break;
      
   }
}

void parseMIDIToLED( byte CCNumber, byte CCValue ) {
  for ( int i = 1; i < NUMBER_OF_RGBLEDS; i++ ) {
    if ( CCNumber == 19 + i ) {
      resetRGBForLED( i - 1 );
      turnOnColorByCCValue( i - 1, CCValue );
    }

  }
  
  /*switch( CCNumber ) {
    case 20: // STOP
      //changeLEDs( 0, 0, 0 );

      resetRGBForLED( 0 );
      turnOnColorByCCValue( 0, CCValue );
      
      
      break;
    */
    /*case 21: // REC
      changeLEDs( 1, 0, 0 );
      break;
    case 22: // PLAY
      changeLEDs( 0, 1, 0 );
      break;
    case 23: // OVERDUB
      changeLEDs( 1, 1, 0 );
      break;
   }*/
}

void turnOnColorByCCValue( int led, byte CCValue ) {
  switch( CCValue ) {
    case 0: // STOP
      break;
    case 1: // REC
      digitalWrite( rgbLEDs[ led ][ 0 ], HIGH ); // r
      digitalWrite( rgbLEDs[ led ][ 1 ], LOW ); // g
      digitalWrite( rgbLEDs[ led ][ 2 ], LOW ); // b
      break;
    case 2: // PLAY
      digitalWrite( rgbLEDs[ led ][ 0 ], LOW ); // r
      digitalWrite( rgbLEDs[ led ][ 1 ], HIGH ); // g
      digitalWrite( rgbLEDs[ led ][ 2 ], LOW ); // b
      break;
    case 3: // OVERDUB
      digitalWrite( rgbLEDs[ led ][ 0 ], HIGH ); // r
      digitalWrite( rgbLEDs[ led ][ 1 ], HIGH ); // g
      digitalWrite( rgbLEDs[ led ][ 2 ], LOW ); // b
      break;
    }
}

void resetRGBForLED( int led ) {
  // RED
  digitalWrite( rgbLEDs[ led ][ 0 ], LOW );

  // GREEN
  digitalWrite( rgbLEDs[ led ][ 1 ], LOW );

  // BLUE
  digitalWrite( rgbLEDs[ led ][ 2 ], LOW );
}

void changeLEDs( bool red, bool green, bool blue ) {
  digitalWrite( RED_LED, red);
  digitalWrite( GREEN_LED, green );
  digitalWrite( BLUE_LED, blue );
}
