/**********************************************************************************************************************
File: user_app.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
1. Follow the instructions at the top of user_app.c
2. Use ctrl-h to find and replace all instances of "user_app" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for yournewtaskname.c

**********************************************************************************************************************/

#ifndef __USER_APP_H
#define __USER_APP_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Required constants for ANT channel configuration */
#define ANT_CHANNEL_USERAPP             ANT_CHANNEL_0         /* Channel 0 - 7 */
#define ANT_CHANNEL_TYPE_USERAPP        CHANNEL_TYPE_SLAVE    /* ANT SLAVE */
#define ANT_DEVICEID_LO_USERAPP         (u8)0                 /* Low byte of two-byte Device # */
#define ANT_DEVICEID_HI_USERAPP         (u8)0                 /* High byte of two-byte Device # */
#define ANT_DEVICE_TYPE_USERAPP         (u8)(0x78)            /* 1 - 255 */
#define ANT_TRANSMISSION_TYPE_USERAPP   (u8)1                 /* 1-127 (MSB is pairing bit) */
#define ANT_CHANNEL_PERIOD_LO_USERAPP   (u8)0x86              /* Low byte of two-byte channel period 0x0001 - 0x7fff */
#define ANT_CHANNEL_PERIOD_HI_USERAPP   (u8)0x1F              /* High byte of two-byte channel period */
#define ANT_FREQUENCY_USERAPP           (u8)57                /* 2400MHz + this number 0 - 99 */
#define ANT_TX_POWER_USERAPP            RADIO_TX_POWER_4DBM   /* RADIO_TX_POWER_0DBM, RADIO_TX_POWER_MINUS5DBM, RADIO_TX_POWER_MINUS10DBM, RADIO_TX_POWER_MINUS20DBM */

#define AU8_ANT_PLUS_NETWORK_KEY        {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45}

#define TIMEOUT_VALUE                   (u32)2000             /* Maximum allowed timeout value for any transition state */

/* G_u32UserAppFlags */
#define _CHANNEL_SYNCED                 0x00000001            /* Set when channel is synced */


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_IfConfigured(void);
static void UserApp1SM_JudgeButton(void);
//static void UserApp1SM_IfChannelOpen0(void);
static void UserApp1SM_IfChannelOpen1(void);
static void UserApp1SM_IfChannelOpen2(void);
static void UserApp1SM_IfChannelOpen3(void);
static void UserApp1SM_CommonMode(void);
static void UserApp1SM_LoseWeightMode(void);
static void UserApp1SM_FindMode(void);


static void UserApp1SM_Error(void);         
static void UserApp1SM_FailedInit(void);        


#endif /* __USER_APP_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

