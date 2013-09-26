//////////////////////////////////////////////
// Get XML formatted data from the XIG. Use it to control LED Driver.
// Original Code from Bob S.
// Modified by Alex Olivier, 12/12/2012.
//////////////////////////////////////////////

// Include description files for other libraries used (if any)
#include <string.h>
#include “Tlc5940.h”
#include “tlc_fades.h”
TLC_CHANNEL_TYPE channel;

// Define Constants
// Max string length may have to be adjusted depending on data to be extracted
#define MAX_STRING_LEN 40

// Setup vars
char tagStr[MAX_STRING_LEN] = “”;
char dataStr[MAX_STRING_LEN] = “”;
char tmpStr[MAX_STRING_LEN] = “”;
char endTag[3] = {‘<’, ‘/’, ”};
int len;
boolean getData = false;

// Flags to differentiate XML tags from document elements (ie. data)
boolean tagFlag = false;
boolean dataFlag = false;
unsigned long time;

//for leds
uint16_t duration =2000; //default LED speed

void setup()
{
  Tlc.init(); //initialize LED Driver code
  time = 0;
  Serial.begin(115200);
}

void loop() 
{
  // sample every 30 seconds
  if ((millis() – time > 30000)|| time == 0)
  {
    Serial.println(“http://itpatnyu:Interact4u!@my.idigi.com/ws/DataPoint/dia/channel/00000000-00000000-00409DFF-FF521E03/XBee_4079C120/average_amplitude_front?rollupInterval=hour&rollupMethod=average”);
    time = millis();
  }

  // code for controlling LEDs
  if (tlc_fadeBufferSize < TLC_FADE_BUFFER_LENGTH – 2)
  {
    if (!tlc_isFading(channel))
    {
      int maxValue = 4095;
      uint32_t startMillis = millis() + 50;
      uint32_t endMillis = startMillis + duration;
      tlc_addFade(channel, 0, maxValue, startMillis, endMillis);
      tlc_addFade(channel, maxValue, 0, endMillis, endMillis + duration);
    }
    if (channel++ == NUM_TLCS * 16)
    {
      channel = 0;
    }
  }
  
  tlc_updateFades();

}

// Process each char from web
void serialEvent()
{
  while (Serial.available())
  {
    // Read a char
    char inChar = Serial.read();
    if (inChar == ‘<’)
    {
      // a tag is starting
      //Serial.println(“tag!”);
      // if we’re done getting the data chars
      if (getData == true)
      { 
        getData = false;
        double dataNum = atof(dataStr); // convert chars to a double
        // map sound data to LED animation speed
        int clippedData = constrain(dataNum, 400, 800);
        int lightDelay = map(clippedData, 400,800, 1000, 7000);
        duration = lightDelay;;
        clearStr(dataStr); // flush out data string
      }
      // if it’s not data, start adding the chars to the other string
      else
      {
        addChar2(inChar, tmpStr);
        tagFlag = true;
      }
    }
    else if (inChar == ‘>’)
    { 
      // we’re done with this tag
      addChar2(inChar, tmpStr);
      tagFlag = false;
      // is it a data tag
      if (strcmp(tmpStr, “<data>”)==0)
      { 
        getData = true; // get data until the next < tag.
      }
      clearStr(tmpStr);
    }
    else if (tagFlag == true)
    {
      addChar2(inChar, tmpStr);
    }
    else if (getData == true)
    {
      addChar2(inChar, dataStr);
    }
  }
}

/////////////////////
// Other Functions //
/////////////////////

// Function to clear a string
void clearStr (char* str)
{
  int len = strlen(str);
  for (int c = 0; c < len; c++)
  {
    str[c] = 0;
  }
}

//Function to add a char to a string and check its length (Alex version)
void addChar2 (char ch, char* str)
{
  str[strlen(str)] = ch;
}

// Function to check the current tag for a specific string
boolean matchTag (char* searchTag)
{
  if ( strcmp(tagStr, searchTag) == 0 )
  {
    return true;
  }
  else
  {
    return false;
  }
}
