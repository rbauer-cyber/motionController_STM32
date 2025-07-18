//$file${.::switch.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: MotionController.qm
// File:  ${.::switch.cpp}
//
// This code has been generated by QM 7.0.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// Copyright (c) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                 ____________________________________
//                /                                   /
//               /    GGGGGGG    PPPPPPPP   LL       /
//              /   GG     GG   PP     PP  LL       /
//             /   GG          PP     PP  LL       /
//            /   GG   GGGGG  PPPPPPPP   LL       /
//           /   GG      GG  PP         LL       /
//          /     GGGGGGG   PP         LLLLLLL  /
//         /___________________________________/
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open-source software licensed under the GNU
// General Public License (GPL) as published by the Free Software Foundation
// (see <https://www.gnu.org/licenses>).
//
// NOTE:
// The GPL does NOT permit the incorporation of this code into proprietary
// programs. Please contact Quantum Leaps for commercial licensing options,
// which expressly supersede the GPL and are designed explicitly for
// closed-source distribution.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::switch.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"    // QP/C real-time embedded framework
#include "bsp.hpp"    // Board Support Package interface

#include "common.hpp"
#include "console.h"
#include "motion.h"
#include "switch.hpp"
//#include "motion.hpp"

// ask QM to declare the Switch class
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared::AO_Switch} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AO_Switch} .......................................................
QP::QActive * const AO_Switch = &Switch::inst;

} // namespace APP
//$enddef${Shared::AO_Switch} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Switch} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Switch} .............................................................
Switch Switch::inst;

//${AOs::Switch::CreatePeriodicTimer} ........................................
void Switch::CreatePeriodicTimer(uint32_t time) {
    //QTimeEvt_ctorX(&me->timeEvt, &me->super, UPDATE_TIME_SIG, 0U);
    //QTimeEvt_armX(&me->timeEvt, time, time);
    m_timeEvt.armX(time, time);
}

//${AOs::Switch::SendSwitchEvent} ............................................
void Switch::SendSwitchEvent(uint8_t state) {
    // Send new switch postiion
    SwitchEvt *pEvt = Q_NEW(SwitchEvt, SWITCH_CHANGED_SIG);
    pEvt->state = state;
    //QACTIVE_POST(me->AO_Client, (QEvt*)pEvt, &me->super);
    m_AO_Client->POST(pEvt, this);
}

//${AOs::Switch::CheckSwitchChanged} .........................................
uint8_t Switch::CheckSwitchChanged() {
    uint8_t switchState = BSP_readSwitch();
    uint8_t switchChanged = 0;

    // Check for switch debounce, if recorded state the same,
    // then stable read, otherwise preserve from last reading.
    // Update time between readings provides debounce
    if ( m_state == switchState )
    {
        // Test for switich state changed
        if ( m_state != m_lastState )
        {
            m_lastState = m_state;
            consoleDisplayArgs("Switch: notify, new state: %d;\r\n", m_state);
            switchChanged = 1;
        }
    }
    else
    {
        m_state = !m_lastState;
    }

    return switchChanged;
}

//${AOs::Switch::Switch} .....................................................
Switch::Switch()
  : QActive(Q_STATE_CAST(&initial)),
    m_timeEvt(this, UPDATE_TIME_SIG, 0U)
{
    m_state = 1;
    m_lastState = 1;
    m_notifyCount = 0;
    m_initSystemState = 10;
}

//${AOs::Switch::CreateOneShotTimer} .........................................
void Switch::CreateOneShotTimer(uint32_t time) {
    //QTimeEvt_ctorX(&me->timeEvt, &me->super, UPDATE_TIME_SIG, 0U);
    //QTimeEvt_armX(&me->timeEvt, time, time);
    m_timeEvt.armX(time, 0);
}

//${AOs::Switch::SM} .........................................................
Q_STATE_DEF(Switch, initial) {
    //${AOs::Switch::SM::initial}
    // Start periodic time to read switch position
    m_AO_Client = Q_EVT_CAST(ClientEvt)->client;
    consoleDisplay("Switch: starting;\r\n");
    m_measure.Initialize();
    //m_measure.Run();
    CreatePeriodicTimer(1000);
    return tran(&updateSwitchState);
}

//${AOs::Switch::SM::updateSwitchState} ......................................
Q_STATE_DEF(Switch, updateSwitchState) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Switch::SM::updateSwitchState}
        case Q_ENTRY_SIG: {
            //BSP_ToggleLed(BSP_MAX_LED);

            if ( m_notifyCount >= 1 )
            {
                CheckSwitchChanged();
                // Always update client with switch state
                SendSwitchEvent(m_state);
                m_notifyCount = 0;
            }

            //CreateOneShotTimer(1000);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Switch::SM::updateSwitchStat~::UPDATE_TIME}
        case UPDATE_TIME_SIG: {
            m_measure.Start();
            status_ = tran(&readSwitch);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${AOs::Switch::SM::readSwitch} .............................................
Q_STATE_DEF(Switch, readSwitch) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Switch::SM::readSwitch}
        case Q_ENTRY_SIG: {
            // Read and store state of switch for debouncing in Idle.
            m_state = BSP_readSwitch();
            m_notifyCount += 1;

            #if 0
            if ( m_initSystemState-- > 1 )
            {
                MoveEvt* pe = Q_NEW(MoveEvt, SHOW_STATE_SIG);
                QP::QActive::PUBLISH(pe, this);
            }
            #endif
            //CreateOneShotTimer(1000);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Switch::SM::readSwitch::UPDATE_TIME}
        case UPDATE_TIME_SIG: {
            m_measure.UpdateElapsedTime();
            m_measure.DisplayElapsedTimeDelta();
            status_ = tran(&updateSwitchState);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

} // namespace APP
//$enddef${AOs::Switch} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
