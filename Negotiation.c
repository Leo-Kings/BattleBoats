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

#define LastBit 16
#define Mask_1 0x01
#define bitcheck 0x01

NegotiationData NegotiationHash(NegotiationData secret) {
    uint64_t ret = (uint64_t) secret;
    ret = (ret * ret) % PUBLIC_KEY;
    return ((NegotiationData) ret);
}

int NegotiationVerify(NegotiationData secret, NegotiationData commitment) {
    if (NegotiationHash(secret) == commitment) {
        return TRUE;
    } else {
        return FALSE;
    }
}

NegotiationOutcome NegotiateCoinFlip(NegotiationData A, NegotiationData B) {
    int counter = 0, XORBitCounter = 0, parity;
    NegotiationData xorA_B = A ^ B;
    while (counter != LastBit) {
        counter++;
        int temp;
        temp = xorA_B & Mask_1;
        xorA_B >>= 1;
        if (temp) {
            XORBitCounter++;
        }
    }
    parity = XORBitCounter % 2;
    if (parity) {
        return HEADS;
    } else {
        return TAILS;
    }
}


/**
 * Extra credit: 
 * Use either or both of these two functions if you want to generate a "cheating" agent.  
 *
 * To get extra credit, define these functions 
 * and use these functions in agent.c to generate A and/or B
 * 
 * Your agent only needs to be able to cheat at one role for extra credit.  They must result in 
 * your agent going first more than 75% of the time in that role when
 * competing against a fair agent (that is, an agent that uses purely random A and B).
 *
 * You must state that you did this at the top of your README, and describe your 
 * strategy thoroughly.
 */
NegotiationData NegotiateGenerateBGivenHash(NegotiationData hash_a);
//  We want to create a B value given the hash_a 
//  We generate hash #a by squaring A and then modular it by public key

NegotiationData NegotiateGenerateAGivenB(NegotiationData B);
//  We generate an A data that matches the hash #a given B
