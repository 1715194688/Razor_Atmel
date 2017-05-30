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


/*----------------------------------------------------------------------------------------------------
Function UserApp1GeneralModule()

Description:
Call different functions.
*/
static void UserApp1GeneralModule(void)
{
  /*Determine whether the program is running*/
  static bool bWhetherStart = TRUE;
  u8 au8TopOutput[] = "******************************************************\n\rLED Programming Interface\n\rrPress 1 to program Led command sequence\n\rrPress 2 to show current USER program\n\r******************************************************\n\r";
  /*Store the input content*/
  static u8 au8InputData[100];

  if(bWhetherStart)
  {
      DebugPrintf(au8TopOutput);
      bWhetherStart = FALSE;
  }

  /*Determine which function the user uses*/
  if(G_u8DebugScanfCharCount == 1)
  {
      DebugScanf(au8InputData);

      /*Call the UserApp1Press1Module after pressing 1*/
      if(au8InputData[0] == '1')
      {
          DebugLineFeed();
          LedDisplayStartList();
          UserApp1_StateMachine = UserApp1Press1Module;
      }

      /*Call the UserApp1Press2Module after pressing 2*/
      if(au8InputData[0] == '2')
      {
          DebugLineFeed();
          UserApp1_StateMachine = UserApp1Press2Module;
      }

      /*Remind the user to press 1 or 2*/
      else if(au8InputData[0] != '1' && au8InputData[0] != '2')
      {
          DebugLineFeed();
          DebugPrintf("What you input is invalid, press 1 or 2 please");
          DebugLineFeed();
      }
  }
}


/*----------------------------------------------------------------------------------------------------
Function UserApp1Press1Module()

Description:
Help the user input commands they want.
*/
static void UserApp1Press1Module(void)
{
  static bool bWhetherStart = TRUE;
  static bool bStartInput = TRUE;
  static bool bEndInput = FALSE;
  static u8 u8Index1 = 2;
  static u8 u8Index2 = 2;
  static u32 u32TurnOnTime = 0;
  static u32 u32TurnOffTime = 0;
  LedCommandType eYourCommand;

  if(bWhetherStart)
  {
      DebugLineFeed();
      DebugLineFeed();
      /*Describe the rule of the command*/
      DebugPrintf("Enter commands as LED-ONTIME-OFFTIME and press Enter\n\rTime is in milliseconds, max 100commands\n\rLED colors:R,O,Y,G,C,B,P,W\n\rExample:R-100-200(Red on at 100ms and off at 200 ms)\n\rPress Enter on blank line to end\n\r1:");
      bWhetherStart = FALSE;
  }

  if(G_u8DebugScanfCharCount == 1)
  {
      if(G_au8DebugScanfBuffer[0] == 'R' || G_au8DebugScanfBuffer[0] == 'r')
      {
          eYourCommand.eLED = RED;
      }

      if(G_au8DebugScanfBuffer[0] == 'O' || G_au8DebugScanfBuffer[0] == 'o')
      {
          eYourCommand.eLED = ORANGE;
      }

      if(G_au8DebugScanfBuffer[0] == 'Y' || G_au8DebugScanfBuffer[0] == 'y')
      {
          eYourCommand.eLED = YELLOW;
      }

      if(G_au8DebugScanfBuffer[0] == 'G' || G_au8DebugScanfBuffer[0] == 'g')
      {
          eYourCommand.eLED = GREEN;
      }

      if(G_au8DebugScanfBuffer[0] == 'C' || G_au8DebugScanfBuffer[0] == 'c')
      {
          eYourCommand.eLED = CYAN;
      }

      if(G_au8DebugScanfBuffer[0] == 'B' || G_au8DebugScanfBuffer[0] == 'b')
      {
          eYourCommand.eLED = BLUE;
      }

      if(G_au8DebugScanfBuffer[0] == 'P' || G_au8DebugScanfBuffer[0] == 'p')
      {
          eYourCommand.eLED = PURPLE;
      }

      if(G_au8DebugScanfBuffer[0] == 'W' || G_au8DebugScanfBuffer[0] == 'w')
      {
          eYourCommand.eLED = WHITE;
      }
  }

  /*Set the moment when the LED lights are turned on*/
  if(bStartInput)
  {
      if(G_u8DebugScanfCharCount >= 3)
      {
          if(G_au8DebugScanfBuffer[u8Index1] == '-')
          {
              eYourCommand.bOn = TRUE;
              eYourCommand.u32Time = u32TurnOnTime;
              eYourCommand.eCurrentRate = LED_PWM_0;
              LedDisplayAddCommand(USER_LIST, &eYourCommand);
              bStartInput = FALSE;
              bEndInput = TRUE;
              u8Index2 = u8Index1;
          }

          else
          {
              if(u8Index2 == u8Index1)
              {
                  /*Get the TurnOnTime number you input*/
                  u32TurnOnTime = u32TurnOnTime*10 + (G_au8DebugScanfBuffer[u8Index1]-'0');
                  u8Index1++;
              }
              u8Index2 = G_u8DebugScanfCharCount - 1;
          }
      }
  }

  if(bEndInput)
  {
      if(G_au8DebugScanfBuffer[u8Index1] != '\r')
      {
          if(G_au8DebugScanfBuffer[u8Index1] != '-')
          {
              if(u8Index1 == u8Index2)
              {
                  u32TurnOffTime = u32TurnOffTime*10+(G_au8DebugScanfBuffer[u8Index1]-'0');
                  u8Index1++;
              }
              u8Index2 = G_u8DebugScanfCharCount - 1;
          }

          else
          {
              u8Index1++;
          }
      }

      else
      {
          eYourCommand.bOn = FALSE;
          eYourCommand.u32Time = u32TurnOffTime;
          eYourCommand.eCurrentRate = LED_PWM_100;
          LedDisplayAddCommand(USER_LIST, &eYourCommand);
          /*Reset the time*/
          u32TurnOnTime = 0;
          u32TurnOffTime = 0;
          /*Reset the index*/
          u8Index1 = 2;
          u8Index2 = 2;
          /*Empty the buffer*/
          for(u8 i = 0; i<G_u8DebugScanfCharCount; i++)
          {
              G_au8DebugScanfBuffer[i] = '\0';
          }
          /*Reset the char counter*/
          G_u8DebugScanfCharCount = 0;
          bStartInput = TRUE;
          bEndInput = FALSE;
          DebugLineFeed();
          DebugPrintNumber(u8Number);
          DebugPrintf(":"); 
          u8Number++;
      }
  }
  
  if(G_au8DebugScanfBuffer[0] =='\r')
  {
      UserApp1_StateMachine = UserApp1Module;
  }
  else
  {
      G_au8DebugScanfBuffer[0] = '\0';
  }
}


/*----------------------------------------------------------------------------------------------------
Function UserApp1Press2Module()

Description:
Help to see the current program.
*/
static void UserApp1Press2Module(void)
{
  static bool bWhetherStart = TRUE;
  static u8 au8InputData[100];
  u8 u8EntryCounter = 0;

  /*Show the user's commands they input*/
  if(bWhetherStart)
  {
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("Current USER Program:");
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("LED   ON TIME   OFF TIME\n\r------------------------\n\r");
      while( LedDisplayPrintListLine(u8EntryCounter++) )
      DebugPrintf("\n\r----------------------\n\r");
      DebugPrintf("Press 1 to Program\n\r");
      bWhetherStart = FALSE;
  }

  /*Return to the input interface*/
  if(G_u8DebugScanfCharCount == 1)
  {
      DebugScanf(au8InputData);
      if(au8InputData[0] == '1')
      {
          UserApp1_StateMachine = UserApp1Press1Module;
      }
  }
}


/*----------------------------------------------------------------------------------------------------
Function UserApp1Module()
Description:
Help the user see new commands they input.
*/
static void UserApp1Module(void)
{
  static bool bWhetherStart = TRUE;
  static u8 au8InputData[100];
  u8 u8EntryCounter = 0;

  if(bWhetherStart)
  {
      DebugLineFeed();
      DebugPrintf("Command entry complete.\n\rCommand entered:");
      DebugPrintNumber(u8Number - 2);
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("New USER program:");
      DebugLineFeed();
      DebugLineFeed();
      DebugPrintf("LED   ON TIME   OFF TIME\n\r------------------------\n\r");
      while( LedDisplayPrintListLine(u8EntryCounter++) );
      DebugPrintf("\n\r------------------------\n\r");

      /*Empty the buffer*/
      for(u8 i = 0; i < G_u8DebugScanfCharCount; i++)
      {
          G_au8DebugScanfBuffer[i] = '\0';
      }
      /*Reset the char counter*/
      G_u8DebugScanfCharCount = 0;
      bWhetherStart = FALSE;
  }

  if(G_u8DebugScanfCharCount == 1)
  {
      DebugScanf(au8InputData);
      if(au8InputData[0] == '1')
      {
          UserApp1_StateMachine = UserApp1Press2Module;
      }
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
