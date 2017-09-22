/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

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
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
 
  /* If good initialization, set state to state1 */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_state1;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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


/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1SM_state1()

Description:
Some Debug, LCD, LED and buzzer orders in the first state.
*/
static void UserApp1SM_state1(void)
{
  static bool bDebug1 = TRUE;
  static u8 au8InPut[1];
  u8 au8DebugOut1[] = "Entering state 1";
  u8 au8State1[] = "STATE 1";

  /* Display "Entering state 1" once in Tera Term */
  if(bDebug1)
  {
      DebugPrintf(au8DebugOut1);
      DebugLineFeed();
      bDebug1 = FALSE;
  }

  /*  Display "STATE 1" on the LCD screen and make the LCD screen purple */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8State1);
  LedOff(LCD_GREEN);
  LedPWM(LCD_RED, LED_PWM_50);
  LedPWM(LCD_BLUE, LED_PWM_50);

  /* Turn off the green, yellow, orange and red LEDs. Turn on the white, purple, blue and cyan LEDs */
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  LedOn(WHITE);
  LedOn(PURPLE);
  LedOn(BLUE);
  LedOn(CYAN);

  /* Turn off the buzzer */
  PWMAudioOff(BUZZER1);

  /* Change to state2 */
  DebugScanf(au8InPut);
  if(WasButtonPressed(BUTTON2))
  {
      ButtonAcknowledge(BUTTON2);
      UserApp1_StateMachine = UserApp1SM_state2;
  }

  if(au8InPut[0] == '2')
  {
      DebugLineFeed();
      UserApp1_StateMachine = UserApp1SM_state2;
  }

}/* end UserApp1SM_state1 */


/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1SM_state2()

Description:
Some Debug, LCD, LED and buzzer orders in the second state.
*/
static void UserApp1SM_state2(void)
{
  static bool bDebug2 = TRUE;
  static u8 au8InPut[1];
  static u16 u16Counter = 0;
  //static u16 u16Counter1 = 0;
  //static u16 u16Counter2 = 0;
  u8 au8DebugOut[] = "Entering state 2";
  u8 au8State[] = "STATE 2";

  u16Counter++;

  /* Display "Entering state 2" once in Tera Term */
  if(bDebug2)
  {
      DebugPrintf(au8DebugOut);
      DebugLineFeed();
      bDebug2 = FALSE;
  }

  /* Display "STATE 2" on the LCD screen and make the LCD screen orange */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8State);
  LedOff(LCD_BLUE);
  LedPWM(LCD_RED, LED_PWM_65);
  LedPWM(LCD_GREEN, LED_PWM_35);

  /* Turn off the white, purple, blue and cyan LEDs. Make the green, yellow, orange and red LEDs blink */
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedBlink(GREEN, LED_1HZ);
  LedBlink(YELLOW, LED_2HZ);
  LedBlink(ORANGE, LED_4HZ);
  LedBlink(RED, LED_8HZ);

  /* Let the buzzer ring at 200 Hz in 100ms and turn off it until 1s */
  if(u16Counter < 100)
  {
      PWMAudioOn(BUZZER1);
      PWMAudioSetFrequency(BUZZER1, 200);
  }
  else if(u16Counter > 100 && u16Counter < 1000)
  {
      PWMAudioOff(BUZZER1);
  }

      /*u16Counter2++;
      if(u16Counter2 == 900)
      {
          u16Counter2 = 0;
          u16Counter1++;
          PWMAudioOn(BUZZER1);
          PWMAudioSetFrequency(BUZZER1, 200);
          if(u16Counter1 == 100)
          {
              u16Counter1 = 0;
              //u16Counter2++;
              PWMAudioOff(BUZZER1);
          }
      }*/

  /* Change to state1 */
  DebugScanf(au8InPut);
  if(WasButtonPressed(BUTTON1))
  {
      ButtonAcknowledge(BUTTON1);
      UserApp1_StateMachine = UserApp1SM_state1;
  }

  if(au8InPut[0] == '1')
  {
      DebugLineFeed();
      UserApp1_StateMachine = UserApp1SM_state1;
  }

} /* end UserApp1SM_state2() */


#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
