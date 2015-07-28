#pragma once

#include "../config/robot.hpp"

/**
 * Initializes the peripheral nested vector interrupt controller (PNVIC) with
 * appropriate values. Low values have the higest priority (with system
 * interrupts having negative priority). All maskable interrupts are
 * diabled; do not intialize any interrupt frameworks before this funtion is
 * called. PNVIC interrupt priorities are independent of thread and NVIC
 * priorities.
 *
 * The PNVIC is independent of the NVIC responsible for system NMI's. The NVIC
 * is not accissable through the library, so in this context the NVIC functions
 * refer to the PNVIC. The configuration system for PNVIC priorities is strange
 * and different from X86. If you haven't already, look over
 * doc/ARM-Cortex-M_Interrupt-Priorities.pdf from RJ root and the online
 * documentation regarding Interrupt Priority Registers (IPRs) first.
 */
void setISRPriorities(void)
{
    //set two bits for preemptive data, two bits for priority as the
    //structure for the IPRs. Grouping is global withing the IPRs so
    //this value should only be changed here.
    NVIC_SetPriorityGrouping(5);
    uint32_t priorityGrouping = NVIC_GetPriorityGrouping();

    //set preemptive priority default to 2 (0..3)
    //set priority default to 1 (0..3)
    uint32_t defaultPriority = NVIC_EncodePriority(priorityGrouping, 2, 1);

    //When the kernel initialzes the PNVIC, all ISRs are set to the
    //highest priority, making it impossible to elevate a few over
    //the rest, so the default priority is lowered globally for the
    //table first.
    //
    //Consult LPC17xx.h under IRQn_Type for PNVIC ranges, this is LPC1768
    //specific
    for (uint32_t IRQn = TIMER0_IRQn; IRQn <= CANActivity_IRQn; IRQn++)
        NVIC_SetPriority((IRQn_Type) IRQn, defaultPriority);

    ////////////////////////////////////
    //  begin raise priority section  //
    ////////////////////////////////////

    //reestablish watchdog
    NVIC_SetPriority(WDT_IRQn, NVIC_EncodePriority(priorityGrouping, 0, 0));

    //TODO raise radio
    //TODO raise others after discussion

    ////////////////////////////////////
    //  begin lower priotity section  //
    ////////////////////////////////////

    //set UART (console) interrupts to minimal priority
    //when debugging radio and other time sensitive operations, this
    //interrupt will need to be deferred.
    NVIC_SetPriority(UART0_IRQn, NVIC_EncodePriority(priorityGrouping, 3, 0));
    NVIC_SetPriority(UART1_IRQn, NVIC_EncodePriority(priorityGrouping, 3, 2));
    NVIC_SetPriority(UART2_IRQn, NVIC_EncodePriority(priorityGrouping, 3, 2));
    NVIC_SetPriority(UART3_IRQn, NVIC_EncodePriority(priorityGrouping, 3, 1));

    //TODO lower others after discussion

    //NVIC_EnableIRQ(TIMER0_IRQn);
    //NVIC_EnableIRQ(TIMER1_IRQn);
    //NVIC_EnableIRQ(TIMER2_IRQn);
    //NVIC_EnableIRQ(TIMER3_IRQn);
}