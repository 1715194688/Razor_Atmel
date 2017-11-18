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
static s8 s8RssiChannel2;                            /* Record the current RSSI level */
static u32 u32MasterMSGCounter = 0;

static AntAssignChannelInfoType sAntSetupMaster;
static AntAssignChannelInfoType sAntSetupSlave;
static u8 au8MasterSend[9] = "0\0\0\0\0\0\0\0";

static u8 au8TempMaster[9] = {'M',':','-',0,0,'d','B','m','\0'};
static u8 au8TempSlave[9] = {'S',':','-',0,0,'d','B','m','\0'};
static u8 u8TempMaster;
static u8 u8TempSlave;
u8 au8WelcomeMessage[] = "Hide and Go Seek!";
u8 au8Instructions[] = "Press B0 to Start";


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
  //AntAssignChannelInfoType sAntSetupData;
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
  LCDMessage(LINE2_START_ADDR, au8Instructions);

  /* Master (Channel2) */
  sAntSetupMaster.AntChannel = ANT_CHANNEL_2;
  sAntSetupMaster.AntChannelType = CHANNEL_TYPE_MASTER;
  sAntSetupMaster.AntChannelPeriodLo = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupMaster.AntChannelPeriodHi = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupMaster.AntDeviceIdLo = ANT_DEVICEID_LO_USERAPP;
  sAntSetupMaster.AntDeviceIdHi = ANT_DEVICEID_HI_USERAPP;
  sAntSetupMaster.AntDeviceType = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupMaster.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  
  sAntSetupMaster.AntFrequency = ANT_FREQUENCY_USERAPP;
  sAntSetupMaster.AntTxPower = ANT_TX_POWER_USERAPP;
  sAntSetupMaster.AntNetwork = ANT_NETWORK_DEFAULT;

 /* Slave (Channel1) */
  sAntSetupSlave.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupSlave.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupSlave.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupSlave.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupSlave.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupSlave.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupSlave.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupSlave.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  
  sAntSetupSlave.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupSlave.AntTxPower          = ANT_TX_POWER_USERAPP;
  sAntSetupSlave.AntNetwork = ANT_NETWORK_DEFAULT;
  
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupMaster.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    sAntSetupSlave.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  AntAssignChannel(&sAntSetupMaster);
  UserApp1_u32Timeout = G_u32SystemTime1ms;
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_AntConfigureMaster;
  }
  else
  {
    /* Stop running */
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
static void UserApp1SM_AntConfigureMaster(void)
{
  /* Judge if the channel has been confrigured */
  if(AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_CONFIGURED)
  {
    /* Queue configuration of Slave channel */
    AntAssignChannel(&sAntSetupSlave);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntConfigureSlave;
  }
  
  /* Check if it is times out */
  if( IsTimeUp(&UserApp1_u32Timeout, 2000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Master config failed");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}


static void UserApp1SM_AntConfigureSlave(void)
{
  /* Judge if the channel has been configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CONFIGURED)
  {
    /* Update the broadcast message data */
    AntQueueBroadcastMessage(ANT_CHANNEL_2, au8MasterSend);
    AntQueueBroadcastMessage(ANT_CHANNEL_1, au8MasterSend);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check if it is times out */
  if( IsTimeUp(&UserApp1_u32Timeout, 2000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Slave config failed");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}


static void UserApp1SM_Idle(void)
{
  u8 au8ChooseStatus[] = "Choose one status";
  u8 au8ChoosePress[] = "B1 Master   B2 Slave";

  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    
    /* Open the channels and choose one identity */
    AntOpenChannelNumber(ANT_CHANNEL_2);
    AntOpenChannelNumber(ANT_CHANNEL_1);
    
    UserApp1_u32Timeout = G_u32SystemTime1ms;

    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8ChooseStatus);
    LCDMessage(LINE2_START_ADDR, au8ChoosePress);
  }
  if(WasButtonPressed(BUTTON1))
  {
    UserApp1_StateMachine = UserApp1SM_OpenToMaster;
  }
  if(WasButtonPressed(BUTTON2))
  {
    UserApp1_StateMachine = UserApp1SM_OpenToSlave;
  }
}


static void UserApp1SM_OpenToMaster(void)
{
  u8 au8Master[] = "Hider";
  /* Ensure that both channels have opened */
  if( (AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_OPEN) && (AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_OPEN) )
  {
    /* Update LCD and go to main Radio monitoring state */
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Master);
    au8TempMaster[3] = u8TempMaster/10 + 48;
    au8TempMaster[4] = u8TempMaster%10 + 48;
    LCDMessage(LINE1_END_ADDR-8, au8TempMaster);
    LCDMessage(LINE2_START_ADDR, au8MasterSend);

    UserApp1_StateMachine = UserApp1SM_MasterPlaying;
  }

  /* Check if it times out */
  if( IsTimeUp(&UserApp1_u32Timeout, 2000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Fail to Open channel");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}


static void UserApp1SM_OpenToSlave(void)
{
  u8 au8Slave[] = "Seeker";
  /* Ensure that both channels have opened */
  if( (AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_OPEN) && (AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_OPEN) )
  {
    /* Update LCD and go to main Radio monitoring state */
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Slave);
    au8TempSlave[3] = u8TempSlave/10 + 48;
    au8TempSlave[4] = u8TempSlave%10 + 48;
    LCDMessage(LINE1_END_ADDR-8, au8TempSlave);
    LCDMessage(LINE2_START_ADDR, au8MasterSend);
    
    UserApp1_StateMachine = UserApp1SM_SlavePlaying;
  }

  /* Check if it times out */
  if( IsTimeUp(&UserApp1_u32Timeout, 2000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Fail to Open channel");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}


static void UserApp1SM_MasterPlaying(void)
{
  u8 u8EventCode2;
  static bool bEnd2 = FALSE;
  static bool bStop2 = TRUE;
  u8 au8EndFound[] = "You found me!";

  /*if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8SlaveBeforeSeek);
  }*/

  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      /* Get the EVENT code */
      u8EventCode2 = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];

      if(G_sAntApiCurrentMessageExtData.u8Channel == 2)
      {
        if(u8EventCode2 == EVENT_TX)
        {
          u32MasterMSGCounter++;
          if(u32MasterMSGCounter >= 8)
          {
            s8RssiChannel2 = -99;
            au8TempMaster[3] = 0;
            au8TempMaster[4] = 0;
          }
        }
      }
    }

    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* Check the channel number */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 2)
      {
        /* Reset the message counter */
        u32MasterMSGCounter = 0;
        
        /* Record RSSI */
        s8RssiChannel2 = G_sAntApiCurrentMessageExtData.s8RSSI;
        u8TempMaster = abs(s8RssiChannel2);
        au8TempMaster[3] = u8TempMaster/10 + 48;
        au8TempMaster[4] = u8TempMaster%10 + 48;
        LCDMessage(LINE1_END_ADDR-8, au8TempMaster);
      }
    }

      if(s8RssiChannel2 < -90)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -90 || s8RssiChannel2 == -90)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -85 || s8RssiChannel2 == -85)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -80 || s8RssiChannel2 == -80)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -75 || s8RssiChannel2 == -75)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -70 || s8RssiChannel2 == -70)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -65 || s8RssiChannel2 == -65)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -60 || s8RssiChannel2 == -60)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel2 > -55 || s8RssiChannel2 == -55)
      {
        if(bStop2)
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
      }
      if(s8RssiChannel1 > -50 || s8RssiChannel1 == -50)
      {
        bStop2 = FALSE;
        bEnd2 = TRUE;
      }
  }

      if(bEnd2)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, au8EndFound);
        LedBlink(WHITE, LED_2HZ);
        LedBlink(PURPLE, LED_2HZ);
        LedBlink(BLUE, LED_2HZ);
        LedBlink(CYAN, LED_2HZ);
        LedBlink(GREEN, LED_2HZ);
        LedBlink(YELLOW, LED_2HZ);
        LedBlink(ORANGE, LED_2HZ);
        LedBlink(RED, LED_2HZ);
      }

  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);

    /* Turn off all the LEDs */
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    
    /* Back to the start */
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
    LCDMessage(LINE2_START_ADDR, au8Instructions);
    
    /* Close channels */
    AntCloseChannelNumber(ANT_CHANNEL_2);
    AntCloseChannelNumber(ANT_CHANNEL_1);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_CloseChannels;
  }
}


static void UserApp1SM_SlavePlaying(void)
{
  u8 u8EventCode1;
  static bool bStop1 = TRUE;
  static bool bEnd1 = FALSE;
  u8 au8EndSeek[] = "Found you!";

  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      /* Get the EVENT code */
      u8EventCode1 = G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX];
      if(G_sAntApiCurrentMessageExtData.u8Channel == 1)
      {
        /* Check the Event code */
        if (u8EventCode1 == EVENT_RX_FAIL_GO_TO_SEARCH)
        {
            s8RssiChannel1 = -99;
            au8TempSlave[3] = 0;
            au8TempSlave[4] = 0;
        }
      }
    }

    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      if(G_sAntApiCurrentMessageExtData.u8Channel == 1)
      {
        /* Queue a response message */
        AntQueueBroadcastMessage(ANT_CHANNEL_1, au8MasterSend);

        /* Record RSSI */
        s8RssiChannel1 = G_sAntApiCurrentMessageExtData.s8RSSI;
        u8TempSlave = abs(s8RssiChannel1);
        au8TempSlave[3] = u8TempSlave/10 + 48;
        au8TempSlave[4] = u8TempSlave%10 + 48;
        LCDMessage(LINE1_END_ADDR-8, au8TempSlave);
      }
    }

      if(s8RssiChannel1 < -90)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -90 || s8RssiChannel1 == -90)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -85 || s8RssiChannel1 == -85)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -80 || s8RssiChannel1 == -80)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -75 || s8RssiChannel1 == -75)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -70 || s8RssiChannel1 == -70)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -65 || s8RssiChannel1 == -65)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -60 || s8RssiChannel1 == -60)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -55 || s8RssiChannel1 == -55)
      {
        if(bStop1)
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
      }
      if(s8RssiChannel1 > -55 || s8RssiChannel1 == -50)
      {
        bStop1 = FALSE;
        bEnd1 = TRUE;
      }
  }

      if(bEnd1)
      {
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, au8EndSeek);
        LedBlink(WHITE, LED_2HZ);
        LedBlink(PURPLE, LED_2HZ);
        LedBlink(BLUE, LED_2HZ);
        LedBlink(CYAN, LED_2HZ);
        LedBlink(GREEN, LED_2HZ);
        LedBlink(YELLOW, LED_2HZ);
        LedBlink(ORANGE, LED_2HZ);
        LedBlink(RED, LED_2HZ);
      }

  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    //LedOff(LCD_RED);
    //LedOff(LCD_BLUE);

    /* Turn off all the LEDs */
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    
    /* Back to the start */
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
    LCDMessage(LINE2_START_ADDR, au8Instructions);
    
    /* Close channels */
    AntCloseChannelNumber(ANT_CHANNEL_2);
    AntCloseChannelNumber(ANT_CHANNEL_1);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_CloseChannels;
  }
}


static void UserApp1SM_CloseChannels(void)
{
  /* Ensure that both channels have opened */
  if( (AntRadioStatusChannel(ANT_CHANNEL_2) == ANT_CLOSED) &&
      (AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CLOSED) )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }

  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 2000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Channel close failed");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
