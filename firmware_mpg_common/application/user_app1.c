/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */

static bool bSetAge = TRUE;
static bool bFindMode = TRUE;
static bool bClear = TRUE;
static u8 u8ChangeFlag=0;


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  static u8 au8Networkkey[8] = AU8_ANT_PLUS_NETWORK_KEY;
  AntAssignChannelInfoType sAntSetupData;

  PWMAudioSetFrequency(BUZZER1, 2000);
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, "B1 CommonMode B2 Los");
  LCDMessage(LINE2_START_ADDR, "eWeight B3 FindMode ");
  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = au8Networkkey[i];
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel assignment is queued so start timer */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    
    UserApp1_StateMachine = UserApp1SM_IfConfigured;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
static void UserApp1SM_IfConfigured(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_JudgeButton;
  }

  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Fail to Configure");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}/**/


/**/
static void UserApp1SM_JudgeButton(void)
{
  bSetAge = TRUE;
  bFindMode = TRUE;
  bClear = TRUE;
  u8ChangeFlag = 0;

  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);

    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_IfChannelOpen1;
  }

  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);

    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_IfChannelOpen2;
  }

  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);

    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_IfChannelOpen3;
  }
}/**/


/**/
static void UserApp1SM_IfChannelOpen1(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_CommonMode;
  }

  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Fail to open channel");
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);

    UserApp1_StateMachine = UserApp1SM_JudgeButton;
  }
}/**/


/**/
static void UserApp1SM_IfChannelOpen2(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_LoseWeightMode;
  }

  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Fail to open channel");
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);

    UserApp1_StateMachine = UserApp1SM_JudgeButton;
  }
}/**/


/**/
static void UserApp1SM_IfChannelOpen3(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_FindMode;
  }

  if( IsTimeUp(&UserApp1_u32Timeout, 5000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Fail to open channel");
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);

    UserApp1_StateMachine = UserApp1SM_JudgeButton;
  }
}/**/


/**/
static void UserApp1SM_CommonMode(void)
{
  static bool bLow = FALSE;
  static bool bHigh = FALSE;
  static u8 au8HeartRate[] = {'H', 'e', 'a', 'r', 't', 'R', 'a', 't', 'e', ':', 0, 0, 0};
  u8 au8LowRate[] = "Too low!Be careful!";
  u8 au8NormalRate[] = "Your Rate is Normal!";
  u8 au8HighRate[] = "Too High!Attention!";
  static u8 u8HeartRate = 50;

  if( AntReadAppMessageBuffer() )
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      u8HeartRate = G_au8AntApiCurrentMessageBytes[7];
      au8HeartRate[10] = G_au8AntApiCurrentMessageBytes[7]/100 + 48;
      au8HeartRate[11] = G_au8AntApiCurrentMessageBytes[7]%100/10 + 48;
      au8HeartRate[12] = G_au8AntApiCurrentMessageBytes[7]%10 + 48;

      LCDClearChars(LINE1_START_ADDR, 20);
      LCDMessage(LINE1_START_ADDR, au8HeartRate);

      if(u8HeartRate < 50 && !bLow)
      {
        bLow = TRUE;
        LCDClearChars(LINE2_START_ADDR, 20);
        LCDMessage(LINE2_START_ADDR, au8LowRate);
        //PWMAudioOn(BUZZER1);
        LedOff(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
        LedBlink(WHITE, LED_2HZ);
        LedBlink(PURPLE, LED_2HZ);
        LedBlink(BLUE, LED_2HZ);
        LedBlink(CYAN, LED_2HZ);
        LedBlink(GREEN, LED_2HZ);
        LedBlink(YELLOW, LED_2HZ);
        LedBlink(ORANGE, LED_2HZ);
        LedBlink(RED, LED_2HZ);
      }
      if(u8HeartRate > 50 && u8HeartRate < 60)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOff(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 60 && u8HeartRate < 70)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOn(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 70 && u8HeartRate < 80)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 80 && u8HeartRate < 90)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 90 && u8HeartRate < 100)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 100 && u8HeartRate < 110)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 110 && u8HeartRate < 120)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOn(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 120 && u8HeartRate < 130)
      {
        bLow = FALSE;
        bHigh = FALSE;
        LCDMessage(LINE2_START_ADDR, au8NormalRate);
        PWMAudioOff(BUZZER1);
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOn(YELLOW);
        LedOn(ORANGE);
        LedOff(RED);
      }
      if(u8HeartRate > 130 && !bHigh)
      {
        bHigh = TRUE;
        LCDClearChars(LINE2_START_ADDR, 20);
        LCDMessage(LINE2_START_ADDR, au8HighRate);
        //PWMAudioOn(BUZZER1);
        LedOff(ORANGE);
        LedOff(RED);
        LedOff(PURPLE);
        LedOff(CYAN);
        LedOff(YELLOW);
        LedOff(BLUE);
        LedOff(GREEN);
        LedOff(WHITE);
        LedBlink(ORANGE,LED_2HZ);
        LedBlink(RED,LED_2HZ);
        LedBlink(PURPLE,LED_2HZ);
        LedBlink(CYAN,LED_2HZ);
        LedBlink(YELLOW,LED_2HZ);
        LedBlink(BLUE,LED_2HZ);
        LedBlink(GREEN,LED_2HZ);
        LedBlink(WHITE,LED_2HZ);
      }
    }
  }

  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_JudgeButton;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "B1 CommonMode B2 Los");
    LCDMessage(LINE2_START_ADDR, "eWeight B3 FindMode ");
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
  }
}/**/


/**/
static void UserApp1SM_LoseWeightMode(void)
{
  static u16 u16ReciprocalCounter = 10000;
  static u8 u8Counter = 0;
  static u8 u8MaxLoseWeightHR = 0;
  static u8 u8MinLoseWeightHR = 0;
  static u32 u32Sum = 0;
  static u8 au8Sum[100];
  u8 au8bSetAge[] = "SET YOUR AGE";
  static u8 au8HeartRate[]={'H','e','a','r','t','R','a','t','e',':', 0, 0, 0};
  static u8 au8ApproriateHR[20]={'A','p','p','r','o','p','r','i','a','t','e',':','0','0','0','-','0','0','0'};
  static u8 u8MaxRate = 0;
  static u8 u8Age = 0;
  static u8 u8StaticRate = 0;
  static bool bApproriateHR = TRUE;

  static u8 au8AgeMessage[20]={' ',' ',' ','C','O','N','T','I','N','U','E',' ',' ','0',' ',' ',' ',' ',' ','0'};
  static u8 u8AgeHi[1];
  static u8 u8AgeLo[1];

  if(bSetAge)
  {
    bSetAge = FALSE;
    bApproriateHR = TRUE;
    au8AgeMessage[13] = 48;
    au8AgeMessage[19] = 48;
    u8AgeLo[0] = 0;
    u8AgeHi[0] = 0;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8bSetAge);
    LCDMessage(LINE2_START_ADDR, au8AgeMessage);
  }

  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);

    u8ChangeFlag = 1;
    LCDCommand(LCD_CLEAR_CMD);
    u8Age = u8AgeHi[0]*10 + u8AgeLo[0];
    u8MaxRate = 220 - u8Age;
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    u8AgeHi[0] = u8AgeHi[0] + 1;
    if(u8AgeHi[0] > 9)
    {
      u8AgeHi[0] = u8AgeHi[0] - 10;
    }
    au8AgeMessage[13] = u8AgeHi[0] + 48;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8bSetAge);
    LCDMessage(LINE2_START_ADDR, au8AgeMessage);
  }
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    u8AgeLo[0] = u8AgeLo[0] + 1;
    if(u8AgeLo[0] > 9)
    {
      u8AgeLo[0] = u8AgeLo[0] - 10;
    }
    au8AgeMessage[19] = u8AgeLo[0] + 48;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8bSetAge);
    LCDMessage(LINE2_START_ADDR, au8AgeMessage);
  }

  if(u8ChangeFlag == 1)
  {
    u16ReciprocalCounter--;
    if( AntReadAppMessageBuffer() )
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        au8HeartRate[10] = G_au8AntApiCurrentMessageBytes[7]/100 + 48;
        au8HeartRate[11] = (G_au8AntApiCurrentMessageBytes[7]%100)/10 + 48;
        au8HeartRate[12] = (G_au8AntApiCurrentMessageBytes[7]%100)%10 + 48;
        LCDClearChars(LINE1_START_ADDR, 20);
        LCDMessage(LINE1_START_ADDR, au8HeartRate);

        au8Sum[u8Counter] = G_au8AntApiCurrentMessageBytes[7];
        u8Counter++;
      }
    }
    if(u16ReciprocalCounter == 0)
    {
      u8ChangeFlag = 2;
      LCDCommand(LCD_CLEAR_CMD);
    }
  }
  if(u8ChangeFlag == 2)
  {
    u16ReciprocalCounter = 10000;
    for(u8 i = 0;i < u8Counter;i++)
    {
      u32Sum+=au8Sum[i];
    }
    u8StaticRate = u32Sum/u8Counter;
    u8ChangeFlag = 3;
  }
  if(u8ChangeFlag == 3)
  {
    u8Counter=0;
    u8MaxLoseWeightHR = ((u8MaxRate - u8StaticRate) * 0.85) + u8StaticRate;
    u8MinLoseWeightHR = ((u8MaxRate - u8StaticRate) * 0.65) + u8StaticRate;
    au8ApproriateHR[12] = u8MinLoseWeightHR/100 + 48;
    au8ApproriateHR[13] = (u8MinLoseWeightHR%100)/10 + 48;
    au8ApproriateHR[14] = (u8MinLoseWeightHR%100)%10 + 48;
    au8ApproriateHR[16] = u8MaxLoseWeightHR/100 + 48;
    au8ApproriateHR[17] = (u8MaxLoseWeightHR%100)/10 + 48;
    au8ApproriateHR[18] = (u8MaxLoseWeightHR%100)%10 + 48;

    if(bApproriateHR)
    {
      LCDMessage(LINE2_START_ADDR, au8ApproriateHR);
      bApproriateHR = FALSE;
    }

    if( AntReadAppMessageBuffer() )
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        au8HeartRate[10] = G_au8AntApiCurrentMessageBytes[7]/100 + 48;
        au8HeartRate[11] = (G_au8AntApiCurrentMessageBytes[7]%100)/10 + 48;
        au8HeartRate[12] = (G_au8AntApiCurrentMessageBytes[7]%100)%10 + 48;
        LCDClearChars(LINE1_START_ADDR, 20);
        LCDMessage(LINE1_START_ADDR, au8HeartRate);

        au8Sum[u8Counter] = G_au8AntApiCurrentMessageBytes[7];
        u8Counter++;
      }
    }
  }

  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_JudgeButton;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "B1 CommonMode B2 Los");
    LCDMessage(LINE2_START_ADDR, "eWeight B3 FindMode ");
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
  }
}/**/


/**/
static void UserApp1SM_FindMode(void)
{
  static s8 s8RssiChannel0 = -99;
  static u8 au8Temp[] = {'R','S','S','I', ':', '-', 0, 0, 'd', 'B', 'm', '\0'};
  static u8 u8Temp;

  if(bFindMode)
  {
    bFindMode = FALSE;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Find Mode");
  }

  if( AntReadAppMessageBuffer() )
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        if(G_sAntApiCurrentMessageExtData.u8Channel == 0)
        {
          s8RssiChannel0 = G_sAntApiCurrentMessageExtData.s8RSSI;
        }
      }

      u8Temp = abs(s8RssiChannel0);
      au8Temp[6] = u8Temp/10 + 48;
      au8Temp[7] = u8Temp%10 + 48;
      LCDMessage(LINE2_START_ADDR, au8Temp);

      if(s8RssiChannel0 < -90)
      {
          LedOff(WHITE);
          LedOff(PURPLE);
          LedOff(BLUE);
          LedOff(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -90 || s8RssiChannel0 == -90)
      {
          LedOn(WHITE);
          LedOff(PURPLE);
          LedOff(BLUE);
          LedOff(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -85 || s8RssiChannel0 == -85)
      {
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOff(BLUE);
          LedOff(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -80 || s8RssiChannel0 == -80)
      {
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOff(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -75 || s8RssiChannel0 == -75)
      {
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOff(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -70 || s8RssiChannel0 == -70)
      {
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOff(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -65 || s8RssiChannel0 == -65)
      {
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOn(YELLOW);
          LedOff(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -60 || s8RssiChannel0 == -60)
      {
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOn(YELLOW);
          LedOn(ORANGE);
          LedOff(RED);
      }
      if(s8RssiChannel0 > -55 || s8RssiChannel0 == -55)
      {
          LedOn(WHITE);
          LedOn(PURPLE);
          LedOn(BLUE);
          LedOn(CYAN);
          LedOn(GREEN);
          LedOn(YELLOW);
          LedOn(ORANGE);
          LedOn(RED);
      }
      if(s8RssiChannel0 > -50 || s8RssiChannel0 == -50)
      {
        LedOn(WHITE);
        LedOn(PURPLE);
        LedOn(BLUE);
        LedOn(CYAN);
        LedOn(GREEN);
        LedOn(YELLOW);
        LedOn(ORANGE);
        LedOn(RED);
        s8RssiChannel0=-99;
      }
    }

  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_JudgeButton;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "B1 CommonMode B2 Los");
    LCDMessage(LINE2_START_ADDR, "eWeight B3 FindMode ");
    LedOff(ORANGE);
    LedOff(RED);
    LedOff(PURPLE);
    LedOff(CYAN);
    LedOff(YELLOW);
    LedOff(BLUE);
    LedOff(GREEN);
    LedOff(WHITE);
  }
}/**/



/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/