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
 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
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
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  u32 u32CounterLimit=500;                       /*Time limitation*/

  static u8 au8OurNames[]="Liu Rendi Wang Zeyun Chen Jiale      ";              /*Our names*/
  
  static u8 au8Line1[20];                       /*Store the message that should display on the line1*/
  static u8 au8Line2[20];                       /*Store the message that should display on the line2*/
  
  static u8 u8Adress1=0;                        /*Control where the message display*/
  static u8 u8Adress2=20;                       /*Control where the message display*/
  
  static u16 u16Counter=0;                      /*Record the time*/
  static u16 u16LedCounter=0;                   /*Record the time*/

  u16LedCounter++;
  u16Counter++;

  /*Assign values to the two arrays*/
  for(u8 i1=0; i1 < 20; i1++)
  {
      au8Line1[i1]=au8OurNames[i1+u8Adress1];
  }
  for(u8 i2=0; i2 < 20; i2++)
  {
      au8Line2[i2]=au8OurNames[i2+u8Adress2];
  }

  /*Slow down*/
  if(WasButtonPressed(BUTTON0))
  {
      ButtonAcknowledge(BUTTON0);
      u32CounterLimit = u32CounterLimit/2;
  }

  /*Speed up*/
  if(WasButtonPressed(BUTTON1))
  {
      ButtonAcknowledge(BUTTON1);
      u32CounterLimit = u32CounterLimit*2;
  }

  /*When the time is equal to the time limitation, do the display operation*/
  if(u16Counter == u32CounterLimit)
  {
      u16Counter=0;
      u8Adress1++;
      u8Adress2++;
      if(u8Adress2 > 50)
      {
          u8Adress1=0;
          u8Adress2=20;
      }

      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8Line1);
      LCDMessage(LINE2_START_ADDR, au8Line2);
  }

  /*Some simple LED and LCD effects*/
  if(u16LedCounter == 100)
  {
      LedOff(WHITE);
      LedOn(RED);
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 200)
  {
      LedOn(ORANGE);
      LedOff(RED);
      LedOn(LCD_RED);
      LedOff(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 300)
  {
      LedOn(YELLOW);
      LedOff(ORANGE);
      LedOff(LCD_RED);
      LedOff(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 400)
  {
      LedOn(GREEN);
      LedOff(YELLOW);
      LedOff(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 500)
  {
      LedOn(CYAN);
      LedOff(GREEN);
      LedOff(LCD_RED);
      LedOn(LCD_GREEN);
      LedOff(LCD_BLUE);
  }
  if(u16LedCounter == 600)
  {
      LedOn(BLUE);
      LedOff(CYAN);
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOff(LCD_BLUE);
  }
  if(u16LedCounter == 700)
  {
      LedOn(PURPLE);
      LedOff(BLUE);
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOff(LCD_BLUE);
  }
  if(u16LedCounter == 800)
  {
      LedOn(WHITE);
      LedOff(PURPLE);
      LedOn(LCD_RED);
      LedOff(LCD_GREEN);
      LedOff(LCD_BLUE);
  }
  if(u16LedCounter == 900)
  {
      LedOn(PURPLE);
      LedOff(WHITE);
      LedOn(LCD_RED);
      LedOff(LCD_GREEN);
      LedOff(LCD_BLUE);
  }
  if(u16LedCounter == 1000)
  {
      LedOn(BLUE);
      LedOff(PURPLE);
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 1100)
  {
      LedOn(CYAN);
      LedOff(BLUE);
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 1200)
  {
      LedOn(GREEN);
      LedOff(CYAN);
      LedOn(LCD_RED);
      LedOff(LCD_GREEN);
      LedOff(LCD_BLUE);
  }
  if(u16LedCounter == 1300)
  {
      LedOn(YELLOW);
      LedOff(GREEN);
      LedOff(LCD_RED);
      LedOff(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 1400)
  {
      LedOn(ORANGE);
      LedOff(YELLOW);
      LedOff(LCD_RED);
      LedOn(LCD_GREEN);
      LedOn(LCD_BLUE);
  }
  if(u16LedCounter == 1500)
  {
      LedOn(RED);
      LedOff(ORANGE);
      LedOn(LCD_RED);
      LedOn(LCD_GREEN);
      LedOff(LCD_BLUE);
      u16LedCounter=0;
  }


} /* end UserApp1SM_Idle() */
    
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
