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

#define HEAP_SIZE 64 //The number of heap leaves.
#define MAX_POINT_TIME 170 //the duration of a dot
#define MAX_DASH_TIME 3*MAX_POINT_TIME //The duration of a dash
#define LETTER_PAUSE MAX_DASH_TIME //How long to wait till the current character is set as final
#define WORD_PAUSE 7*MAX_POINT_TIME //After this time passed space is wirten
#define DEBOUNCE_TIME 10 //Exactly what it sounds like
#define BUTTON_PIN 9 // Set a button to any pin
#define LONG_PRESS_PIN 2 // Set a button to any pin
#define TIMED_SPACE_PIN 3 // Set a button to any pin

//You might want to make sure one of these is 1 as else you can only write a space with 4 dashes (which is not set to CH as that would mess with the deletion of the previously reached char in the input)
bool USE_LONG_PRESS_SPACE = 0; //Whether to let the user press longer than a dash to print space
bool USE_TIMED_SPACE = 1; //Wheter to print a space after WORD_PAUSE has passed with no input

bool wasDown = false;
bool timedSpaceWritten = true; //So I only send one space for each time there is no input for long enoug after an input.
unsigned short pos = 1; //Holds the curent position of the morseHeap
unsigned long downT, upT;
unsigned long lastUpT, lastDownT;


//+1 due to the 1-indexing. //Do not change ' ' as else you would have to do some rewriting concerning the USE_LONG_PRESS_SPACE stuff
char const * morseHeap[HEAP_SIZE+1] = {"ONE INDEXED!", " ", "E", "T", "I", "A", "N", "M", "S", "U", "R", "W", "D", "K", "G", "O", "H", "V", "F", "\"U", "L", "\"A", "P", "J", "B", "X", "C", "Y", "Z", "Q", "\"O", " ", "5", "4", "\0", "3", ".", ",", "\0", "2", "\0" , "\0", "+", "-", "\0", "\0", "\0", "1", "6", "=", "/", "*", "\0", "\r", "\0", "\0", "7", "\0", "\0", "\0", "8", "\0", "9", "0"};


void setup()
{
  pinMode(BUTTON_PIN, INPUT); // Set the button as an input
  digitalWrite(BUTTON_PIN, HIGH); // Pull the button high
  pinMode(LONG_PRESS_PIN, INPUT); // Set the button as an input
  digitalWrite(LONG_PRESS_PIN, HIGH); // Pull high
  pinMode(TIMED_SPACE_PIN, INPUT); // Set the button as an input
  digitalWrite(TIMED_SPACE_PIN, HIGH); // Pull high
  Keyboard.begin();
}

void loop()
{//TODO make some sort of "pro-mode" that does not update the letter as one types the morse-char and just displays it after 600ms or similar.

  USE_TIMED_SPACE = !digitalRead(TIMED_SPACE_PIN);
  USE_LONG_PRESS_SPACE = !digitalRead(LONG_PRESS_PIN);

  if ( millis() - lastUpT > LETTER_PAUSE && !digitalRead(BUTTON_PIN)) {pos = 1;}
  if ( USE_TIMED_SPACE && millis() - lastUpT > WORD_PAUSE && !timedSpaceWritten && !wasDown)
      {pos = 1; Keyboard.write(' '); timedSpaceWritten = true;}

  if ( !wasDown && !digitalRead(BUTTON_PIN) ) {//Switch gets pressed
    //upT = millis() - lastUpT;
    lastDownT = millis();
    timedSpaceWritten = false;//After down press, reset
    wasDown = true;
  }

  if ( wasDown && digitalRead(BUTTON_PIN) ){//Switch gets released
    downT = millis() - lastDownT;
    if( DEBOUNCE_TIME < downT ) {

        if(downT < MAX_POINT_TIME )
            pos *= 2;// .
        else if ( downT < MAX_DASH_TIME )
            pos = pos*2+1;// _

        if ( 3 < pos ) {//if there is already a char, delete it or if the pos it outside fo the heap range, send backspace.
            if ( HEAP_SIZE+1 < pos )
                pos = HEAP_SIZE+2;//for infinite deltes
            Keyboard.write('\b');
        }

        if ( pos < HEAP_SIZE )//Do nothing, if pos == 0 as we would not want a space when USE_LONG_PRESS_SPACE is false
            if ( !USE_LONG_PRESS_SPACE && pos == 1) ;
            else if(morseHeap[pos] == "\r") Keyboard.write(KEY_RETURN);//FIXME AWFUL mess
            else Keyboard.print( morseHeap[pos] );

        lastUpT = millis();
        wasDown = false;
        }
    }
}
