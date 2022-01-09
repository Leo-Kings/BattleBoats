
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "BattleBoats.h"
#include "Field.h"
#include "FieldOled.h"
#include "Oled.h"
#include "OledDriver.h"
#include "BOARD.h"
#define str(x) #x
#define print(x) printf("    Failed Test: %s\n", ((str(x))))

Field own;
Field opponent;

/*typedef struct {
    uint8_t grid[FIELD_ROWS][FIELD_COLS];
    uint8_t smallBoatLives;
    uint8_t mediumBoatLives;
    uint8_t largeBoatLives;
    uint8_t hugeBoatLives;
} Field;*/
int Init = 0, FGSS = 0, FSSS = 0, FAB = 0, FREA = 0, FUK = 0, FAPAB = 0, FADG = 0;

int main() {
    BOARD_Init();
    //Field Init Test:
    printf("Field Initialized Tests:\n");
    FieldInit(&own, &opponent);
    if (own.grid[0][0] == FIELD_SQUARE_EMPTY) {
        Init++;
    } else {
        print(Your Field is not Initialized!);
    }
    if (opponent.grid[1][2] == FIELD_SQUARE_UNKNOWN) {
        Init++;
    } else {
        print(Their Field is not Initialized!\n);
    }
    if (opponent.hugeBoatLives == FIELD_BOAT_SIZE_HUGE) {
        Init++;
    } else {
        print(The Opponent Boat Lives Not Initialize as Full!);
    }
    printf("    Passed (%d/3) Tests for Initialize Test!\n", Init);
    
    //FieldGetSquareStatus Test
    printf("\nFieldGetSquareStatus Tests:\n");
    if (FieldGetSquareStatus(&opponent, 1, 1) == FIELD_SQUARE_UNKNOWN) {
        FGSS++;
    } else {
        print(Failed to Get Square Unknown);
    }
    if (FieldGetSquareStatus(&opponent, 10, 10) == FIELD_SQUARE_INVALID){
        FGSS++;
    } else {
        print(Failed to Return Invalid Square!);
    }
    printf("    Passed (%d/2) Tests for FieldGetSquareStatus Test!\n", FGSS);
    
    //FieldSetSquareStatus Tests:
    printf("\nFieldSetSquareStatus Tests:\n");
    if (FieldSetSquareStatus(&opponent, 5, 5, FIELD_SQUARE_HIT) == FIELD_SQUARE_UNKNOWN) {
        FSSS++;
    } else {
        print(Did Not Return Previous Square);
    }
    if (FieldGetSquareStatus(&opponent, 5, 5) == FIELD_SQUARE_HIT) {
        FSSS++;
    } else {
        print(Did Not Set Square to FIELD_SQUARE_HIT);
    }
    printf("    Passed (%d/2) Tests for FieldSetSquareStatus Test!\n", FSSS);
    
    //FIELD ADD Boat Tests:
    printf("\nFieldAddBoat Tests:\n");
    int i = 0, j = 0;
    FieldAddBoat(&own, 0, 0, FIELD_DIR_EAST, FIELD_BOAT_TYPE_SMALL);
    FieldAddBoat(&own, 0, 6, FIELD_DIR_SOUTH, FIELD_BOAT_TYPE_SMALL);
    FieldAddBoat(&own, 1, 0, FIELD_DIR_EAST, FIELD_BOAT_TYPE_MEDIUM);
    FieldAddBoat(&own, 2, 0, FIELD_DIR_EAST, FIELD_BOAT_TYPE_HUGE);
    while (i < FIELD_BOAT_SIZE_SMALL) {
        if (FieldGetSquareStatus(&own, 0, i) == FIELD_SQUARE_SMALL_BOAT) {
            if (i == FIELD_BOAT_SIZE_SMALL - 1) {
                FAB++;
            }
            i++;
        } else {
            print(A Column Square Is Not Set to Small Boat);
            break;
        }
    }
    
    while (j < FIELD_BOAT_SIZE_SMALL) {
        if (FieldGetSquareStatus(&own, j, 6) == FIELD_SQUARE_SMALL_BOAT) {
            if (j == FIELD_BOAT_SIZE_SMALL - 1) {
                FAB++;
            }
            j++;
        } else {
            print(A Row Square Is Not Set to Small Boat);
            break;
        }
    }
    printf("    Passed (%d/2) Test for Field Add Boat\n", FAB);
    //Register Enemy Attack
    GuessData hit = {0, 0, RESULT_MISS};
    GuessData missed = {0, 5, RESULT_HIT};
    printf("\nFieldRegisterEnemyAttack Test:\n");
    SquareStatus HitReceived = FieldRegisterEnemyAttack(&own, &hit);
    FieldRegisterEnemyAttack(&own, &missed);
    if (hit.result == RESULT_HIT) {
        FREA++;
    } else {
        print(Did not Edit Guess Data to get Hit);
    }
    if (missed.result == RESULT_MISS) {
        FREA++;
    } else {
        print(Did not Edit Guess Data to get Miss);
    }
    if (HitReceived == FIELD_SQUARE_SMALL_BOAT) {
        FREA++;
    } else {
        print(Does not Return What Type of Boat Hit);
    }
    printf("    Passed (%d/3) Tests for Field RegisterEnemyAttack\n", FREA);
    //FieldUpdateKnowledge Test
    printf("\nFieldUpdateKnowledge Tests:\n");
    FieldAddBoat(&opponent, 0, 0, FIELD_DIR_EAST, FIELD_BOAT_TYPE_SMALL);
    GuessData ownGuessHit = {0, 0, RESULT_MISS};
    GuessData ownGuessMiss = {0, 1, RESULT_HIT};
    FieldUpdateKnowledge(&opponent, &ownGuessHit);
    SquareStatus MissUpdate = FieldUpdateKnowledge(&opponent, &ownGuessMiss);
    if (FieldGetSquareStatus(&opponent, 0, 1) == FIELD_SQUARE_HIT) {
        FUK++;
    } else {
        print(Did not Update your Field for a Hit Result);
    }
    if (FieldGetSquareStatus(&opponent, 0, 0) == FIELD_SQUARE_EMPTY) {
        FUK++;
    } else {
        print(Did not Update your Field for a Miss Result);
    }
    if (MissUpdate == FIELD_SQUARE_UNKNOWN) {
        FUK++;
    } else {
        print(FIELD SQUARE DOES NOT RETURN UNKNOWN!);
    }
    printf("    Passed (%d/3) Tests for FieldUpdateKnowledge\n", FUK);
    //FieldGetBoatStates Test:
    printf("\nFieldGetBoatStates Test:\n");
    uint8_t status = FieldGetBoatStates(&own);
    if (status == 0x0B) {
        printf("    Passed (1/1) Test for FieldGetBoatStates Test\n");
    } else {
        print(Wrong Boat States);
    }
    //FieldPlaceAllBoats
    printf("\nFieldAIPlaceAllBoats Tests:\n");
    FieldInit(&own, &opponent);
    int result = FieldAIPlaceAllBoats(&own);
    if (result == SUCCESS) {
        FAPAB++;
    } else {
        print(Did Not Place All Boats);
    }
    if (FieldGetBoatStates(&own) == 0x0F) {
        FAPAB++;
    } else {
        print(Not Enough Boats Alive);
    }
    printf("    Passed (%d/2) Tests for FieldAIPlaceAllBoats\n", FAPAB);
    //FieldAIDecideGuess Test;
    printf("\nFieldAIDecideGuess Tests:\n");
    FieldUpdateKnowledge(&opponent, &ownGuessHit);
    FieldUpdateKnowledge(&opponent, &ownGuessMiss);
    GuessData AIGuess = FieldAIDecideGuess(&opponent); 
    if ((AIGuess.col >= 0 && AIGuess.col <= FIELD_COLS) && 
            (AIGuess.row >= 0 && AIGuess.row <= FIELD_ROWS)) {
        printf("    Passed (1/1) Tests For FieldAIDecideGuess\n");
        printf("AI Guessed: (%d, %d)", AIGuess.row, AIGuess.col);
    } else {
        print(AI made No Guesses);
    }
    
    BOARD_End();
    while(1);
    return 0;
}

