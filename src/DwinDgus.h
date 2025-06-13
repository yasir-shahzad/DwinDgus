/*
* DWIN DGUS DWIN Library for Arduino Uno | ESP32 
* This Library Supports all Basic Function
* Created by Yasir Shahzad ( yasirshahzad918@gmail.com ) 
* Please Checkout Latest Offerings FROM DWIN 
* Here : https://www.dwin-global.com/
*/


#ifndef DWINDGUS_H
#define DWINDGUS_H

#include "string.h"
#include "arduino.h"

//#define Debug_LCD_SentData 
//#define Debug_LCD_RecvData 

/*********************************/
#define FHONE   (0x5A)
#define FHTWO   (0xA5)
#define TEXTBYTELEN     18
#define MaxFileNumber   20

#define	CEIconGrap      12
#define	FileNum	MaxFileNumber
#define	FileNameLen	TEXTBYTELEN
#define	RTS_UPDATE_INTERVAL 2000
#define	RTS_UPDATE_VALUE    (2*RTS_UPDATE_INTERVAL)

#define SizeofDatabuf		26

/*************Register and Variable addr*****************/
#define	RegAddr_W	0x80
#define	RegAddr_R	0x81
#define	VarAddr_W	0x82
#define	VarAddr_R	0x83
#define	ExchangePageBase    ((unsigned long)0x5A010000)     // the first page ID. other page = first page ID + relevant num;
#define	StartSoundSet       ((unsigned long)0x060480A0)     // 06,start-music; 04, 4 musics; 80, the volume value; 04, return value about music number.


/*variable addr*/
#define	ExchangepageAddr	0x0084
#define	SoundAddr			0x00A0
#define	StartIcon			0x1000
#define	FeedrateDisplay		0x1006
#define	PageAddr			0x2000
#define	HeaterButton	0x2126
#define	MotorButton 	0x2152
#define	MotorSlider 	0x2150
#define	LidButtons  	0x2188
#define	LEDSLIDER  	  0x2160
#define	FANSLIDER  	  0x2162



#define	Pauseprint			0x100A
#define	Resumeprint			0x100C
#define	PrintscheduleIcon	0x100E
#define	Timehour			0x1010
#define	Timemin				0x1012
#define	IconPrintstatus		0x1014
#define	Percentage			0x1016
#define	FanKeyIcon			0x101E

#define	HeatPercentIcon		0x1024

#define	NzBdSet				0x1032
#define	NozzlePreheat		0x1034
#define	NozzleTemp			0x1036
#define	BedPreheat			0x103A
#define	Bedtemp			  	0x103C

#define	AutoZeroIcon		0x1042
#define	AutoLevelMode		0x1045
#define	AutoZero			0x1046
#define	DisplayXaxis		0x1048
#define	DisplayYaxis		0x104A
#define	DisplayZaxis		0x104C

#define	FilementUnit1		0x1054
#define	Exchfilement		0x1056
#define	FilementUnit2		0x1058

#define	MacVersion			0x1060
#define	SoftVersion			0x106A
#define	PrinterSize			0x1074
#define	CorpWebsite			0x107E
#define	VolumeIcon			0x108A
#define	SoundIcon			  0x108C
#define	AutolevelIcon		0x108D
#define	ExchFlmntIcon		0x108E
#define	SensAddrVal	  	0x2100
#define	BatteryAddrVal	0x2170
#define HeaterAddr      0x2120
#define WeightAddr      0x2130


#define	FilenameIcon		0x1200
#define	FilenameIcon1		0x1220
#define	Printfilename		0x2000
#define	SDFILE_ADDR			0x200A
#define	FilenamePlay		0x20D2
#define	FilenameChs			0x20D3
#define	Choosefilename	0x20D4
#define	FilenameCount		0x20DE
#define	FilenameNature	0x6003

extern uint8_t LCDstate;

enum RTSState {
    HOME = 0,
    WAIT,
    I2CSCAN,
    SENSORS,
    WEIGHT,
    TREXTEST,
    PUMPMOTOR,
    HEATERTEST,
    SOLENOIDTEST,
    PWMTEST,
    BATTERYTEST,
    LIDLOCKTEST,
    TOTALRTS
};

inline void SetRTSState(RTSState status) { LCDstate = status; }
inline uint8_t GetRTSState(void) { return LCDstate; };


/************struct**************/
typedef struct DataBuf
{
    unsigned char len;
    unsigned char head[2];
    unsigned char command;
    unsigned long addr;
    unsigned long bytelen;
    uint16_t      data[32];
    unsigned char reserv[4];
} DB;

typedef struct pageButtonChoice
{
    uint8_t buttonState;
    int8_t sliderValue;
} PB;

typedef struct pwmSliderChoice
{
    int8_t ledSlider;
    int8_t fanSlider;
    uint8_t eSwitchState;
} pwmSL;

class RTSSHOW {
  public:
    RTSSHOW();
    int RTS_RecData();
    void RTS_SndData(void);
    void RTS_SndData(const String &, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(const char[], unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(char, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(unsigned char*, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(int, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(float, unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(unsigned int,unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(long,unsigned long, unsigned char = VarAddr_W);
    void RTS_SndData(unsigned long,unsigned long, unsigned char = VarAddr_W);
    void RTS_HandleData();
    void RTS_Init();
    
    DB recdat;
    DB snddat;
    static inline PB hChoice;
    static inline PB mChoice;
    static inline PB lidChoice;
    static inline pwmSL pwmChoice;



  private:
    unsigned char databuf[SizeofDatabuf];
  };

/// preinstiate the object
extern RTSSHOW rtscheck;

#define	Addvalue	3
#define	PrintChoice_Value	(0+Addvalue)
#define	Zoffset_Value		(3+Addvalue)
#define	Setting_Value		(8+Addvalue)
#define	XYZEaxis_Value		(12+Addvalue)
#define	Filement_Value		(15+Addvalue)
#define	Language_Value		(18+Addvalue)
#define	Filename_Value		(22+Addvalue)

enum PROC_COM {PageChanged = 0, HeaterButtonPress, MotorChoice, LidChoise, pwmChoice, Feedrate, PrintChoice = PrintChoice_Value, Zoffset=Zoffset_Value, TempControl, ManualSetTemp,
               Setting=Setting_Value, ReturnBack, Bedlevel, Autohome, XYZEaxis = XYZEaxis_Value, Filement = Filement_Value,
			   LanguageChoice = Language_Value, No_Filement, PwrOffNoF, Volume, Filename = Filename_Value
              };

const unsigned long Addrbuf[] = {0x2000, 0x200B, 0x2008, 0x200A, 0x200E,0x2126,0x2152,0x2150,0x2160,0x2162,0x2188, 0x1004, 0x1006, 0x1008, 0x100A, 0x100C, 0x1026, 0x1030, 0x1032, 0x1034, 0x103A,
                                    0x1046, 0x1048, 0x104A, 0x104C, 0x1054, 0x1056, 0x1058, 0x105C,
                                 0x105E, 0x105F, 0x1088,  0
                                };

extern void RTSUpdate();
extern void RTSInit();

extern char waitway;
extern bool InforShowStatus;
extern unsigned char LanguageRecbuf;
extern unsigned char AxisPagenum;
extern bool AutohomeKey;
extern bool TPShowStatus;

extern bool SelectI2C;
extern bool weightCalibration;
extern unsigned long startTime;


extern char commandbuf[30];
extern int PrintModeTime;

#endif// DWINDGUS_H
