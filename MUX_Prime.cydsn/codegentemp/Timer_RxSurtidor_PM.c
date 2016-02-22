/*******************************************************************************
* File Name: Timer_RxSurtidor_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_RxSurtidor.h"

static Timer_RxSurtidor_backupStruct Timer_RxSurtidor_backup;


/*******************************************************************************
* Function Name: Timer_RxSurtidor_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RxSurtidor_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_RxSurtidor_SaveConfig(void) 
{
    #if (!Timer_RxSurtidor_UsingFixedFunction)
        Timer_RxSurtidor_backup.TimerUdb = Timer_RxSurtidor_ReadCounter();
        Timer_RxSurtidor_backup.InterruptMaskValue = Timer_RxSurtidor_STATUS_MASK;
        #if (Timer_RxSurtidor_UsingHWCaptureCounter)
            Timer_RxSurtidor_backup.TimerCaptureCounter = Timer_RxSurtidor_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_RxSurtidor_UDB_CONTROL_REG_REMOVED)
            Timer_RxSurtidor_backup.TimerControlRegister = Timer_RxSurtidor_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_RxSurtidor_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RxSurtidor_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_RxSurtidor_RestoreConfig(void) 
{   
    #if (!Timer_RxSurtidor_UsingFixedFunction)

        Timer_RxSurtidor_WriteCounter(Timer_RxSurtidor_backup.TimerUdb);
        Timer_RxSurtidor_STATUS_MASK =Timer_RxSurtidor_backup.InterruptMaskValue;
        #if (Timer_RxSurtidor_UsingHWCaptureCounter)
            Timer_RxSurtidor_SetCaptureCount(Timer_RxSurtidor_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_RxSurtidor_UDB_CONTROL_REG_REMOVED)
            Timer_RxSurtidor_WriteControlRegister(Timer_RxSurtidor_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_RxSurtidor_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RxSurtidor_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_RxSurtidor_Sleep(void) 
{
    #if(!Timer_RxSurtidor_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_RxSurtidor_CTRL_ENABLE == (Timer_RxSurtidor_CONTROL & Timer_RxSurtidor_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_RxSurtidor_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_RxSurtidor_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_RxSurtidor_Stop();
    Timer_RxSurtidor_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_RxSurtidor_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_RxSurtidor_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_RxSurtidor_Wakeup(void) 
{
    Timer_RxSurtidor_RestoreConfig();
    #if(!Timer_RxSurtidor_UDB_CONTROL_REG_REMOVED)
        if(Timer_RxSurtidor_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_RxSurtidor_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */