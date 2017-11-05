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
//static u32 UserApp1_u32DataMsgCount = 0;             /* Counts the number of ANT_DATA packets received */
//static u32 UserApp1_u32TickMsgCount = 0;             /* Counts the number of ANT_TICK packets received */

static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */

static s8 s8RssiChannel1;                            /* Record the current RSSI level */


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
  u8 au8WelcomeMessage[] = "Hide and Go Seek!";
  u8 au8Instructions[] = "Press B0 to Start";
  AntAssignChannelInfoType sAntSetupData;
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
  LCDMessage(LINE2_START_ADDR, au8Instructions);

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
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel assignment is queued so start timer */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    //LedOn(RED);

    UserApp1_StateMachine = UserApp1SM_Slave;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    //LedBlink(RED, LED_4HZ);

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
static void UserApp1SM_Slave(void)
{
  u8 au8Temp[7] = {'-',0,0,'d','B','m','\0'};
  u8 u8Temp;
  u8 u8StartCounter = 0;
  u8 au8SlaveBeforeSeek[] = "Seeker";
  u8 au8SlaveStartSeek_1[] = "Ready or not";
  u8 au8SlaveStartSeek_2[] = "Here I come!";
  u8 au8SlaveSeeking[] = "Seeking";
  u8 au8SlaveEndSeek[] = "Found You!";

  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    //u8StartCounter++;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8SlaveBeforeSeek);

    /*if(u8StartCounter == 10)
    {
      u8StartCounter = 0;
      u8StartCounter++;
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8SlaveStartSeek_1);
      LCDMessage(LINE2_START_ADDR, au8SlaveStartSeek_2);

      //PWMAudioSetFrequency(BUZZER1, 500);
      //PWMAudioOn(BUZZER1);

      if(u8StartCounter == 2)
      {
        u8StartCounter = 0;
        PWMAudioOff(BUZZER1);
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, au8SlaveSeeking);
      }
    }*/
  }

    if(AntReadAppMessageBuffer())
    {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        s8RssiChannel1 = G_sAntApiCurrentMessageExtData.s8RSSI;
        u8Temp = abs(s8RssiChannel1);
        au8Temp[1] = u8Temp/10 + 48;
        au8Temp[2] = u8Temp%10 + 48;
        LCDMessage(LINE1_END_ADDR-6, au8Temp);

        if(s8RssiChannel1 > -90 || s8RssiChannel1 == -90)
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
        if(s8RssiChannel1 > -85 || s8RssiChannel1 == -85)
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
        if(s8RssiChannel1 > -80 || s8RssiChannel1 == -80)
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
        if(s8RssiChannel1 > -75 || s8RssiChannel1 == -75)
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
        if(s8RssiChannel1 > -70 || s8RssiChannel1 == -70)
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
        if(s8RssiChannel1 > -65 || s8RssiChannel1 == -65)
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
        if(s8RssiChannel1 > -60 || s8RssiChannel1 == -60)
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
        if(s8RssiChannel1 > -55 || s8RssiChannel1 == -55)
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
        if(s8RssiChannel1 == -50)
        {
          //u8StartCounter++;
          /*LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR, au8SlaveEndSeek);
          LedBlink(WHITE, LED_2HZ);
          LedBlink(PURPLE, LED_2HZ);
          LedBlink(BLUE, LED_2HZ);
          LedBlink(CYAN, LED_2HZ);
          LedBlink(GREEN, LED_2HZ);
          LedBlink(YELLOW, LED_2HZ);
          LedBlink(ORANGE, LED_2HZ);
          LedBlink(RED, LED_2HZ);*/
          //if(u8StartCounter == 5)
          //{
            UserApp1_StateMachine = UserApp1SM_Master;
          //}
        }

      }


    }

}


static void UserApp1SM_Master(void)
{
u8 au8SlaveEndSeek[] = "Found You!";
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR, au8SlaveEndSeek);
          LedBlink(WHITE, LED_2HZ);
          LedBlink(PURPLE, LED_2HZ);
          LedBlink(BLUE, LED_2HZ);
          LedBlink(CYAN, LED_2HZ);
          LedBlink(GREEN, LED_2HZ);
          LedBlink(YELLOW, LED_2HZ);
          LedBlink(ORANGE, LED_2HZ);
          LedBlink(RED, LED_2HZ);



}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
