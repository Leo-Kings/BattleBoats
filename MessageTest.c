
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "BattleBoats.h"
#include "Message.h"
#include "BOARD.h"

#define checksum1 0x5E
#define EncodeLen1 14
#define q 0x22
BB_Event sample1;
Message sample2 = {MESSAGE_CHA, 43182, 0, 0};
Message sample3 = {MESSAGE_NONE, 0, 0, 0};
BB_Event sample4;
char str1[MESSAGE_MAX_LEN];
char str2[MESSAGE_MAX_LEN];
char str3[] = "$CHA,43182*5A\n";
int cc = 0, pm = 0, e = 0, dc = 0, i;
int main() {
    BOARD_Init();
    
    //CheckSum Tests
    printf("Message CalculateChecksum Test: \n");
    if (Message_CalculateChecksum("ACC,57203") == checksum1) {
        cc++;
    } else {
        printf("    Failed to return correct CheckSum\n");
    }
    printf("    Passed (%d/1) CheckSum Tests!\n", cc);
    //Parse Message Tests
    printf("\nParse Message Test: \n");
    if (Message_ParseMessage("CHA,43182", "5B", &sample1) == STANDARD_ERROR) {
        pm++;
    } else {
        printf("    Failed Payload not Equal to CheckSum Test!\n");
    }
    if (Message_ParseMessage("CHA,", "66", &sample1) == STANDARD_ERROR) {
        pm++;
    } else {
        printf("    FAILED Invalid Payload Test!\n");
    } 
    if (Message_ParseMessage("CHA,43182", "5A", &sample1) == SUCCESS || 
            Message_ParseMessage("RES,1,0,3", "5A", &sample1) == SUCCESS) {
        pm++;
    } else {
        printf("    Failed Parse Message Test!\n");
    }
    printf("    Passed (%d/3) Parse Message Tests!\n", pm);
    //Encode Tests
    printf("\nEncode Message Tests: \n");
    if (Message_Encode(str1, sample2) == EncodeLen1) {
        e++;
    } else {
        printf("    Failed Test: Returns Incorrect String Length!\n");
    } 
    if (Message_Encode(str2, sample3) == 0) {
        e++;
    } else {
        printf("    Failed Test: Does Not Return %c0%c Given MessageNone!\n", q, q);
    }
    printf("    Passed (%d/2) Encode Message Tests!\n", e);
    
    //Decode Tests
    printf("\nDecode Message Tests:\n");
    Message_Decode('$', &sample4);
    Message_Decode('C', &sample4);
    Message_Decode('H', &sample4);
    Message_Decode('A', &sample4);
    Message_Decode(',', &sample4);
    Message_Decode('4', &sample4);
    Message_Decode('3', &sample4);
    Message_Decode('1', &sample4);
    Message_Decode('8', &sample4);
    Message_Decode('2', &sample4);
    Message_Decode('*', &sample4);
    Message_Decode('5', &sample4);
    Message_Decode('A', &sample4);
    Message_Decode('\n', &sample4);
    if (sample4.type == BB_EVENT_CHA_RECEIVED) {
        dc++;
    } else {
        printf("    Failed Test: Decode Failure!\n");
    }
    Message_Decode('$', &sample4);
    Message_Decode('C', &sample4);
    Message_Decode('H', &sample4);
    Message_Decode('A', &sample4);
    Message_Decode(',', &sample4);
    Message_Decode('4', &sample4);
    Message_Decode('3', &sample4);
    Message_Decode('1', &sample4);
    Message_Decode('8', &sample4);
    Message_Decode('2', &sample4);
    Message_Decode('*', &sample4);
    Message_Decode('5', &sample4);
    Message_Decode('\n', &sample4);
    Message_Decode('\n', &sample4);
    if (sample4.type == BB_EVENT_NO_EVENT) {
        dc++;
    } else {
        printf("    Failed Test: Didn't Return BB_EVENT_ERROR!\n");
    }
    if (sample4.param0 == BB_ERROR_BAD_CHECKSUM) {
        dc++;
    } else {
        printf("    Failed Test: param0 doesn't hold CheckSum Len Insufficient!\n");
    }
    printf("    Passed (%d/3) Decode Message Tests!\n", dc);
    
    BOARD_End();
    while(1);
    return(0);
}