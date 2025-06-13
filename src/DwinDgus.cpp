#include "DwinDgus.h"
#include <HardwareSerial.h>
#include <arduino.h>
#include <stdio.h>
#include <wstring.h>
#include "types.h"

int startprogress = 0;

float pause_e = 0;

 uint8_t LCDstate;

//millis_t next_rts_update_ms = 0;

char waitway = 0;
int recnum = 0;
bool SelectI2C = 1;
bool weightCalibration = true;
unsigned char original_extruder = 0;


int PrintModeTime = 0;
bool PrintMode = true;
bool PrintModeChange = true;
// PrintStatue[0], 0 represent  to 43 page, 1 represent to 44 page
char PrintStatue[2] = {0};

// represents to update file list
bool CardUpdate = false;

// !0 represent Chinese, 0 represent English
unsigned char LanguageRecbuf;

// PrinterStatusKey[1] value: 0 represents to keep temperature, 1 represents  to heating , 2 stands for cooling , 3
// stands for printing PrinterStatusKey[0] value: 0 reprensents 3D printer ready
char PrinterStatusKey[2] = {0};

// represents SD-card status, true means SD is available, false means opposite.
bool lcd_sd_status;

char Checkfilenum = 0;
char FilenamesCount = 0;
char cmdbuf[20] = {0};

// 0 for 10mm, 1 for 1mm, 2 for 0.1mm
unsigned char AxisPagenum = 0;
bool InforShowStatus = true;

// true for only opening time and percentage, false for closing time and percentage.
unsigned char AutoHomeIconNum;

//preintetiate the object of lcd
RTSSHOW rtscheck;

unsigned long startTime = 0;

char commandbuf[30];


// Constructor
RTSSHOW::RTSSHOW()
{
    recdat.head[0] = snddat.head[0] = FHONE;
    recdat.head[1] = snddat.head[1] = FHTWO;
    memset(databuf, 0, sizeof(databuf));
}

void RTSSHOW::RTS_Init()
{
    Serial1.begin(115200);

}

int RTSSHOW::RTS_RecData()
{
    while (Serial1.available() > 0 && (recnum < SizeofDatabuf))
    {
        databuf[recnum] = Serial1.read();
        // ignore the invalid data
        if (databuf[0] != FHONE)
        {
            // prevent the program from running.
            if (recnum > 0)
            {
                memset(databuf, 0, sizeof(databuf));
                recnum = 0;
            }
            continue;
        }

        if(!Serial1.available())
           delayMicroseconds(100);

        recnum++;
    }

// //for debugging purpose only
#ifdef Debug_LCD_RecvData
if(recnum > 0) {
for (size_t i = 0; i < recnum; i++) {
     Serial.print(databuf[i], HEX); Serial.print(" ");
}
Serial.println();
}
#endif



    // receive nothing
    if (recnum < 1)
    {
    return -1;
    }
    else if ((recdat.head[0] == databuf[0]) && (recdat.head[1] == databuf[1]) && recnum > 2)
    {
        recdat.len = databuf[2];
        recdat.command = databuf[3];
        if (recdat.len == 0x03 && (recdat.command == 0x82 || recdat.command == 0x80) && (databuf[4] == 0x4F) &&
            (databuf[5] == 0x4B)) // response for writing byte
        {
            memset(databuf, 0, sizeof(databuf));
            recnum = 0;
            return -1;
        }
        else if (recdat.command == 0x83)
        {
            // response for reading the data from the variate
            recdat.addr = databuf[4];
            recdat.addr = (recdat.addr << 8) | databuf[5];
            recdat.bytelen = databuf[6];
            for (int i = 0; i < recdat.bytelen; i += 2)
            {
                recdat.data[i / 2] = databuf[7 + i];
                recdat.data[i / 2] = (recdat.data[i / 2] << 8) | databuf[8 + i];
            }
        }
        else if (recdat.command == 0x81)
        {
            // response for reading the page from the register
            recdat.addr = databuf[4];
            recdat.bytelen = databuf[5];
            for (int i = 0; i < recdat.bytelen; i++)
            {
                recdat.data[i] = databuf[6 + i];
                // recdat.data[i]= (recdat.data[i] << 8 )| databuf[7+i];
            }
        }
    }
    else
    {
        memset(databuf, 0, sizeof(databuf));
        recnum = 0;
        // receive the wrong data
        return -1;
    }
    memset(databuf, 0, sizeof(databuf));
    recnum = 0;
    return 1;
}

void RTSSHOW::RTS_SndData(void)
{
    // Check if the head of the data is correct and the length is greater than or equal to 3
    if ((snddat.head[0] == FHONE) && (snddat.head[1] == FHTWO) && snddat.len >= 3)
    {
        // Copy the head and length to the databuf
        databuf[0] = snddat.head[0];
        databuf[1] = snddat.head[1];
        databuf[2] = snddat.len;
        databuf[3] = snddat.command;
        // to write data to the register
        if (snddat.command == 0x80)
        {
            // Copy the address and data to the databuf
            databuf[4] = snddat.addr;
            for (int i = 0; i < (snddat.len - 2); i++)
            {
                databuf[5 + i] = snddat.data[i];
            }
        }
        // to read data from the register
        else if (snddat.len == 3 && (snddat.command == 0x81))
        {
            // to read data from the register
            databuf[4] = snddat.addr;
            databuf[5] = snddat.bytelen;
        }
        else if (snddat.command == 0x82)
        {
            // to write data to the variate
            databuf[4] = snddat.addr >> 8;
            databuf[5] = snddat.addr & 0xFF;
            for (int i = 0; i < (snddat.len - 3); i += 2)
            {
                databuf[6 + i] = snddat.data[i / 2] >> 8;
                databuf[7 + i] = snddat.data[i / 2] & 0xFF;
            }
        }
        else if (snddat.len == 4 && (snddat.command == 0x83))
        {
            // to read data from the variate
            databuf[4] = snddat.addr >> 8;
            databuf[5] = snddat.addr & 0xFF;
            databuf[6] = snddat.bytelen;
        }
        for (int i = 0; i < (snddat.len + 3); i++)
        {
            Serial1.write(databuf[i]);
            delayMicroseconds(1);
        }

 //for debugging purpose only
#ifdef Debug_LCD_SentData
for (int i = 0; i < (snddat.len + 3); i++) {
     Serial.print(databuf[i], HEX); Serial.print(" ");
}
Serial.println();
#endif

        memset(&snddat, 0, sizeof(snddat));
        memset(databuf, 0, sizeof(databuf));
        snddat.head[0] = FHONE;
        snddat.head[1] = FHTWO;
    }
}

void RTSSHOW::RTS_SndData(const String &s, unsigned long addr, unsigned char cmd /*= VarAddr_W*/)
{
    if (s.length() < 1)
    {
        return;
    }
    RTS_SndData(s.c_str(), addr, cmd);
}

void RTSSHOW::RTS_SndData(const char *str, unsigned long addr, unsigned char cmd /*= VarAddr_W*/)
{
    int len = strlen(str);
    if (len > 0)
    {
        databuf[0] = FHONE;
        databuf[1] = FHTWO;
        databuf[2] = 3 + len;
        databuf[3] = cmd;
        databuf[4] = addr >> 8;
        databuf[5] = addr & 0x00FF;
        for (int i = 0; i < len; i++)
        {
            databuf[6 + i] = str[i];
        }

        for (int i = 0; i < (len + 6); i++)
        {
            Serial1.write(databuf[i]);
            delayMicroseconds(1);
        }
        memset(databuf, 0, sizeof(databuf));
    }
}

void RTSSHOW::RTS_SndData(char c, unsigned long addr, unsigned char cmd /*= VarAddr_W*/)
{
    snddat.command = cmd;
    snddat.addr = addr;
    snddat.data[0] = (unsigned long)c;
    snddat.data[0] = snddat.data[0] << 8;
    snddat.len = 5;
    RTS_SndData();
}

void RTSSHOW::RTS_SndData(unsigned char *str, unsigned long addr, unsigned char cmd)
{
    RTS_SndData((char *)str, addr, cmd);
}

void RTSSHOW::RTS_SndData(int n, unsigned long addr, unsigned char cmd /*= VarAddr_W*/)
{
    if (cmd == VarAddr_W)
    {
        if (n > 0xFFFF)
        {
            snddat.data[0] = n >> 16;
            snddat.data[1] = n & 0xFFFF;
            snddat.len = 7;
        }
        else
        {
            snddat.data[0] = static_cast<short>(n);
            snddat.len = 5;
        }
    }
    else if (cmd == RegAddr_W)
    {
        snddat.data[0] = n;
        snddat.len = 3;
    }
    else if (cmd == VarAddr_R)
    {
        snddat.bytelen = n;
        snddat.len = 4;
    }
    snddat.command = cmd;
    snddat.addr = addr;
    RTS_SndData();
}

void RTSSHOW::RTS_SndData(unsigned int n, unsigned long addr, unsigned char cmd)
{
    RTS_SndData((int)n, addr, cmd);
}

void RTSSHOW::RTS_SndData(float n, unsigned long addr, unsigned char cmd)
{
    RTS_SndData((int)n, addr, cmd);
}

void RTSSHOW::RTS_SndData(long n, unsigned long addr, unsigned char cmd)
{
    RTS_SndData((unsigned long)n, addr, cmd);
}

void RTSSHOW::RTS_SndData(unsigned long n, unsigned long addr, unsigned char cmd /*= VarAddr_W*/)
{
    if (cmd == VarAddr_W)
    {
            snddat.data[0] = n >> 16;
            snddat.data[1] = n & 0xFFFF;
            snddat.len = 7;
    }
    else if (cmd == VarAddr_R)
    {
        snddat.bytelen = n;
        snddat.len = 4;
    }
    snddat.command = cmd;
    snddat.addr = addr;
    RTS_SndData();
}

void RTSSHOW::RTS_HandleData()
{
    int Checkkey = -1;
    // for waiting
    if (waitway > 0)
    {
        memset(&recdat, 0, sizeof(recdat));
        recdat.head[0] = FHONE;
        recdat.head[1] = FHTWO;
        return;
    }
    for (int i = 0; Addrbuf[i] != 0; i++)
    {
        if (recdat.addr == Addrbuf[i])  //Validate addr
        {
            if (Addrbuf[i] == PageAddr)
            {
                Checkkey = PROC_COM::PageChanged;
            }
            else if (Addrbuf[i] == HeaterButton)
            {
                Checkkey = PROC_COM::HeaterButtonPress;
            }
            else if ((Addrbuf[i] == MotorButton) || (Addrbuf[i] == MotorSlider))
            {
                Checkkey = PROC_COM::MotorChoice;
            }
            else if ((Addrbuf[i] == LidButtons))
            {
                Checkkey = PROC_COM::LidChoise;
            }
            else if ((Addrbuf[i] == LEDSLIDER) || (Addrbuf[i] == FANSLIDER))
            {
                Checkkey = PROC_COM::pwmChoice;
            }


            // if ((Addrbuf[i] >= Stopprint) && (Addrbuf[i] <= Resumeprint))
            // {
            //     Checkkey = PrintChoice;
            // }
            // else if ((Addrbuf[i] == NzBdSet) || (Addrbuf[i] == NozzlePreheat) || (Addrbuf[i] == BedPreheat))
            // {
            //     Checkkey = ManualSetTemp;
            // }
            // else if ((Addrbuf[i] >= AutoZero) && (Addrbuf[i] <= DisplayZaxis))
            // {
            //     Checkkey = XYZEaxis;
            // }
            // else if ((Addrbuf[i] >= FilementUnit1) && (Addrbuf[i] <= FilementUnit2))
            // {
            //     Checkkey = Filement;
            // }
            // else
            // {
            //     Checkkey = i;
            // }
            // break;
        }
    }

    if (Checkkey < 0)
    {
        memset(&recdat, 0, sizeof(recdat));
        recdat.head[0] = FHONE;
        recdat.head[1] = FHTWO;
        return;
    }

    switch (Checkkey)
    {
    case PROC_COM::PageChanged:
       if (recdat.data[0] == 0)
       {
        RTS_SndData(ExchangePageBase + 0, ExchangepageAddr);
        startTime = 0;
        SetRTSState(HOME);
       }
       else if (recdat.data[0] == 1) {
       SetRTSState(I2CSCAN);
       SelectI2C = 1;
       }
       else if (recdat.data[0] == 2) {
       SetRTSState(I2CSCAN);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 4) {
       SetRTSState(SENSORS);
       RTS_SndData(ExchangePageBase + 4, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 5) {
       SetRTSState(WEIGHT);
       RTS_SndData(ExchangePageBase + 5, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 6) {
       SetRTSState(HEATERTEST);
       RTS_SndData(ExchangePageBase + 6, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 7) {
       SetRTSState(TREXTEST);
       RTS_SndData(ExchangePageBase + 7, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 8) {
       SetRTSState(PWMTEST);
       RTS_SndData(ExchangePageBase + 8, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 9) {
       SetRTSState(PUMPMOTOR);
       RTS_SndData(ExchangePageBase + 9, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 10) {
       SetRTSState(BATTERYTEST);
       RTS_SndData(ExchangePageBase + 10, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 11) {
       SetRTSState(LIDLOCKTEST);
       RTS_SndData(ExchangePageBase + 11, ExchangepageAddr);
       RTS_SndData(0, 0x218A);
       RTS_SndData(0, 0x218C);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 13) {
       SetRTSState(HOME);
       RTS_SndData(ExchangePageBase + 13, ExchangepageAddr);
       SelectI2C = 0;
       }
       else if (recdat.data[0] == 14) {
       SetRTSState(SOLENOIDTEST);
       // RTS_SndData(ExchangePageBase + 14, ExchangepageAddr);  //there is no page exists
       SelectI2C = 0;
       }
    break;
case PROC_COM::HeaterButtonPress:
if (recdat.data[0] == 0){
       hChoice.buttonState = 1;
}
else if(recdat.data[0]== 1){
    hChoice.buttonState = 2;
}
break;

case PROC_COM::MotorChoice:
if (recdat.addr == MotorButton)
{
    if (recdat.data[0] == 0)
    {
                mChoice.buttonState = 2;
    }
    else if (recdat.data[0] == 1)
    {
                mChoice.buttonState = 1;
    }
}
else if(recdat.addr == MotorSlider){
mChoice.sliderValue = highByte(recdat.data[0]);
}
break;

case PROC_COM::LidChoise:
if (recdat.data[0] == 1)
{
lidChoice.buttonState = 1;
}
else if (recdat.data[0] == 2)
{
rtscheck.RTS_SndData(1, 0x218C);
lidChoice.buttonState = 2;
}

break;

case PROC_COM::pwmChoice:
if (recdat.addr == LEDSLIDER)
{
pwmChoice.ledSlider = highByte(recdat.data[0]);
}
else if (recdat.addr == FANSLIDER)
{
pwmChoice.fanSlider = highByte(recdat.data[0]);
}

break;



}
    memset(&recdat, 0, sizeof(recdat));
    recdat.head[0] = FHONE;
    recdat.head[1] = FHTWO;
}

// looping at the loop function
void RTSUpdate()
{
    // wait to receive massage and response
    if (rtscheck.RTS_RecData() > 0)
    {
    rtscheck.RTS_HandleData();
    }
}
