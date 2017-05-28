/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 u8Number = 2;

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
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  
    /* If good initialization, set state to General Function */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1GeneralModule;
  }
  else
  {

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
/* Wait for input */
static void UserApp1SM_Idle(void)
{
  
} /* end UserApp1SM_Idle() */


/*The total function*/
static void UserApp1GeneralModule(void)
{
  /*Determine whether the program is running*/
  static bool bWhetherStart=TRUE;
  u8 au8TopOutput[]="******************************************************\n\rLED Programming Interface\n\rrPress 1 to program Led command sequence\n\rrPress 2 to show current USER program\n\r******************************************************";
  /*Store the input content*/
  static u8 au8InputData[100];

  if(bWhetherStart)
  {
      DebugPrintf(au8TopOutput);
      bWhetherStart=FALSE;
  }

  /*Determine which function the user uses*/
  if(G_u8DebugScanfCharCount == 1)
  {
      DebugScanf(au8InputData);

      if(au8InputData[0] == 1)
      {
          DebugLineFeed();
          LedDisplayStartList();
          /*Call the UserApp1Press1Function after pressing 1*/
          UserApp1_StateMachine = UserApp1Press1Module;
      }

      if(au8InputData[0] == 2)
      {
          DebugLineFeed();
          /*Call the UserApp1Press2Function after pressing 2*/
          UserApp1_StateMachine = UserApp1Press2Module;
      }
      else
      {
          DebugLineFeed();
          DebugPrintf("What you input is invalid, press 1 or 2 please");
          DebugLineFeed();
      }
  }
}


static void UserApp1Press1Module(void)
{
  static u8 au8InputData[100];
  static bool bWhetherStart=TRUE;
  static bool bStartInput=TRUE;
  static bool bEndInput=FALSE;
  static u8 u8Index1=2;
  static u8 u8Index2=2;
  static u32 u32TurnOnTime=0;
  static u32 u32TurnOffTime=0;
  LedCommandType eCommand;

  if(bWhetherStart)
  {
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("Enter commands as LED-ONTIME-OFFTIME and press Enter\n\rTime is in milliseconds, max 100commands\n\rLED colors:R,O,Y,G,C,B,P,W\n\rExample:R-100-200(Red on at 100ms and off at 200 ms)\n\rPress Enter on blank line to end\n\r");
      bWhetherStart=FALSE;
  }

  if(G_u8DebugScanfCharCount == 1)
  {
      if(G_au8DebugScanfBuffer[0] == 'R')
      {
          eCommand.eLED=RED;
      }

      if(G_au8DebugScanfBuffer[0] == 'O')
      {
          eCommand.eLED=ORANGE;
      }

      if(G_au8DebugScanfBuffer[0] == 'Y')
      {
          eCommand.eLED=YELLOW;
      }

      if(G_au8DebugScanfBuffer[0] == 'G')
      {
          eCommand.eLED=GREEN;
      }

      if(G_au8DebugScanfBuffer[0] == 'C')
      {
          eCommand.eLED=CYAN;
      }

      if(G_au8DebugScanfBuffer[0] == 'B')
      {
          eCommand.eLED=BLUE;
      }

      if(G_au8DebugScanfBuffer[0] == 'P')
      {
          eCommand.eLED=PURPLE;
      }

      if(G_au8DebugScanfBuffer[0] == 'W')
      {
          eCommand.eLED=WHITE;
      }
  }

  if(bStartInput)
  {
      DebugScanf(au8InputData);
      if(G_u8DebugScanfCharCount>2)
      {
          if(G_au8DebugScanfBuffer[u8Index1] == '-')
          {
              eCommand.bOn=TRUE;
              eCommand.u32Time=u32TurnOnTime;
              eCommand.eCurrentRate=LED_PWM_0;
              LedDisplayAddCommand(USER_LIST, &eCommand);
              bStartInput=FALSE;
              bEndInput=TRUE;
              u8Index2=u8Index1;
          }

          else
          {
              if(u8Index2 == u8Index1)
              {
                  u32TurnOnTime=u32TurnOnTime*10+(G_au8DebugScanfBuffer[u8Index1]-'0');
                  u8Index1++;
              }
          }
          u8Index2=G_u8DebugScanfCharCount-1;
          
      }
  }

  if(bEndInput)
  {
      if(G_au8DebugScanfBuffer[u8Index1] == '\r')
      {
          if(G_au8DebugScanfBuffer[u8Index1] == '-')
          {
              u8Index1++;
              
              if(u8Index1 == u8Index2)
              {
                  u32TurnOffTime=u32TurnOffTime*10+(G_au8DebugScanfBuffer[u8Index1]-'0');
                  u8Index1++;
              }

              u8Index2=G_u8DebugScanfCharCount-1;
          }

          else
          {
              u8Index1++;
          }
      }

      else
      {
          eCommand.bOn = FALSE;
          eCommand.u32Time = u32TurnOffTime;
          eCommand.eCurrentRate = LED_PWM_100;
          LedDisplayAddCommand(USER_LIST, &eCommand);
          u32TurnOnTime=0;
          u32TurnOffTime=0;
          u8Index1=2;
          u8Index2=2;
          for(u8 i=0;i<G_u8DebugScanfCharCount;i++)
          {
              G_au8DebugScanfBuffer[i] = '\0';
          }
          G_u8DebugScanfCharCount=0;
          bStartInput=TRUE;
          bEndInput=FALSE;
          DebugLineFeed();
          DebugPrintNumber(u8Number);
          DebugPrintf(":"); 
          u8Number++;
      }
  }


}







static void UserApp1Press2Module(void)
{
  static bool bWhetherStart=TRUE;
  static u8 au8InputData[100];
  u8 u8EntryCounter=0;

  if(bWhetherStart)
  {
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("Current USER Program:");
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("LED   ON TIME   OFF TIME\n\r------------------------\n\r");
      while(LedDisplayListLine(u8EntryCounter++))
      DebugPrintf("\n\r------------------------\n\r");
      DebugPrintf("Press 1 to Program");

      bWhetherStart=FALSE;
  }

  if(G_u8DebugScanfCharCount == 1)
  {
      DebugScanf(au8InputData);
      
      if(au8InputData[0] == 1)
      {
          UserApp1_StateMachine = UserApp1Press1Module;
      }
  }

}



static void UserApp1Module(void)
{
  static bool bWhetherStart=TRUE;
  static u8 au8InputData[100];
  u8 u8EntryCounter=0;

  if(bWhetherStart)
  {
      DebugLineFeed();
      DebugPrintf("Command entry complete.\n\rCommand entered:");
      DebugPrintNumber(u8CommandCount);
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("New USER program:");
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("LED   ON TIME   OFF TIME\n\r------------------------\n\r");
      while(LedDisplayListLine(u8EntryCounter++))
      DebugPrintf("\n\r------------------------\n\r");
      
      
  }







}





                      
            
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
