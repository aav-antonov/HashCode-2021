/*
 * structures.h
 *
 *  Created on: Aug 23, 2022
 *      Author: aav
 */

#ifndef CAR_H_
#define CAR_H_



//--------------- CAR --------------------------//
// typedef struct CAR - state of the car at current time: 

typedef struct
{
    int i;    // i - index of current street
    int sid;  // sid - id of current street
    int l;    // l - distnace to junction, -1 -> at junction
    int q;    // q - junction query (q = 0 -> first, 1 -> second, ..., -1 -> othervice)
} CAR;

//------------END CAR    ------------//




#endif /* CAR_H_ */





