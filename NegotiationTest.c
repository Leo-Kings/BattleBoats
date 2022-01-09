/*
 * 
 * Leo King lking1
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "BOARD.h"
#include "Negotiation.h"

#define SimpleTest 3
#define ModularTest1 12345
#define ModularTest2 0xFFFF
#define CoinTest1 0x7FDC

int NGH = 0, NGV = 0, NCF = 0;

int main() 
{
    BOARD_Init();
    //NEGOTIATIONHASH TESTS
    printf("NegotiationHash Tests:\n");
    if (NegotiationHash(SimpleTest) == 9) {              //SimpleTest
        NGH++;
    } else {
        printf("    Failed Simple Test!!\n");
    }
    if (NegotiationHash(ModularTest1) == 43182) {      //Mod PUBLIC_KEY Test
        NGH++;
    } else {
        printf("    Failed to Pass Modular Test with Public Key!\n");
    }
    if (NegotiationHash(ModularTest2) > PUBLIC_KEY) {
        printf("    Failed to Pass Harder Modular Test with Public Key!\n");
    } else {
        NGH++;
        printf("    Passed (%d/3) Tests for Negotiation Hash!\n", NGH);
    }
    //NegotiationVerify Test
    printf("NegotiationVerify Tests:\n");
    if (NegotiationVerify(ModularTest1, NegotiationHash(ModularTest1))) {
        NGV++;
    } else {
        printf("    Failed to Return True on Check Test!\n");
    }
    if (NegotiationVerify(ModularTest2, ModularTest2) == FALSE) {
        NGV++;
        printf("    Passed (%d/2) Tests for NegotiationVerify!\n", NGV);
    } else {
        printf("    Failed To Return FALSE on Check Test!\n");
    }
    printf("NegotiateCoinFlip Tests:\n");
    
    int A = rand(), B = rand();
    if (NegotiateCoinFlip(A, B) == HEADS || NegotiateCoinFlip(A, B) == TAILS) {
        NCF++;
    } else {
        printf("    Failed to return Heads nor Tails!\n");
    }
    if (NegotiateCoinFlip(SimpleTest, SimpleTest) == TAILS) {
        NCF++;
    } else {
        printf("    Failed to return Tails given Even Bits!\n");
    }
    if (NegotiateCoinFlip(CoinTest1, PUBLIC_KEY) == TAILS) {
        printf("    Failed to return Heads given Odd Bits!\n"); 
    } else {
        NCF++;
        printf("    Passed (%d/3) Tests for Negotiate Coin Flip!\n", NCF);
    }
    
    BOARD_End();
    while(1);
    return 0
}