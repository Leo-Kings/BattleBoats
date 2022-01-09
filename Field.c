//NEO AGOJO
//iagojo@ucsc.edu

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Field.h"
#include "BOARD.h"

#define SMALL 3
#define MEDIUM 4
#define LARGE 5
#define HUGE 6
#define DIR 2

/**
 * This function is optional, but recommended.   It prints a representation of both
 * fields, similar to the OLED display.
 * @param f The field to initialize.
 * @param p The data to initialize the entire field to, should be a member of enum
 *                     SquareStatus.
 */
void FieldPrint_UART(Field *own_field, Field * opp_field);

/**
 * FieldInit() will initialize two passed field structs for the beginning of play.
 * Each field's grid should be filled with the appropriate SquareStatus (
 * FIELD_SQUARE_EMPTY for your own field, FIELD_SQUARE_UNKNOWN for opponent's).
 * Additionally, your opponent's field's boatLives parameters should be filled
 *  (your own field's boatLives will be filled when boats are added)
 * 
 * FieldAI_PlaceAllBoats() should NOT be called in this function.
 * 
 * @param own_field     //A field representing the agents own ships
 * @param opp_field     //A field representing the opponent's ships
 */
void FieldInit(Field *own_field, Field *opp_field) {
    uint8_t col;
    uint8_t row;

    for (col = FIELD_COLS; col != 0; col--) {
        own_field -> grid[row][col] = FIELD_SQUARE_EMPTY;
        opp_field -> grid[row][col] = FIELD_SQUARE_UNKNOWN;
        for (row = FIELD_ROWS; row != 0; row--) {
            own_field -> grid[row][col] = FIELD_SQUARE_EMPTY;
            opp_field -> grid[row][col] = FIELD_SQUARE_UNKNOWN;
        }
    }
    opp_field -> smallBoatLives = SMALL;
    opp_field -> mediumBoatLives = MEDIUM;
    opp_field -> largeBoatLives = LARGE;
    opp_field -> hugeBoatLives = HUGE;

    own_field -> smallBoatLives = 0;
    own_field -> mediumBoatLives = 0;
    own_field -> largeBoatLives = 0;
    own_field -> hugeBoatLives = 0;
}

/**
 * Retrieves the value at the specified field position.
 * @param f     //The Field being referenced
 * @param row   //The row-component of the location to retrieve
 * @param col   //The column-component of the location to retrieve
 * @return  FIELD_SQUARE_INVALID if row and col are not valid field locations
 *          Otherwise, return the status of the referenced square 
 */
SquareStatus FieldGetSquareStatus(const Field *f, uint8_t row, uint8_t col) {
    static SquareStatus p;
    p = f -> grid[row][col];
    if ((col > 10 || col < 0) || (row > 6 || row < 0)) {
        return FIELD_SQUARE_INVALID;
    }
    if (f -> grid[row][col] == FIELD_SQUARE_INVALID) {
        return FIELD_SQUARE_INVALID;
    }
    return p;
}

/**
 * This function provides an interface for setting individual locations within a Field struct. This
 * is useful when FieldAddBoat() doesn't do exactly what you need. For example, if you'd like to use
 * FIELD_SQUARE_CURSOR, this is the function to use.
 * 
 * @param f The Field to modify.
 * @param row The row-component of the location to modify
 * @param col The column-component of the location to modify
 * @param p The new value of the field location
 * @return The old value at that field location
 */
SquareStatus FieldSetSquareStatus(Field *f, uint8_t row, uint8_t col, SquareStatus p) {
    SquareStatus OldValue;
    OldValue = f -> grid[row][col];
    f -> grid[row][col] = p;
    return OldValue;
}

/**
 * FieldAddBoat() places a single ship on the player's field based on arguments 2-5. Arguments 2, 3
 * represent the x, y coordinates of the pivot point of the ship.  Argument 4 represents the
 * direction of the ship, and argument 5 is the length of the ship being placed. 
 * 
 * All spaces that
 * the boat would occupy are checked to be clear before the field is modified so that if the boat
 * can fit in the desired position, the field is modified as SUCCESS is returned. Otherwise the
 * field is unmodified and STANDARD_ERROR is returned. There is no hard-coded limit to how many
 * times a boat can be added to a field within this function.
 * 
 * In addition, this function should update the appropriate boatLives parameter of the field.
 *
 * So this is valid test code:
 * {
 *   Field myField;
 *   FieldInit(&myField,FIELD_SQUARE_EMPTY);
 *   FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_TYPE_SMALL);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_TYPE_MEDIUM);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_TYPE_HUGE);
 *   FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_TYPE_SMALL);
 * }
 *
 * should result in a field like:
 *      0 1 2 3 4 5 6 7 8 9
 *     ---------------------
 *  0 [ 3 3 3 . . . 3 . . . ]
 *  1 [ 4 4 4 4 . . 3 . . . ]
 *  2 [ . . . . . . 3 . . . ]
 *  3 [ . . . . . . . . . . ]
 *  4 [ . . . . . . . . . . ]
 *  5 [ . . . . . . . . . . ]
 *     
 * @param f The field to grab data from.
 * @param row The row that the boat will start from, valid range is from 0 and to FIELD_ROWS - 1.
 * @param col The column that the boat will start from, valid range is from 0 and to FIELD_COLS - 1.
 * @param dir The direction that the boat will face once places, from the BoatDirection enum.
 * @param boatType The type of boat to place. Relies on the FIELD_SQUARE_*_BOAT values from the
 * SquareStatus enum.
 * @return SUCCESS for success, STANDARD_ERROR for failure
 */
uint8_t FieldAddBoat(Field *own_field, uint8_t row, uint8_t col, BoatDirection dir,
        BoatType boat_type) {
    int i;
    int j;
    if (FieldGetSquareStatus(own_field, row, col) == FIELD_SQUARE_INVALID) {
        return STANDARD_ERROR;
    }
    switch (dir) {
        case FIELD_DIR_SOUTH:
            if (boat_type == FIELD_BOAT_TYPE_SMALL) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row + 1][col] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row + 2][col] == FIELD_SQUARE_EMPTY) {
                            for (i = 0; i < SMALL; i++) {
                                own_field -> grid[row + i][col] = FIELD_SQUARE_SMALL_BOAT;
                                return SUCCESS;
                            }
                        }
                    }
                }
            }
            if (boat_type == FIELD_BOAT_TYPE_MEDIUM) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row + 1][col] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row + 2][col] == FIELD_SQUARE_EMPTY) {
                            if (own_field -> grid[row + 3][col] == FIELD_SQUARE_EMPTY) {
                                for (i = 0; i < MEDIUM; i++) {
                                    own_field -> grid[row + i][col] = FIELD_SQUARE_MEDIUM_BOAT;
                                    return SUCCESS;
                                }
                            }
                        }
                    }
                }
            }
            if (boat_type == FIELD_BOAT_TYPE_LARGE) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row + 1][col] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row + 2][col] == FIELD_SQUARE_EMPTY) {
                            if (own_field -> grid[row + 3][col] == FIELD_SQUARE_EMPTY) {
                                if (own_field -> grid[row + 4][col] == FIELD_SQUARE_EMPTY) {
                                    for (i = 0; i < LARGE; i++) {
                                        own_field -> grid[row + i][col] = FIELD_SQUARE_LARGE_BOAT;
                                        return SUCCESS;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (boat_type == FIELD_BOAT_TYPE_HUGE) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row + 1][col] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row + 2][col] == FIELD_SQUARE_EMPTY) {
                            if (own_field -> grid[row + 3][col] == FIELD_SQUARE_EMPTY) {
                                if (own_field -> grid[row + 4][col] == FIELD_SQUARE_EMPTY) {
                                    if (own_field -> grid[row + 5][col] == FIELD_SQUARE_EMPTY) {
                                        for (i = 0; i < HUGE; i++) {
                                            own_field -> grid[row + i][col] =
                                                    FIELD_SQUARE_HUGE_BOAT;
                                            return SUCCESS;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (own_field -> grid[row][col] != FIELD_SQUARE_EMPTY) {
                return STANDARD_ERROR;
            }
            break;
        case FIELD_DIR_EAST:
            if (boat_type == FIELD_BOAT_TYPE_SMALL) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row][col + 1] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row][col + 2] == FIELD_SQUARE_EMPTY) {
                            for (j = 0; j < SMALL; j++) {
                                own_field -> grid[row][col + j] = FIELD_SQUARE_SMALL_BOAT;
                                return SUCCESS;
                            }
                        }
                    }
                }
            }
            if (boat_type == FIELD_BOAT_TYPE_MEDIUM) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row][col + 1] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row][col + 2] == FIELD_SQUARE_EMPTY) {
                            if (own_field -> grid[row][col + 3] == FIELD_SQUARE_EMPTY) {
                                for (j = 0; j < MEDIUM; j++) {
                                    own_field -> grid[row][col + j] = FIELD_SQUARE_MEDIUM_BOAT;
                                    return SUCCESS;
                                }
                            }
                        }
                    }
                }
            }
            if (boat_type == FIELD_BOAT_TYPE_LARGE) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row][col + 1] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row][col + 2] == FIELD_SQUARE_EMPTY) {
                            if (own_field -> grid[row][col + 3] == FIELD_SQUARE_EMPTY) {
                                if (own_field -> grid[row][col + 4] == FIELD_SQUARE_EMPTY) {
                                    for (j = 0; j < LARGE; j++) {
                                        own_field -> grid[row][col + j] =
                                                FIELD_SQUARE_LARGE_BOAT;
                                        return SUCCESS;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (boat_type == FIELD_BOAT_TYPE_HUGE) {
                if (own_field -> grid[row][col] == FIELD_SQUARE_EMPTY) {
                    if (own_field -> grid[row][col + 1] == FIELD_SQUARE_EMPTY) {
                        if (own_field -> grid[row][col + 2] == FIELD_SQUARE_EMPTY) {
                            if (own_field -> grid[row][col + 3] == FIELD_SQUARE_EMPTY) {
                                if (own_field -> grid[row ][col + 4] == FIELD_SQUARE_EMPTY) {
                                    if (own_field -> grid[row][col + 5] == FIELD_SQUARE_EMPTY) {
                                        for (j = 0; j < HUGE; j++) {
                                            own_field -> grid[row][col + j] =
                                                    FIELD_SQUARE_HUGE_BOAT;
                                            return SUCCESS;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (own_field -> grid[row][col] != FIELD_SQUARE_EMPTY) {
                return STANDARD_ERROR;
            }
            break;
        default:
            return STANDARD_ERROR;
    }
    switch (boat_type) {
        case FIELD_BOAT_TYPE_SMALL:
            own_field -> smallBoatLives = SMALL;
            break;
        case FIELD_BOAT_TYPE_MEDIUM:
            own_field -> mediumBoatLives = MEDIUM;
            break;
        case FIELD_BOAT_TYPE_LARGE:
            own_field -> largeBoatLives = LARGE;
            break;
        case FIELD_BOAT_TYPE_HUGE:
            own_field -> hugeBoatLives = HUGE;
            break;
        default:
            return STANDARD_ERROR;
    }

    return SUCCESS;
}

/**
 * This function registers an attack at the gData coordinates on the provided field. This means that
 * 'f' is updated with a FIELD_SQUARE_HIT or FIELD_SQUARE_MISS depending on what was at the
 * coordinates indicated in 'gData'. 'gData' is also updated with the proper HitStatus value
 * depending on what happened AND the value of that field position BEFORE it was attacked. Finally
 * this function also reduces the lives for any boat that was hit from this attack.
 * @param f The field to check against and update.
 * @param gData The coordinates that were guessed. The result is stored in gData->result as an
 *               output.  The result can be a RESULT_HIT, RESULT_MISS, or RESULT_***_SUNK.
 * @return The data that was stored at the field position indicated by gData before this attack.
 */
SquareStatus FieldRegisterEnemyAttack(Field *own_field, GuessData *opp_guess) {
    SquareStatus HitStatus = own_field -> grid[opp_guess -> row][opp_guess -> col];
    switch (HitStatus) {
        case FIELD_SQUARE_SMALL_BOAT:
            if (own_field -> smallBoatLives > 0) {
                opp_guess -> result = RESULT_HIT;
                own_field -> smallBoatLives--;
                own_field -> grid[opp_guess -> row][opp_guess -> col] = FIELD_SQUARE_HIT;
            } else {
                opp_guess -> result = RESULT_SMALL_BOAT_SUNK;
            }
            break;
        case FIELD_SQUARE_MEDIUM_BOAT:
            if (own_field -> mediumBoatLives > 0) {
                opp_guess -> result = RESULT_HIT;
                own_field -> mediumBoatLives--;
                own_field -> grid[opp_guess -> row][opp_guess -> col] = FIELD_SQUARE_HIT;
            } else {
                opp_guess -> result = RESULT_MEDIUM_BOAT_SUNK;
            }
            break;
        case FIELD_SQUARE_LARGE_BOAT:
            if (own_field -> largeBoatLives > 0) {
                opp_guess -> result = RESULT_HIT;
                own_field -> largeBoatLives--;
                own_field -> grid[opp_guess -> row][opp_guess -> col] = FIELD_SQUARE_HIT;
            } else {
                opp_guess -> result = RESULT_LARGE_BOAT_SUNK;
            }
            break;
        case FIELD_SQUARE_HUGE_BOAT:
            if (own_field -> hugeBoatLives > 0) {
                opp_guess -> result = RESULT_HIT;
                own_field -> hugeBoatLives--;
                own_field -> grid[opp_guess -> row][opp_guess -> col] = FIELD_SQUARE_HIT;
            } else {
                opp_guess -> result = RESULT_HUGE_BOAT_SUNK;
            }
            break;
        case FIELD_SQUARE_EMPTY:
            own_field -> grid[opp_guess -> row][opp_guess -> col] = FIELD_SQUARE_MISS;
            opp_guess -> result = RESULT_MISS;
            break;
            //case FIELD_SQUARE_HIT:
            //own_field -> grid[opp_guess -> row][opp_guess -> col] = FIELD_SQUARE_INVALID;
        default:
            return STANDARD_ERROR;
    }
    return HitStatus;
}

/**
 * This function updates the FieldState representing the opponent's game board with whether the
 * guess indicated within gData was a hit or not. If it was a hit, then the field is updated with a
 * FIELD_SQUARE_HIT at that position. If it was a miss, display a FIELD_SQUARE_EMPTY instead, as
 * it is now known that there was no boat there. The FieldState struct also contains data on how
 * many lives each ship has. Each hit only reports if it was a hit on any boat or if a specific boat
 * was sunk, this function also clears a boats lives if it detects that the hit was a
 * RESULT_*_BOAT_SUNK.
 * @param f The field to grab data from.
 * @param gData The coordinates that were guessed along with their HitStatus.
 * @return The previous value of that coordinate position in the field before the hit/miss was
 * registered.
 */
SquareStatus FieldUpdateKnowledge(Field *opp_field, const GuessData *own_guess) {
    SquareStatus HitStatus = opp_field -> grid[own_guess -> row][own_guess -> col];
    switch (own_guess -> result) {
        case RESULT_HIT:
            opp_field -> grid[own_guess -> row][own_guess -> col] = FIELD_SQUARE_HIT;
            break;
        case RESULT_MISS:
            opp_field -> grid[own_guess -> row][own_guess -> col] = FIELD_SQUARE_EMPTY;
            break;
        case RESULT_SMALL_BOAT_SUNK:
            opp_field -> smallBoatLives = 0;
            opp_field -> grid[own_guess -> row][own_guess -> col] = FIELD_SQUARE_HIT;
            break;
        case RESULT_MEDIUM_BOAT_SUNK:
            opp_field -> mediumBoatLives = 0;
            opp_field -> grid[own_guess -> row][own_guess -> col] = FIELD_SQUARE_HIT;
            break;
        case RESULT_LARGE_BOAT_SUNK:
            opp_field -> largeBoatLives = 0;
            opp_field -> grid[own_guess -> row][own_guess -> col] = FIELD_SQUARE_HIT;
            break;
        case RESULT_HUGE_BOAT_SUNK:
            opp_field -> hugeBoatLives = 0;
            opp_field -> grid[own_guess -> row][own_guess -> col] = FIELD_SQUARE_HIT;
            break;
        default:
            return STANDARD_ERROR;
    }
    return HitStatus;
}

/**
 * This function returns the alive states of all 4 boats as a 4-bit bitfield (stored as a uint8).
 * The boats are ordered from smallest to largest starting at the least-significant bit. So that:
 * 0b00001010 indicates that the small boat and large boat are sunk, while the medium and huge boat
 * are still alive. See the BoatStatus enum for the bit arrangement.
 * @param f The field to grab data from.
 * @return A 4-bit value with each bit corresponding to whether each ship is alive or not.
 */
uint8_t FieldGetBoatStates(const Field *f) {
    uint8_t BoatStatus = 0;
    if (f -> smallBoatLives > 0) {
        BoatStatus += FIELD_BOAT_STATUS_SMALL;
    }
    if (f -> mediumBoatLives > 0) {
        BoatStatus += FIELD_BOAT_STATUS_MEDIUM;
    }
    if (f -> largeBoatLives > 0) {
        BoatStatus += FIELD_BOAT_STATUS_LARGE;
    }
    if (f -> hugeBoatLives > 0) {
        BoatStatus += FIELD_BOAT_STATUS_HUGE;
    }
    return BoatStatus;
}

/**
 * This function is responsible for placing all four of the boats on a field.
 * 
 * @param f         //agent's own field, to be modified in place.
 * @return SUCCESS if all boats could be placed, STANDARD_ERROR otherwise.
 * 
 * This function should never fail when passed a properly initialized field!
 */
uint8_t FieldAIPlaceAllBoats(Field *own_field) {
    int counter = 0;
    int row;
    int col;
    BoatDirection dir;
    while (1) {
        row = (rand() % (FIELD_ROWS + 1));
        col = (rand() % (FIELD_COLS + 1));
        dir = (rand() % DIR);
        if ((FieldAddBoat(own_field, row, col, dir, FIELD_BOAT_TYPE_SMALL) == SUCCESS) && (counter == 0)) {
            counter++;
        } else if ((FieldAddBoat(own_field, row, col, dir, FIELD_BOAT_TYPE_MEDIUM) == SUCCESS) && (counter == 1)) {
            counter++;
        } else if ((FieldAddBoat(own_field, row, col, dir, FIELD_BOAT_TYPE_LARGE) == SUCCESS) && (counter == 2)) {
            counter++;
        } else if ((FieldAddBoat(own_field, row, col, dir, FIELD_BOAT_TYPE_HUGE) == SUCCESS) && (counter == 3)) {
            counter++;
        } else if (counter == 4) {
            return SUCCESS;
            break;
        }
    }
    return SUCCESS;
}

/**
 * Given a field, decide the next guess.
 *
 * This function should not attempt to shoot a square which has already been guessed.
 *
 * You may wish to give this function static variables.  If so, that data should be
 * reset when FieldInit() is called.
 * 
 * @param f an opponent's field.
 * @return a GuessData struct whose row and col parameters are the coordinates of the guess.  The 
 *           result parameter is irrelevant.
 */
GuessData FieldAIDecideGuess(const Field *opp_field) {
    int row;
    int col;
    GuessData ownGuess;
    while (1) {
        row = (rand() % (FIELD_ROWS + 1));
        col = (rand() % (FIELD_COLS + 1));
        if ((opp_field -> grid[row][col] == FIELD_SQUARE_HIT) && (opp_field -> grid[row][col] 
                == FIELD_SQUARE_MISS)) {
            continue;
        } else {
            ownGuess.col = col;
            ownGuess.row = row;
            break;
        }
    }
    return ownGuess;
}

 /*     0 1 2 3 4 5 6 7 8 9
 *     ---------------------
 *  0 [ 3 3 3 . . . 3 . . . ]
 *  1 [ 4 4 4 4 . . 3 . . . ]
 *  2 [ . . . . . . 3 . . . ]
 *  3 [ . . . . . . . . . . ]
 *  4 [ . . . . . . . . . . ]
 *  5 [ . . . . . . . . . . ]
 */