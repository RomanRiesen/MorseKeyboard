/* Morse Code Key
by: Roman Riesen
date: 15/7/2018
license: MIT License - Feel free to use this code for any purpose.
No restrictions. Just keep this license if you go on to use this
code in your future endeavors! Reuse and share.

This code lets you use your pro micro (or similar) with a button as a morse-keyboard.
I believe it is newb friendly, as it displays the current char when morsing and sends a backspace when the next symbol is registred.
(Resulting in a sort of preview).
To delete something, just hammer on your switch enough times.
It is very restrictive in it's current state though, as it does not have any special characters and not even lower case letters.
*/
#include <Keyboard.h>

#define HEAP_SIZE 32//The number of heap leaves.
#define UPPER_LETTER_TIME 600 //How long to wait till the current character is set as final
#define MAX_POINT_TIME 170 //the duration of a dot
#define MAX_DASH_TIME 800 //The duration of a dash
#define DEBOUNCE_TIME 10 //Exactly what it sounds like

int buttonPin = 9;  // Set a button to any pin
bool wasDown = false;
unsigned long pos = 1; //Holds the curent position of the morseHeap
unsigned int downT, upT;
unsigned long lastUpT, lastDownT;

//+1 due to the 1-indexing.
char const * morseHeap[HEAP_SIZE+1] = {'ONE INDEXED!', 'start', 'E', 'T', 'I', 'A', 'N', 'M', 'S', 'U', 'R', 'W', 'D', 'K', 'G', 'O', 'H', 'V', 'F', '\0', 'L', '\0', 'P', 'J', 'B', 'X', 'C', 'Y', 'Q', 'Z', '\0', ' '};

void setup()
{
  pinMode(buttonPin, INPUT);  // Set the button as an input
  digitalWrite(buttonPin, HIGH);  // Pull the button high
}

void loop()
{//TODO make some sort of "pro-mode" that does not update the letter as one types the morse-char and just displays it after 600ms or similar.
  if ( millis() - lastUpT > UPPER_LETTER_TIME ) {pos = 1; lastUpT = millis();}

  if ( !wasDown && !digitalRead(buttonPin) ) {//Button is pressed now
    wasDown = true;
    //upT = millis() - lastUpT;
    lastDownT = millis();
  }

  if ( wasDown && digitalRead(buttonPin) ){//Switch gets released
    downT = millis() - lastDownT;
    if( DEBOUNCE_TIME < downT ) {

        if(downT < MAX_POINT_TIME )
            pos *= 2;// .
        else if ( downT < MAX_DASH_TIME )
            pos = pos*2+1;// _

        if ( 3 < pos ) {//if there is already a char, delte it or if the pos it outside fo the heap range, send backspace.
            if ( HEAP_SIZE+1 < pos )
                pos = HEAP_SIZE+2;
            Keyboard.write('\b');
        }

        if ( pos < HEAP_SIZE )
            Keyboard.write( morseHeap[pos] );

        lastUpT = millis();
        wasDown = false;
        }
    }
}
