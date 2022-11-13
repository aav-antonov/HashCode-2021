/*
 * structures.h
 *
 *  Created on: Aug 23, 2022
 *      Author: aav
 */

#ifndef MUTATION_H_
#define MUTATION_H_



//------------- MUTATION ----------------------------//
// MUTATION - specify mutation in schedule i1->i2

typedef struct
{
    int nid;        // nid - node id, i1,i2 - permutational indexs in  *nodesSchedule
    int  i1;        // sid1 = nodesSchedule[nodesIndex[nid] + i1]
    int  i2;        // sid2 = nodesSchedule[nodesIndex[nid] + i2]
    int criteria;    
    int score;      // sid2 = nodesSchedule[nodesIndex[nid] + i2]

} MUTATION;
//------------END MUTATION    ------------//




#endif /* MUTATION_H_ */





