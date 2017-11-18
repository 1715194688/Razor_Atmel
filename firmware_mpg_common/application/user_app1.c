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

static AntAssignChannelInfoType sAntSetupMaster;
static AntAssignChannelInfoType sAntSetupSlave;


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

  /* Master (Channel 2) */
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

 /* Configure ANT for this application */
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
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, ANT_CONFIGURE_TIMEOUT_MS) )
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, "Master config failed");
    UserApp1_StateMachine = UserApp1SM_Error;
  }




}















static void UserApp1SM_Slave(void)
{
  u8 au8Temp[7] = {'-',0,0,'d','B','m','\0'};
  u8 u8Temp;
  static bool bEnd = FALSE;
  static bool bStop = TRUE;
  static bool bStartCount = FALSE;
  u8 au8StartCounter[2] = {0,0};
  u8 u8StartCounter = 10;
  u8 u8BuzzerCount = 0;
  u8 u16msCounter = 0;
  u8 au8SlaveBeforeSeek[] = "Seeker";
  u8 au8SlaveStartSeek_1[] = "Ready or not";
  u8 au8SlaveStartSeek_2[] = "Here I come!";
  u8 au8SlaveSeeking[] = "Seeking";
  u8 au8SlaveEndSeek[] = "Found You!";

  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    bStartCount = TRUE;
    //AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8SlaveBeforeSeek);
  }
  /* Still useless */
  if(bStartCount)
  {
    u16msCounter++;
    if(u16msCounter == 1000)
    {
      u16msCounter = 0;
      u8StartCounter--;
      au8StartCounter[0] = u8StartCounter/10 + 48;
      au8StartCounter[1] = u8StartCounter%10 +48;
      LCDMessage(LINE2_START_ADDR, au8StartCounter);
    }
  }

    if(u8StartCounter == 0)
    {
      bStartCount = FALSE;
      u8StartCounter = 10;
      //u8StartCounter++;
      AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8SlaveStartSeek_1);
      LCDMessage(LINE2_START_ADDR, au8SlaveStartSeek_2);

      //PWMAudioSetFrequency(BUZZER1, 500);
      //PWMAudioOn(BUZZER1);
    }/* End useless */

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
        if(bStop)
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
        if(bStop)
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
        if(bStop)
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
        if(bStop)
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
        if(bStop)
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
        if(bStop)
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
        if(bStop)
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
        if(bStop)
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
      if(s8RssiChannel1 == -50)
      {
        bStop = FALSE;
        bEnd = TRUE;
        /*LedOff(WHITE);
        LedOff(PURPLE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(YELLOW);
        LedOff(ORANGE);
        LedOff(RED);*/

        //u8StartCounter++;

        //if(u8StartCounter == 5)
        //{
          //UserApp1_StateMachine = UserApp1SM_Master;
        //}
      }

      if(bEnd)
      {
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
    }
  }
}


/*static void UserApp1SM_Master(void)
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



}*/


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
