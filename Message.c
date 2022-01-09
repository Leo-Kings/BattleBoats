
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "BattleBoats.h"
#include "Message.h"
#include "BOARD.h"

#define SmallestNum 0x30
#define LargestNum 0x39
#define FirstAlpha 0x41
#define LastAlpha 0x5A
#define delim ","
#define asterisk '*'
#define dollar '$'
uint8_t Message_CalculateChecksum(const char* payload) {
    uint8_t XOR_RES = 0;
    int length = strlen(payload), i;
    for (i = 0; i < length; i++) {
        XOR_RES = XOR_RES ^ payload[i];
    }
    return XOR_RES;
}

int Message_ParseMessage(const char* payload,
        const char* checksum_string, BB_Event * message_event) {
    
    char payload_copy1[MESSAGE_MAX_PAYLOAD_LEN];
    char payload_copy2[MESSAGE_MAX_PAYLOAD_LEN];
    char calculated_checksum_to_str[MESSAGE_CHECKSUM_LEN + 1]; //Self Calculated CheckSum Holder as a str
 
    strcpy(payload_copy1, payload);
    strcpy(payload_copy2, payload);
    char *token1 = strtok(payload_copy1, delim);
    int token_counter = 0, i = 0;
    //Checks if payload does not match checksum and is not 2 char long 
    sprintf(calculated_checksum_to_str, "%X", Message_CalculateChecksum(payload));
    if (strcmp(calculated_checksum_to_str, checksum_string) != 0 ||
            strlen(checksum_string) > MESSAGE_CHECKSUM_LEN) {
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;  
    }
    
    //Makes message_event.type the correct received event
    if (!strcmp(token1, "CHA")) {
        message_event->type = BB_EVENT_CHA_RECEIVED;
    } else if (!strcmp(token1, "ACC")){
        message_event->type = BB_EVENT_ACC_RECEIVED;
    } else if (!strcmp(token1, "REV")){
        message_event->type = BB_EVENT_REV_RECEIVED;
    } else if (!strcmp(token1, "SHO")){
        message_event->type = BB_EVENT_SHO_RECEIVED;
    } else if (!strcmp(token1, "RES")){
        message_event->type = BB_EVENT_RES_RECEIVED;
    } else {
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;  
    }
    //Counts how many times token is used
    while (token1 != NULL) {
        token1 = strtok(NULL, delim);
        token_counter++;
    } 
    if (token_counter < 2) {
        return STANDARD_ERROR;
    }
    
    char *token2;
    token2 = strtok(payload_copy2, delim);
    while(token2 != NULL) {
        if (i == 1) {
            message_event->param0 = (unsigned)atol(token2);
        } else if (i == 2) {
            message_event->param1 = (unsigned)atol(token2);
        } else if (i == 3) {
            message_event->param2 = (unsigned)atol(token2);
        } else if (i > 3) {
            return STANDARD_ERROR;
        }
        token2 = strtok(NULL, delim);
        i++;
    }
    return SUCCESS;
}

int Message_Encode(char *message_string, Message message_to_encode) {
    char payload[MESSAGE_MAX_PAYLOAD_LEN];
    switch (message_to_encode.type) {
        case MESSAGE_CHA:
            sprintf(payload,PAYLOAD_TEMPLATE_CHA, message_to_encode.param0);
            break;
        case MESSAGE_ACC:
            sprintf(payload,PAYLOAD_TEMPLATE_ACC, message_to_encode.param0);
            break;
        case MESSAGE_REV:
            sprintf(payload,PAYLOAD_TEMPLATE_REV, message_to_encode.param0);
            break;
        case MESSAGE_SHO:
            sprintf(payload,PAYLOAD_TEMPLATE_SHO, message_to_encode.param0, 
                    message_to_encode.param1);
            break;   
        case MESSAGE_RES:
            sprintf(payload,PAYLOAD_TEMPLATE_RES, message_to_encode.param0, 
                    message_to_encode.param1, message_to_encode.param2);
            break;    
        default: 
            printf("Tried to encode with invalid message type: %d,%d,%d\n"
                    , message_to_encode.param0, 
                    message_to_encode.param1, message_to_encode.param2);
            return 0;
    }
    sprintf(message_string,MESSAGE_TEMPLATE, payload, Message_CalculateChecksum(payload));
    return strlen(message_string);
}

//MESSAGE DECODE:
typedef enum {
    WAITING,
    RECORDING_PAYLOAD,
    RECORDING_CHECKSUM,        
} DecodeState;
int Message_Decode(unsigned char char_in, BB_Event * decoded_message_event) {
    static DecodeState decState = WAITING;  
    static char payload[MESSAGE_MAX_PAYLOAD_LEN + 1] = "";
    static char checksum[MESSAGE_CHECKSUM_LEN + 1] = "";
    static int i, j;
    switch (decState) {
        case WAITING:
            if (char_in == '$') {
                i = 0, j = 0;
                decState = RECORDING_PAYLOAD;
            } break;
        case RECORDING_PAYLOAD:
            if ((char_in == '$') || (char_in == '\n')) {
                decState = WAITING;
                decoded_message_event->type = BB_EVENT_ERROR;
                decoded_message_event->param0 = BB_ERROR_PAYLOAD_LEN_EXCEEDED;
                return STANDARD_ERROR;
            } else if (strlen(payload) > MESSAGE_MAX_PAYLOAD_LEN + 1){
                decState = WAITING;
                decoded_message_event->type = BB_EVENT_ERROR;
                decoded_message_event->param0 = BB_ERROR_PAYLOAD_LEN_EXCEEDED;
                return STANDARD_ERROR;
            } else if (char_in == '*') {
                decState = RECORDING_CHECKSUM;
            } else {
                payload[i] = char_in;
                i++;
            } break;
        case RECORDING_CHECKSUM:
            if (strlen(checksum) > MESSAGE_CHECKSUM_LEN) {
                decState = WAITING;
                decoded_message_event->type = BB_EVENT_NO_EVENT;
                decoded_message_event->param0 = BB_ERROR_CHECKSUM_LEN_EXCEEDED;
                return STANDARD_ERROR;
            } else if ((char_in >= SmallestNum && char_in <= LargestNum) ||
                    (char_in >= FirstAlpha && char_in <= LastAlpha)) {
                checksum[j] = char_in;
                j++;
            } else if (char_in = '\n' && strlen(checksum) == MESSAGE_CHECKSUM_LEN) {
                int (error) = (Message_ParseMessage(payload, checksum, decoded_message_event));
                if (error == STANDARD_ERROR) {
                    decoded_message_event->type = BB_EVENT_ERROR;
                    decoded_message_event->param0 = BB_ERROR_BAD_CHECKSUM;
                    return STANDARD_ERROR;
                } else {
                    decState = WAITING;
                    return SUCCESS;
                }
            } else {
                decState = WAITING;
                decoded_message_event->type = BB_EVENT_NO_EVENT;
                decoded_message_event->param0 = BB_ERROR_BAD_CHECKSUM;
                return STANDARD_ERROR;
            } break;
        default: 
            decState = WAITING;
            decoded_message_event->type = BB_EVENT_ERROR;
            decoded_message_event->param0 = BB_EVENT_NO_EVENT;
            return STANDARD_ERROR;
    }
    return SUCCESS;
}