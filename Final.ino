#include<Wire.h>
//#include <SoftwareSerial.h>
#include "RTClib.h"
#include  <EEPROM.h>
#include <string.h>

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const byte numChars = 32;
char temp[numChars];
char tempChars[numChars];
boolean newData = false;
boolean schflag = false;

char rdwtbt[numChars] = {0};

typedef struct schedule
{
char time[numChars];
}sch;

sch finSch[6];
sch Eebuff[6];

void setup()
{

   Serial.begin(9600);
   Serial.println("Code is for reading and writing schedule to the Microcontroller and configuring the alarms.");
   Serial.println("Data is sent from the app in the following format");
   Serial.println("(0:Read/1:Write , <Time of the day bit> )");
   Serial.println(" Configurable times are :\n1] Morning\n2] Afternoon\n3] Night\n4] Two Times\n5] Three Times");
   for(int j=2;j<8;j++)
   pinMode(j,OUTPUT);

  if (! rtc.begin())
  {
    //Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 11, 46, 0));
  }
}

void loop()
{
  DateTime now = rtc.now();

  int cmd=getCmd();


  switch (cmd)
  {
    case 0:
    readEepromSch();
    break;

    case 1:
    writeEepromSch();
    break;

    case 2:
    schflag = true;
    break;

  }

  compTm();

//writeEepromSch();
//getSchedule();
//readEepromSch();
//while (1);

/*
    Serial.println("RTC STAT");

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
*/

  delay(3000);

}



//=============================================================================

/*void RecWEnd()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char endMarker = '>';
    char rc;
                                                Serial.println("Debug 1");
    while (!Serial.available());
    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();
        recvInProgress = true;
                                                Serial.println("Debug 2");
        if (recvInProgress == true)
        {

            if (rc != endMarker)
            {
                                                Serial.println("Debug 3");
                temp[ndx] = rc;
                ndx++;
                if (ndx >= numChars)
                {
                                                Serial.println("Debug 4");
                    ndx = numChars - 1;
                }
            }
            else
            {
                                              Serial.println("Debug end");
                temp[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }
    }
}*/

//=============================================================================

void RecWEnd()
{
    static byte ndx = 0;
    char endMarker = '>';
    char rc;
                                                Serial.println("Debug 1");
    while (!Serial.available());
    while (Serial.available() > 0 && newData == false)
    {
        rc = Serial.read();
                                                Serial.println("Debug 2");

            if (rc != endMarker)
            {
                                                Serial.println("Debug 3");
                temp[ndx] = rc;
                ndx++;
                if (ndx >= numChars)
                {
                                                Serial.println("Debug 4");
                    ndx = numChars - 1;
                }
            }
            else
            {
                                              Serial.println("Debug end");
                temp[ndx] = '\0'; // terminate the string
                ndx = 0;
                newData = true;
            }
      }
}
//=============================================================================

void parseData(int mpos)
{
   // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(rdwtbt, strtokIndx); // copy it to messageFromPC

    strtokIndx = strtok(NULL, ",");
    //strcpy(time, strtokIndx);
    strcpy(finSch[mpos].time, strtokIndx);
    //floatFromPC = atof(strtokIndx);     // convert this part to a float

}
//=============================================================================

void showParsedData(int mpos) {
    Serial.print("Read/Write ");
    Serial.println(rdwtbt);
    Serial.print("Medicine Number ");
    Serial.println(mpos);
    Serial.print("Time ");
    Serial.println(finSch[mpos].time);
}

//=============================================================================

void getSchedule()
{
  int mpos;
  for(mpos=0;mpos<6;mpos++)
  {
    for(int i=0;i<3;i++)
    RecWEnd();
    Serial.println(mpos);
    if (newData == true)
    {

      //Serial.println(mpos);
        strcpy(tempChars, temp);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData(mpos);
        showParsedData(mpos);
        newData = false;
    }
  }
  Serial.println("Done Get Schedule");

}

//=============================================================================

void readEepromSch()
{
  if(schflag)
  {
    EEPROM.get(0,Eebuff);

    for(int i=0;i<6;i++)
    {
      Serial.print("Medicine number");
      Serial.print(i);
      Serial.print("Time");
      Serial.println(Eebuff[i].time);
    }
    /*for(int i=0;i<6;i++)
    {
      Serial.print(EEPROM.read(i));
      Serial.println(EEPROM.read(i+6));
    }*/
  }
  else
  Serial.println("Schedule not set");
}

//=============================================================================

void writeEepromSch()
{
  getSchedule();
  //Eebuff=finSch;
  EEPROM.put(0,finSch);
  schflag = true;
}

//=============================================================================

int getCmd()
{
  for(int i=0;i<2;i++)
  RecWEnd();
  Serial.println(0);
  if (newData == true)
  {
          strcpy(tempChars, temp);
          // this temporary copy is necessary to protect the original data
          //   because strtok() used in parseData() replaces the commas with \0
      parseData(0);
      //showParsedData(0);
      newData = false;
  }
  int cmd= atoi(rdwtbt);
  Serial.print("Command");
  Serial.println(cmd);
  return(cmd);

}

//=============================================================================

void compTm()
{
  DateTime now = rtc.now();
  int tm;
  readEepromSch();

  if(now.hour()==8)
  {
    for(int i=0;i<6;i++)
    {
      tm= atoi(Eebuff[i].time);
      if(tm==1||tm==4||tm==5)
      digitalWrite(i+2,HIGH);
    }

    delay(1000);

    for(int i=2;i<8;i++)
    {
      digitalWrite(i,LOW);
    }
  }

  if(now.hour()==15)
  {
    for(int i=0;i<6;i++)
    {
      tm= atoi(Eebuff[i].time);
      if(tm==2||tm==5)
      digitalWrite(i+2,HIGH);
    }

    delay(1000);

    for(int i=2;i<8;i++)
    {
      digitalWrite(i,LOW);
    }
  }

  if(now.hour()==20)
  {
    for(int i=0;i<6;i++)
    {
      tm= atoi(Eebuff[i].time);
      if(tm==3||tm==4||tm==5)
      digitalWrite(i+2,HIGH);
    }

    delay(1000);

    for(int i=2;i<8;i++)
    {
      digitalWrite(i,LOW);
    }
  }

}
