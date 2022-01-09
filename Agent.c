#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Agent.h"
#include "Negotiation.h"
#include "BattleBoats.h"
#include "Field.h"
#include "FieldOled.h"
#include "Oled.h"
#include "BOARD.h"

//Static variables
static Field ownField;
static Field oppField;

static uint16_t turnCounter;

static NegotiationData own;
static NegotiationData ownHash;
static NegotiationData opp;

static GuessData oppGuessData;
static GuessData ownGuessData;

static AgentState currentState;

/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts.  At a minimum, this requires:
 *   -setting the start state of the Agent SM.
 *   -setting turn counter to 0
 * If you are using any other persistent data in Agent SM, that should be reset as well.
 * 
 * It is not advised to call srand() inside of AgentInit.  
 *  */
void AgentInit(void) {
    currentState = AGENT_STATE_START;
    own = (NegotiationData) rand();
    ownHash = NegotiationHash(own);
    opp = rand();
    turnCounter = 0;
}

/**
 * AgentRun evolves the Agent state machine in response to an event.
 * 
 * @param  The most recently detected event
 * @return Message, a Message struct to send to the opponent. 
 * 
 * If the returned Message struct is a valid message
 * (that is, not of type MESSAGE_NONE), then it will be
 * passed to the transmission module and sent via UART.
 * This is handled at the top level! AgentRun is ONLY responsible 
 * for generating the Message struct, not for encoding or sending it.
 */
Message AgentRun(BB_Event event) {
    Message returnMessage = {MESSAGE_NONE, 0, 0, 0};
    if (event.type == BB_EVENT_RESET_BUTTON) {
        AgentInit();
        OledDrawString("This is BattleBoats!\nPress BTN4 to\nchallenge, or wait\nfor opponent.");
        OledUpdate();
        return returnMessage;
    }
    switch (currentState) {
        case AGENT_STATE_START:
            if (event.type == BB_EVENT_START_BUTTON) {
                currentState = AGENT_STATE_CHALLENGING;
                ownHash = NegotiationHash(own);
                returnMessage.type = MESSAGE_CHA;
                returnMessage.param0 = ownHash;
                FieldInit(&ownField, &oppField);
                FieldAIPlaceAllBoats(&ownField);
            } else if (event.type == BB_EVENT_CHA_RECEIVED) {
                currentState = AGENT_STATE_ACCEPTING;
                opp = (NegotiationData) rand();
                returnMessage.type = MESSAGE_ACC;
                ownHash = event.param0;
                returnMessage.param0 = opp;
                FieldInit(&ownField, &oppField);
                FieldAIPlaceAllBoats(&ownField);
            }
            break;
        case AGENT_STATE_CHALLENGING:
            if (event.type == BB_EVENT_ACC_RECEIVED) {
                if ((NegotiateCoinFlip(own, opp)) == HEADS) {
                    currentState = AGENT_STATE_WAITING_TO_SEND;
                } else if ((NegotiateCoinFlip(own, opp)) == TAILS) {
                    currentState = AGENT_STATE_DEFENDING;
                }
            }
            break;
        case AGENT_STATE_ACCEPTING:
            if (event.type == BB_EVENT_REV_RECEIVED) {
                NegotiationVerify(own, ownHash);
                if (NegotiationVerify(own, ownHash) == FALSE) {
                    OledDrawString("CHEATER DETECTED!");
                    OledUpdate();
                    currentState = AGENT_STATE_END_SCREEN;
                }
                if ((NegotiateCoinFlip(own, opp)) == HEADS) {
                    currentState = AGENT_STATE_DEFENDING;
                } else if ((NegotiateCoinFlip(own, opp)) == TAILS) {
                    ownGuessData = FieldAIDecideGuess(&oppField);
                    returnMessage.type = MESSAGE_SHO;
                    returnMessage.param0 = ownGuessData.row;
                    returnMessage.param1 = ownGuessData.col;
                    returnMessage.param2 = ownGuessData.result;
                    currentState = AGENT_STATE_ATTACKING;
                }
            }
            break;
        case AGENT_STATE_WAITING_TO_SEND:
            if (event.type == BB_EVENT_MESSAGE_SENT) {
                turnCounter++;
                ownGuessData = FieldAIDecideGuess(&oppField);
                returnMessage.type = MESSAGE_SHO;
                returnMessage.param0 = ownGuessData.row;
                returnMessage.param1 = ownGuessData.col;
                returnMessage.param2 = ownGuessData.result;
                currentState = AGENT_STATE_ATTACKING;
            }
            break;
        case AGENT_STATE_DEFENDING:
            if (event.type == BB_EVENT_SHO_RECEIVED) {
                oppGuessData = FieldAIDecideGuess(&ownField);
                FieldUpdateKnowledge(&ownField, &oppGuessData);
                returnMessage.type = MESSAGE_RES;
                if (FieldGetBoatStates(&ownField) == 0) {
                    OledDrawString("DEFEAT!");
                    OledUpdate();
                    currentState = AGENT_STATE_END_SCREEN;
                } else {
                    currentState = AGENT_STATE_WAITING_TO_SEND;
                }
            }
            break;
        case AGENT_STATE_ATTACKING:
            if (event.type == BB_EVENT_RES_RECEIVED) {
                ownGuessData = FieldAIDecideGuess(&oppField);
                FieldUpdateKnowledge(&oppField, &ownGuessData);
                if (FieldGetBoatStates(&oppField) == 0) {
                    OledDrawString("VICTORY!");
                    OledUpdate();
                    currentState = AGENT_STATE_END_SCREEN;
                } else {
                    currentState = AGENT_STATE_DEFENDING;
                }
            }
            break;
        case AGENT_STATE_END_SCREEN:
            break;
        case AGENT_STATE_SETUP_BOATS:
            break;
        default:
            event.type = BB_EVENT_ERROR;
            break;
    }
    if (event.type == BB_EVENT_ERROR) {
        returnMessage.type = MESSAGE_ERROR;
        returnMessage.param0 = event.param0;
        returnMessage.param0 = event.param1;
        returnMessage.param0 = event.param2;
    }
    return returnMessage;
}

/** * 
 * @return Returns the current state that AgentGetState is in.  
 * 
 * This function is very useful for testing AgentRun.
 */
AgentState AgentGetState(void) {
    return currentState;
}

/** * 
 * @param Force the agent into the state given by AgentState
 * 
 * This function is very useful for testing AgentRun.
 */
void AgentSetState(AgentState newState) {
    currentState = newState;
}