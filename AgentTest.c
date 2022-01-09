#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "BattleBoats.h"
#include "Agent.h"
#include "BOARD.h"

static AgentState currentState;
BB_Event event;
int AgentInitCounter = 0;
int AgentGetStateCounter = 0;
int AgentSetStateCounter = 0;

int main(){
    BOARD_Init();
    printf("Welcome to AgentTest.c! Compiled on %s %s.\n\n", __DATE__, __TIME__);
    AgentInit();
    if (currentState == AGENT_STATE_START){
        AgentInitCounter++;
    }
    AgentSetState(AGENT_STATE_CHALLENGING);
    if (AgentGetState() == AGENT_STATE_CHALLENGING){
        AgentGetStateCounter++;
        AgentSetStateCounter++;
    }
    AgentRun(event);
    if (AgentRun(event).type == MESSAGE_NONE){
        printf("AgentRun() Test: PASSED!\n");
    }
    AgentInit();
    if (currentState == AGENT_STATE_START){
        AgentInitCounter++;
    }
    AgentSetState(AGENT_STATE_ACCEPTING);
    if (AgentGetState() == AGENT_STATE_ACCEPTING){
        AgentGetStateCounter++;
        AgentSetStateCounter++;
    }
    printf("AgentInit() Test: %d/2 PASSED!\n", AgentInitCounter);
    printf("AgentGetState() Test: %d/2 PASSED!\n", AgentGetStateCounter);
    printf("AgentSetState() Test: %d/2 PASSED!\n", AgentSetStateCounter);
    
    BOARD_End();
    while(1);
    return(0);
}