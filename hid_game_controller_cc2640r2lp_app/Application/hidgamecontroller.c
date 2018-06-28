/******************************************************************************

 @file       hidgamecontroller.c

 @brief This file contains the HID emulated keyboard sample application for use
        with the CC2650 Bluetooth Low Energy
        Protocol Stack.

 Group: CMCU, SCS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2011-2017, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc2640r2_sdk_ble_example_pack_01_50_00_62
 Release Date: 2017-11-01 10:38:41

 Project: BLE Game Controller
 Maker/Author - Markel T. Robregado
 Date: June 30, 2018
 Modification Details : Add Educational BoosterPack MKII to CC2640R2F Launchpad
                        to function as BLE Game Controller
 Device Setup: TI CC2640R2F Launchpad + Educational BoosterPack MKII
 *****************************************************************************/


/*********************************************************************
 * INCLUDES
 */

#include <hidgamecontroller.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/drivers/ADC.h>
#include <ti/display/Display.h>
#include <icall.h>
#include <string.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "devinfoservice.h"
#include "battservice.h"
#include "hidkbdservice.h"
#include "hiddev.h"
#include "ll_common.h"

#include "peripheral.h"
#include "board_key.h"
#include "board.h"


/*********************************************************************
 * MACROS
 */

#define KEY_NONE                    0x00

// Selected HID LED bitmaps
#define LED_NUM_LOCK                0x01
#define LED_CAPS_LOCK               0x02

// Selected HID mouse button values
#define MOUSE_BUTTON_1              0x01
#define MOUSE_BUTTON_NONE           0x00

// HID keyboard input report length
#define HID_KEYBOARD_IN_RPT_LEN     8

// HID LED output report length
#define HID_LED_OUT_RPT_LEN         1

// HID mouse input report length
#define HID_MOUSE_IN_RPT_LEN        5

/*********************************************************************
 * CONSTANTS
 */

// HID idle timeout in msec; set to zero to disable timeout
#define DEFAULT_HID_IDLE_TIMEOUT              60000

// Minimum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         50

// Supervision timeout value (units of 10ms) if automatic parameter update
// request is enabled.
#define DEFAULT_DESIRED_CONN_TIMEOUT          500

// Whether to enable automatic parameter update request when a connection is
// formed.
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         10

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                  GAPBOND_PAIRING_MODE_WAIT_FOR_REQ

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                     FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                  TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES               GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL           6

// Key bindings, can be modified to any HID value.

#define KEY_UP_HID_BINDING                    HID_KEYBOARD_UP_ARROW
#define KEY_DOWN_HID_BINDING                  HID_KEYBOARD_DOWN_ARROW
#define KEY_LEFT_HID_BINDING                  HID_KEYBOARD_LEFT_ARROW
#define KEY_RIGHT_HID_BINDING                 HID_KEYBOARD_RIGHT_ARROW
#define KEY_SELECT_HID_BINDING                HID_KEYBOARD_SPACEBAR
#define KEY_START_HID_BINDING                 HID_KEYBOARD_RETURN

#define KEY_Z_HID_BINDING                     HID_KEYBOARD_Z
#define KEY_X_HID_BINDING                     HID_KEYBOARD_X

#define USE_HID_MOUSE



// Task configuration
#define HIDGAMECONTROLLER_TASK_PRIORITY               1

#ifndef HIDGAMECONTROLLER_TASK_STACK_SIZE
#define HIDGAMECONTROLLER_TASK_STACK_SIZE             644
#endif

#define HID_STATE_CHANGE_EVT                          0x0001

// Task Events
#define HIDGAMECONTROLLER_ICALL_EVT                   ICALL_MSG_EVENT_ID // Event_Id_31
#define HIDGAMECONTROLLER_QUEUE_EVT                   UTIL_QUEUE_EVENT_ID // Event_Id_30
#define HIDGAMECONTROLLER_PERIODIC_EVT                Event_Id_00

#define HIDGAMECONTROLLER_ALL_EVENTS                  (HIDGAMECONTROLLER_ICALL_EVT | \
                                                       HIDGAMECONTROLLER_QUEUE_EVT | \
                                                       HIDGAMECONTROLLER_PERIODIC_EVT)

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // Event header
} hidGameControllerEvt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Clock instances for internal periodic events.
Clock_Struct periodicClock;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

static uint8_t buf[HID_KEYBOARD_IN_RPT_LEN];

// Task configuration
Task_Struct hidGameControllerTask;
Char hidGameControllerTaskStack[HIDGAMECONTROLLER_TASK_STACK_SIZE];

// GAP Profile - Name attribute for SCAN RSP data
static uint8_t scanData[] =
{
    0x13,                             // length of this data
    GAP_ADTYPE_LOCAL_NAME_COMPLETE,   // AD Type = Complete local name
    'H',
    'I',
    'D',
    ' ',
    'G',
    'a',
    'm',
    'e',
    ' ',
    'C',
    'o',
    'n',
    't',
    'r',
    'o',
    'l',
    'l',
    'e',
    'r'
};

// Advertising data
static uint8_t advData[] =
{
    // flags
    0x02,   // length of this data
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    // appearance
    0x03,   // length of this data
    GAP_ADTYPE_APPEARANCE,
    LO_UINT16(GAP_APPEARE_HID_KEYBOARD),
    HI_UINT16(GAP_APPEARE_HID_KEYBOARD),

    // service UUIDs
    0x05,   // length of this data
    GAP_ADTYPE_16BIT_MORE,
    LO_UINT16(HID_SERV_UUID),
    HI_UINT16(HID_SERV_UUID),
    LO_UINT16(BATT_SERV_UUID),
    HI_UINT16(BATT_SERV_UUID)
};

// Device name attribute value
static CONST uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "HID Game Controller";

// HID Dev configuration
static hidDevCfg_t hidGameControllerCfg =
{
    DEFAULT_HID_IDLE_TIMEOUT,   // Idle timeout
    HID_KBD_FLAGS               // HID feature flags
};

#ifdef USE_HID_MOUSE
// TRUE if boot mouse enabled
static uint8_t hidBootMouseEnabled = FALSE;
#endif // USE_HID_MOUSE

ADC_Handle   adchandlech0;
ADC_Handle   adchandlech5;
ADC_Params   paramsch0;
ADC_Params   paramsch5;


/*********************************************************************
 * LOCAL FUNCTIONS
 */

// Application task and event processing.
static void HidGameController_init(void);
static void HidGameController_taskFxn(UArg a0, UArg a1);
static void HidGameController_processAppMsg(hidGameControllerEvt_t *pMsg);
static void HidGameController_processStackMsg(ICall_Hdr *pMsg);
static void HidGameController_processGattMsg(gattMsgEvent_t *pMsg);
#if 0
static uint8_t HidGameController_enqueueMsg(uint16_t event, uint8_t state);
#endif
static void HID_GameController_clockHandler(UArg arg);

// Key press.
static void HidGameController_keyPressHandler(uint8_t keys);

// HID reports.
static void HidGameController_sendReport(void);
//static void HidGameController_sendReport(uint8_t keycode);
#ifdef USE_HID_MOUSEx
static void HidGameController_sendMouseReport(uint8_t buttons);
#endif // USE_HID_MOUSE
static uint8_t HidGameController_receiveReport(uint8_t len, uint8_t *pData);
static uint8_t HidGameController_reportCB(uint8_t id, uint8_t type, uint16_t uuid,
                                  uint8_t oper, uint16_t *pLen, uint8_t *pData);
static void HidGameController_hidEventCB(uint8_t evt);
static void HidGameController_PeriodicEvent(void);
static void HidJoystick_Init(void);
static void HidJoystick_Read(void);

/*********************************************************************
 * PROFILE CALLBACKS
 */

static hidDevCB_t hidGameControllerHidCBs =
{
    HidGameController_reportCB,
    HidGameController_hidEventCB,
  NULL
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HidJoystick_Init
 *
 * @brief   Initialize ADC0 and ADC5 for reading joystick analog values
 *
 * @param   none
 *
 * @return  none
 */
static void HidJoystick_Init(void)
{
    ADC_init();

    // Joystick X
    ADC_Params_init(&paramsch0);
    adchandlech0 = ADC_open(Board_ADC0, &paramsch0);

    if (adchandlech0 == NULL)
    {
        Display_printf(display, 0, 0, "Error initializing ADC channel 0\n");
        while (1);
    }

    // Joystick Y
    ADC_Params_init(&paramsch5);
    adchandlech5 = ADC_open(Board_ADC5, &paramsch5);

    if (adchandlech5 == NULL)
    {
        Display_printf(display, 0, 0, "Error initializing ADC channel 5\n");
        while (1);
    }

}

/*********************************************************************
 * @fn      HidJoystick_Read
 *
 * @brief   Read Joystick analog values.
 *
 * @param   none
 *
 * @return  none
 */
static void HidJoystick_Read(void)
{
    int_fast16_t resch0, resch5;
    uint16_t adcValuech0, adcValuech5;

    resch0 = ADC_convert(adchandlech0, &adcValuech0);

    if (resch0 != ADC_STATUS_SUCCESS)
    {
        while(1);
    }

    resch5 = ADC_convert(adchandlech5, &adcValuech5);

    if (resch5 != ADC_STATUS_SUCCESS)
    {
        while(1);
    }

    //adcValuech0 x axis no movement 1534 - 1535
    if ((adcValuech0 > (1534 - 20)) && (adcValuech0 < (1534 + 20)))
    {
        buf[2] = KEY_NONE;
    }
    //adcValuech0 x axis left 5
    else if ((adcValuech0 > 0) && (adcValuech0 < 20))
    {
        buf[2] = KEY_LEFT_HID_BINDING;
    }
    //adcValuech0 x axis right 3105 - 3106
    else if ((adcValuech0 > (3105 - 20)) && (adcValuech0 < (3106 + 20)))
    {
        buf[2] = KEY_RIGHT_HID_BINDING;
    }
    else
    {
        buf[2] = KEY_NONE;
    }

    //adcValuech5 y axis no movement 1555 - 1556
    if ((adcValuech5 > (1555 - 20)) && (adcValuech5 < (1555 + 20)))
    {
        buf[3] = KEY_NONE;
    }
    //adcValuech5 y axis down 5
    else if ((adcValuech5 > 0) && (adcValuech5 < 20))
    {
        buf[3] = KEY_DOWN_HID_BINDING;
    }
    //adcValuech5 y axis up 3105 - 3106
    else if ((adcValuech5 > (3105 - 20)) && (adcValuech5 < (3106 + 20)))
    {
        buf[3] = KEY_UP_HID_BINDING;
    }
    else
    {
        buf[3] = KEY_NONE;
    }
}

/*********************************************************************
 * @fn      HidGameController_createTask
 *
 * @brief   Task creation function for the HID emulated keyboard.
 *
 * @param   none
 *
 * @return  none
 */
void HidGameController_createTask(void)
{
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = hidGameControllerTaskStack;
    taskParams.stackSize = HIDGAMECONTROLLER_TASK_STACK_SIZE;
    taskParams.priority = HIDGAMECONTROLLER_TASK_PRIORITY;

    Task_construct(&hidGameControllerTask, HidGameController_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      HidGameController_init
 *
 * @brief   Initialization function for the HidGameController App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ...).
 *
 * @param   none
 *
 * @return  none
 */
void HidGameController_init(void)
{

    /* Shut down external flash on LaunchPads. It is powered on by default
    * but can be shut down through SPI
    */
    Board_shutDownExtFlash();


    // ******************************************************************
    // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
    // ******************************************************************
    // Register the current thread as an ICall dispatcher application
    // so that the application can send and receive messages.
    ICall_registerApp(&selfEntity, &syncEvent);

    // Hard code the DB Address till CC2650 board gets its own IEEE address
    //uint8 bdAddress[B_ADDR_LEN] = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x5A };
    //HCI_EXT_SetBDADDRCmd(bdAddress);

    // Set device's Sleep Clock Accuracy
    //HCI_EXT_SetSCACmd(40);

    // Create an RTOS queue for message from profile to be sent to app.
    appMsgQueue = Util_constructQueue(&appMsg);

    HidJoystick_Init();

    // Create one-shot clocks for internal periodic events.
    Util_constructClock(&periodicClock, HID_GameController_clockHandler,
                        HID_PERIODIC_EVT_PERIOD, 0, false, HIDGAMECONTROLLER_PERIODIC_EVT);

    // Setup the GAP
    VOID GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL,
                         DEFAULT_CONN_PAUSE_PERIPHERAL);

    // Setup the GAP Peripheral Role Profile
    {
        uint8_t initial_advertising_enable = TRUE;

        // By setting this to zero, the device will go into the waiting state after
        // being discoverable for 30.72 second, and will not being advertising again
        // until the enabler is set back to TRUE
        uint16_t gapRole_AdvertOffTime = 0;

        uint8_t enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
        uint16_t desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
        uint16_t desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
        uint16_t desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
        uint16_t desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

        // Set the GAP Role Parameters
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                             &initial_advertising_enable);
        GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                             &gapRole_AdvertOffTime);

        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advData), advData);
        GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanData), scanData);

        GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                             &enable_update_request);
        GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                             &desired_min_interval);
        GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                             &desired_max_interval);
        GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                             &desired_slave_latency);
        GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                             &desired_conn_timeout);
    }

    // Set the GAP Characteristics
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
                   (void *)attDeviceName);

    // Setup the GAP Bond Manager
    {
        uint8_t pairMode = DEFAULT_PAIRING_MODE;
        uint8_t mitm = DEFAULT_MITM_MODE;
        uint8_t ioCap = DEFAULT_IO_CAPABILITIES;
        uint8_t bonding = DEFAULT_BONDING_MODE;

        GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
        GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    }

    // Setup Battery Characteristic Values
    {
        uint8_t critical = DEFAULT_BATT_CRITICAL_LEVEL;

        Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof (uint8_t), &critical);
    }

    // Set up HID keyboard service
    HidKbd_AddService();

    // Register for HID Dev callback
    HidDev_Register(&hidGameControllerCfg, &hidGameControllerHidCBs);

    // Start the GAP Role and Register the Bond Manager.
    HidDev_StartDevice();

    // Initialize keys on CC2640R2F LP.
    Board_initKeys(HidGameController_keyPressHandler);

    // Register with GAP for HCI/Host messages
    GAP_RegisterForMsgs(selfEntity);

#if !defined (USE_LL_CONN_PARAM_UPDATE)
    // Get the currently set local supported LE features
    // The HCI will generate an HCI event that will get received in the main
    // loop
    HCI_LE_ReadLocalSupportedFeaturesCmd();
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

    // Use default data Tx / Rx data length and times
    HCI_EXT_SetMaxDataLenCmd(LL_MIN_LINK_DATA_LEN, LL_MIN_LINK_DATA_TIME, LL_MIN_LINK_DATA_LEN, LL_MIN_LINK_DATA_TIME);
}

/*********************************************************************
 * @fn      HidGameController_taskFxn
 *
 * @brief   HidGameController Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   a0, a1 - not used.
 *
 * @return  none
 */
void HidGameController_taskFxn(UArg a0, UArg a1)
{
    // Initialize the application.
    HidGameController_init();

    // Application main loop.
    for (;;)
    {
        uint32_t events;

        events = Event_pend(syncEvent, Event_Id_NONE, HIDGAMECONTROLLER_ALL_EVENTS,
                            ICALL_TIMEOUT_FOREVER);

        if (events)
        {
            ICall_EntityID dest;
            ICall_ServiceEnum src;
            ICall_HciExtEvt *pMsg = NULL;

            if (ICall_fetchServiceMsg(&src, &dest,
                                    (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
            {
                if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
                {
                  // Process inter-task message
                  HidGameController_processStackMsg((ICall_Hdr *)pMsg);
                }

                if (pMsg)
                {
                    ICall_freeMsg(pMsg);
                }
            }

            // If RTOS queue is not empty, process app message.
            if (events & HIDGAMECONTROLLER_QUEUE_EVT)
            {
                while (!Queue_empty(appMsgQueue))
                {
                    hidGameControllerEvt_t *pMsg = (hidGameControllerEvt_t *)Util_dequeueMsg(appMsgQueue);
                    if (pMsg)
                    {
                        // Process message.
                        HidGameController_processAppMsg(pMsg);

                        // Free the space from the message.
                        ICall_free(pMsg);
                    }
                }
            }

            if (events & HIDGAMECONTROLLER_PERIODIC_EVT)
            {
                HidGameController_PeriodicEvent();
                Util_restartClock(&periodicClock, HID_PERIODIC_EVT_PERIOD);
            }
        }
    }
}

/*********************************************************************
 * @fn      HidGameController_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HidGameController_processStackMsg(ICall_Hdr *pMsg)
{
    switch (pMsg->event)
    {
        case GATT_MSG_EVENT:
            HidGameController_processGattMsg((gattMsgEvent_t *) pMsg);
        break;

        case HCI_GAP_EVENT_EVENT:
        {

            // Process HCI message
            switch(pMsg->status)
            {
                case HCI_COMMAND_COMPLETE_EVENT_CODE:
                // Process HCI Command Complete Event
                {

#if !defined (USE_LL_CONN_PARAM_UPDATE)
                    // This code will disable the use of the LL_CONNECTION_PARAM_REQ
                    // control procedure (for connection parameter updates, the
                    // L2CAP Connection Parameter Update procedure will be used
                    // instead). To re-enable the LL_CONNECTION_PARAM_REQ control
                    // procedures, define the symbol USE_LL_CONN_PARAM_UPDATE

                    // Parse Command Complete Event for opcode and status
                    hciEvt_CmdComplete_t* command_complete = (hciEvt_CmdComplete_t*) pMsg;
                    uint8_t   pktStatus = command_complete->pReturnParam[0];

                    //find which command this command complete is for
                    switch (command_complete->cmdOpcode)
                    {
                        case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
                        {
                            if (pktStatus == SUCCESS)
                            {
                                uint8_t featSet[8];

                                // get current feature set from received event (bits 1-9 of
                                // the returned data
                                memcpy( featSet, &command_complete->pReturnParam[1], 8 );

                                // Clear bit 1 of byte 0 of feature set to disable LL
                                // Connection Parameter Updates
                                CLR_FEATURE_FLAG( featSet[0], LL_FEATURE_CONN_PARAMS_REQ );

                                // Update controller with modified features
                                HCI_EXT_SetLocalSupportedFeaturesCmd( featSet );
                            }
                        }
                        break;

                        default:
                        //do nothing
                        break;
                    }
#endif // !defined (USE_LL_CONN_PARAM_UPDATE)

                }
                break;

                default:
                break;
            }
        }
        break;

        default:
        // Do nothing
        break;
    }
}

/*********************************************************************
 * @fn      HidGameController_processGattMsg
 *
 * @brief   Process GATT messages
 *
 * @return  none
 */
static void HidGameController_processGattMsg(gattMsgEvent_t *pMsg)
{
    GATT_bm_free(&pMsg->msg, pMsg->method);
}

/*********************************************************************
 * @fn      HidGameController_processAppMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HidGameController_processAppMsg(hidGameControllerEvt_t *pMsg)
{
    switch (pMsg->hdr.event)
    {
        case HID_STATE_CHANGE_EVT:
        {

            break;
        }


        default:
        //Do nothing.
        break;
    }
}

/*********************************************************************
 * @fn      HidKEmukbd_keyPressHandler
 *
 * @brief   Key event handler function.
 *
 * @param   a0 - ignored
 *
 * @return  none
 */
static void HidGameController_keyPressHandler(uint8_t keys)
{
    buf[4] = KEY_NONE;         // Keycode 3 z
    buf[5] = KEY_NONE;         // Keycode 4 x
    buf[6] = KEY_NONE;         // Keycode 5 select/start

    if (keys & KEY_Z)
    {
        buf[4] = KEY_Z_HID_BINDING;
    }

    if (keys & KEY_X)
    {
        buf[5] = KEY_X_HID_BINDING;
    }

    if (keys & KEY_SELECT)
    {
        buf[6] = KEY_SELECT_HID_BINDING;
    }

    if (keys & KEY_START)
    {
        buf[6] = KEY_START_HID_BINDING;
    }
}

/*********************************************************************
 * @fn      HidGameController_PeriodicEvent
 *
 * @brief   Perform a periodic application task. This function gets called
 *          every 80 ms (HID_PERIODIC_EVT_PERIOD).
 *
 * @param   None.
 *
 * @return  None.
 */
static void HidGameController_PeriodicEvent(void)
{
    HidJoystick_Read();
    HidGameController_sendReport();
}

/*********************************************************************
 * @fn      HidGameController_sendReport
 *
 * @brief   Build and send a HID keyboard report.
 *
 * @param   keycode - HID keycode.
 *
 * @return  none
 */

static void HidGameController_sendReport(void)
{
    buf[0] = 0;         // Modifier keys
    buf[1] = 0;         // Reserved
    buf[7] = 0;         // Keycode 6

    HidDev_Report(HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT,
                  HID_KEYBOARD_IN_RPT_LEN, buf);

    buf[4] = 0;         // Keycode 3 z
    buf[5] = 0;         // Keycode 4 x
    buf[6] = 0;         // Keycode select start
}

#ifdef USE_HID_MOUSEx
/*********************************************************************
 * @fn      HidGameController_sendMouseReport
 *
 * @brief   Build and send a HID mouse report.
 *
 * @param   buttons - Mouse button code
 *
 * @return  none
 */
static void HidGameController_sendMouseReport(uint8_t buttons)
{
    uint8_t buf[HID_MOUSE_IN_RPT_LEN];

    buf[0] = buttons;   // Buttons
    buf[1] = 0;         // X
    buf[2] = 0;         // Y
    buf[3] = 0;         // Wheel
    buf[4] = 0;         // AC Pan

    HidDev_Report(HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                  HID_MOUSE_IN_RPT_LEN, buf);
}
#endif // USE_HID_MOUSE

/*********************************************************************
 * @fn      HidGameController_receiveReport
 *
 * @brief   Process an incoming HID keyboard report.
 *
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  status
 */
static uint8_t HidGameController_receiveReport(uint8_t len, uint8_t *pData)
{
    // Verify data length
    if (len == HID_LED_OUT_RPT_LEN)
    {
        // Set keyfob LEDs
        //HalLedSet(HAL_LED_1, ((*pData & LED_CAPS_LOCK) == LED_CAPS_LOCK));
        //HalLedSet(HAL_LED_2, ((*pData & LED_NUM_LOCK) == LED_NUM_LOCK));

        return SUCCESS;
    }
    else
    {
        return ATT_ERR_INVALID_VALUE_SIZE;
    }
}

/*********************************************************************
 * @fn      HidGameController_reportCB
 *
 * @brief   HID Dev report callback.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   oper - operation:  read, write, etc.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  GATT status code.
 */
static uint8_t HidGameController_reportCB(uint8_t id, uint8_t type, uint16_t uuid,
                                  uint8_t oper, uint16_t *pLen, uint8_t *pData)
{
    uint8_t status = SUCCESS;

    // Write
    if (oper == HID_DEV_OPER_WRITE)
    {
        if (uuid == REPORT_UUID)
        {
            // Process write to LED output report; ignore others
            if (type == HID_REPORT_TYPE_OUTPUT)
            {
                status = HidGameController_receiveReport(*pLen, pData);
            }
        }

        if (status == SUCCESS)
        {
            status = HidKbd_SetParameter(id, type, uuid, *pLen, pData);
        }
    }
    // Read
    else if (oper == HID_DEV_OPER_READ)
    {
        uint8_t len;

        status = HidKbd_GetParameter(id, type, uuid, &len, pData);
        if (status == SUCCESS)
        {
            *pLen = len;
        }
    }
    // Notifications enabled
    else if (oper == HID_DEV_OPER_ENABLE)
    {
        if (id == HID_RPT_ID_MOUSE_IN && type == HID_REPORT_TYPE_INPUT)
        {
#ifdef USE_HID_MOUSE
            hidBootMouseEnabled = TRUE;
#endif // USE_HID_MOUSE
        }
    }
    // Notifications disabled
    else if (oper == HID_DEV_OPER_DISABLE)
    {
        if (id == HID_RPT_ID_MOUSE_IN && type == HID_REPORT_TYPE_INPUT)
        {
#ifdef USE_HID_MOUSE
            hidBootMouseEnabled = FALSE;
#endif // USE_HID_MOUSE
        }
    }

    return status;
}

/*********************************************************************
 * @fn      HidGameController_hidEventCB
 *
 * @brief   HID Dev event callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void HidGameController_hidEventCB(uint8_t evt)
{
    // Process enter/exit suspend or enter/exit boot mode
    return;
}


/*********************************************************************
 * @fn      BLE_PowerBank_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void HID_GameController_clockHandler(UArg arg)
{
    // Wake up the application.
    Event_post(syncEvent, arg);
}

/*********************************************************************
 * @fn      HidGameController_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event  - message event.
 * @param   state  - message state.
 *
 * @return  TRUE or FALSE
 */
#if 0
static uint8_t HidGameController_enqueueMsg(uint16_t event, uint8_t state)
{
    hidGameControllerEvt_t *pMsg;

    // Create dynamic pointer to message.
    if (pMsg = ICall_malloc(sizeof(hidGameControllerEvt_t)))
    {
        pMsg->hdr.event = event;
        pMsg->hdr.state = state;

        // Enqueue the message.
        return Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
    }

    return FALSE;
}
#endif


/*********************************************************************
*********************************************************************/
