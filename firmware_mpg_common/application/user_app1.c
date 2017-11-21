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

static s8 s8RssiChannel1 = -99;                      /* Record the current RSSI level */
static s8 s8RssiChannel0 = -99;                      /* Record the current RSSI level */

static AntAssignChannelInfoType sAntSetupMaster;
static AntAssignChannelInfoType sAntSetupSlave;

static u8 au8MasterSend[9] = "0\0\0\0\0\0\0\0";

static u8 au8TempMaster[9] = {'M',':','-',0,0,'d','B','m','\0'};
static u8 au8TempSlave[9] = {'S',':','-',0,0,'d','B','m','\0'};
static u8 u8TempMaster;
static u8 u8TempSlave;

u8 au8WelcomeMessage[] = "Hide and Go Seek!";
u8 au8Instructions[] = "Press B0 to Start";

static bool bClear = TRUE;
static bool bStop0 = TRUE;
//static bool bStop1 = TRUE;


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
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
  LCDMessage(LINE2_START_ADDR, au8Instructions);

  /* Initialize Master */
  sAntSetupMaster.AntChannel = ANT_CHANNEL_USERAPP;
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

 /* Initialize Slave */
  sAntSetupSlave.AntChannel          = ANT_CHANNEL_0;
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
  /* If good initialization, set state to AntConfigureMaster */
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
All state machines have a TOTAL of 1ms to execute, so on average n state machines88p
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


/* Function UserApp1SM_AntConfigureSlave():
Check if it finishes configuring the slave channel
*/
static void UserApp1SM_AntConfigureMaster(void)
{
  /* Judge if the channel has been confrigured */
  if(AntRadioStatusChannel(ANT_CHANNEL_1) == ANT_CONFIGURED)
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
}/* End UserApp1SM_AntConfigureMaster() */



/* Function UserApp1SM_AntConfigureSlave():
Check if it finishes configuring the slave channel
*/
static void UserApp1SM_AntConfigureSlave(void)
{
  /* Judge if the channel has been configured */
  if(AntRadioStatusChannel(ANT_CHANNEL_0) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }

  /* Check if it is times out */
  if( IsTimeUp(&UserApp1_u32Timeout, 2000) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Slave config failed");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}/* End UserApp1SM_AntConfigureSlave() */


/* Function UserApp1SM_Idle():
Used to start the game and select identities
*/
static void UserApp1SM_Idle(void)
{
  u8 au8WelcomeMessage[] = "Hide and Go Seek!";
  u8 au8Instructions[] = "Press B0 to Start";

  u8 au8ChooseStatus[] = "Choose one status";
  u8 au8ChoosePress[] = "B1 Hider   B2 Seeker";

  s8RssiChannel0 = -99;
  s8RssiChannel1 = -99;

  if(bClear)
  {
    bClear = FALSE;
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8WelcomeMessage);
    LCDMessage(LINE2_START_ADDR, au8Instructions);
  }

  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);

    UserApp1_u32Timeout = G_u32SystemTime1ms;

    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8ChooseStatus);
    LCDMessage(LINE2_START_ADDR, au8ChoosePress);
  }
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    UserApp1_StateMachine = UserApp1SM_MasterCount;
  }
  if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    UserApp1_StateMachine = UserApp1SM_SlaveCount;
  }
}/* End UserApp1SM_Idle() */


/* Function UserApp1SM_SlaveCount():
Master: Game countdown
*/
static void UserApp1SM_MasterCount(void)
{
  bool bBack0 = TRUE;
  static u16 u16Counter0 = 5000;

  if(bBack0)
  {
    u16Counter0--;
    /*if(u16Counter0==9000)
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "9");
    }

    if(u16Counter0==8000)
    {
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "8");
    }

    if(u16Counter0==7000)
    {
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "7");
    }
    
    if(u16Counter0==6000)
    {
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "6");
    }
    
    if(u16Counter0==5000)
    {
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "5");
    }*/
    
    if(u16Counter0==4000)
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "4");
    }
    
    if(u16Counter0==3000)
    {
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "3");
    }

    if(u16Counter0==2000)
    {
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "2");
    }

    if(u16Counter0==1000)
    {
      LCDMessage(LINE1_START_ADDR, "Hider");
      LCDMessage(LINE2_START_ADDR, "1");
    }

    if(u16Counter0==0)
    {
      u16Counter0 = 10000;
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, "Catch Me!");
      AntOpenChannelNumber(ANT_CHANNEL_1);
      bBack0 = FALSE;

      UserApp1_StateMachine = UserApp1SM_MasterDelay;
    }
  }
}/* End UserApp1SM_SlaveCount() */


/* Function UserApp1SM_SlaveCount():
Slave: Game countdown
*/
static void UserApp1SM_SlaveCount(void)
{
  bool bBack1 = TRUE;
  static u16 u16Counter1 = 5000;

  if(bBack1)
  {
    /*u16Counter1--;
    if(u16Counter1==9000)
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "9");
    }

    if(u16Counter1==8000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "8");
    }

    if(u16Counter1==7000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "7");
    }

    if(u16Counter1==6000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "6");
    }

    if(u16Counter1==5000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "5");
    }*/

    if(u16Counter1==4000)
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "4");
    }

    if(u16Counter1==3000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "3");
    }

    if(u16Counter1==2000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "2");
    }

    if(u16Counter1==1000)
    {
      LCDMessage(LINE1_START_ADDR, "Seeker");
      LCDMessage(LINE2_START_ADDR, "1");
    }

    if(u16Counter1==0)
    {
      u16Counter1 = 10000;
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, "Here I come!");
      AntOpenChannelNumber(ANT_CHANNEL_0);
      bBack1 = FALSE;

      UserApp1_StateMachine = UserApp1SM_SlaveDelay;
    }
  }
}/* End UserApp1SM_SlaveCount() */


/* Function UserApp1SM_MasterDelay():
Set the delay to ensure the channel opens
*/
static void UserApp1SM_MasterDelay(void)
{
  static u16 u16DelayCounter1 = 3000;

  u16DelayCounter1--;
  if(u16DelayCounter1 == 0)
  {
    u16DelayCounter1 = 3000;
    UserApp1_StateMachine = UserApp1SM_MasterPlaying;
  }
}/* End UserApp1SM_MasterDelay() */


/* Function UserApp1SM_MasterDelay():
Set the delay to ensure the channel opens
*/
static void UserApp1SM_SlaveDelay(void)
{
  static u16 u16DelayCounter0 = 3000;

  u16DelayCounter0--;
  if(u16DelayCounter0 == 0)
  {
    u16DelayCounter0 = 3000;
    UserApp1_StateMachine = UserApp1SM_SlavePlaying;
  }
}/* End UserApp1SM_MasterDelay() */



/* Function UserApp1SM_SlavePlaying():
Go to this state after choosing master
*/
static void UserApp1SM_MasterPlaying(void)
{
  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      /* Check the channel number */
      if(G_sAntApiCurrentMessageExtData.u8Channel == 1)
      {
        /* Record RSSI */
        s8RssiChannel1 = G_sAntApiCurrentMessageExtData.s8RSSI;
        u8TempMaster = abs(s8RssiChannel1);
        au8TempMaster[3] = u8TempMaster/10 + 48;
        au8TempMaster[4] = u8TempMaster%10 + 48;
        LCDMessage(LINE1_END_ADDR-8, au8TempMaster);
      }
    }

      if(s8RssiChannel1 > -50 || s8RssiChannel1 == -50)
      {
        UserApp1_StateMachine = UserApp1SM_MasterBlink;
      }
  }

  /* End the game under whatever circumstances */
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

    /* Close the channel */
    AntCloseChannelNumber(ANT_CHANNEL_1);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
}/* End UserApp1SM_MasterPlaying() */


/* Function UserApp1SM_SlavePlaying():
Go to this state after choosing slave
*/
static void UserApp1SM_SlavePlaying(void)
{
  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      if(G_sAntApiCurrentMessageExtData.u8Channel == 0)
      {
        /* Queue a response message */
        AntQueueBroadcastMessage(ANT_CHANNEL_0, au8MasterSend);

        /* Record RSSI */
        s8RssiChannel0 = G_sAntApiCurrentMessageExtData.s8RSSI;
        u8TempSlave = abs(s8RssiChannel0);
        au8TempSlave[3] = u8TempSlave/10 + 48;
        au8TempSlave[4] = u8TempSlave%10 + 48;
        LCDMessage(LINE1_END_ADDR-8, au8TempSlave);
      }
    }

      if(s8RssiChannel0 < -90)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -90 || s8RssiChannel0 == -90)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -85 || s8RssiChannel0 == -85)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -80 || s8RssiChannel0 == -80)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -75 || s8RssiChannel0 == -75)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -70 || s8RssiChannel0 == -70)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -65 || s8RssiChannel0 == -65)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -60 || s8RssiChannel0 == -60)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -55 || s8RssiChannel0 == -55)
      {
        if(bStop0)
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
      if(s8RssiChannel0 > -50 || s8RssiChannel0 == -50)
      {
        bStop0 = FALSE;
        LedOff(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);
        UserApp1_StateMachine = UserApp1SM_SlaveBlink;
      }
  }

  /* End the game under whatever circumstances */
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

    /* Close the channel */
    AntCloseChannelNumber(ANT_CHANNEL_0);
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
}/* End UserApp1SM_SlavePlaying() */


/* Function UserApp1SM_MasterBlink():
After being found, blink the LEDs
*/
static void UserApp1SM_MasterBlink(void)
{
  static bool bMasterBlink = TRUE;
  u8 au8EndFound[] = "You found me!";
  static u16 u16OverCounter0 = 5000;

  u16OverCounter0--;

  if(bMasterBlink)
  {
    //bStop1 = TRUE;
    bMasterBlink = FALSE;
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

  if(u16OverCounter0 == 0)
  {
    bClear = TRUE;
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);

    AntCloseChannelNumber(ANT_CHANNEL_1);
  
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
}/* End UserApp1SM_MasterBlink() */


/* Function UserApp1SM_MasterBlink():
After finding, blink the LEDs
*/
static void UserApp1SM_SlaveBlink(void)
{
  static bool bSlaveBlink = TRUE;
  u8 au8EndSeek[] = "Found you!";
  static u16 u16OverCounter1 = 5000;

  u16OverCounter1--;

  if(bSlaveBlink)
  {
    bStop0 = TRUE;
    bSlaveBlink = FALSE;
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

 
 if(u16OverCounter1 == 0)
  {
    bClear = TRUE;
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);

    AntCloseChannelNumber(ANT_CHANNEL_0);
  
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
}/* End UserApp1SM_SlaveBlink() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
