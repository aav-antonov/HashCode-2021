/*
 * structures.h
 *
 *  Created on: Aug 23, 2022
 *      Author: aav
 */

#ifndef FIXEDSID_H_
#define FIXEDSID_H_




//--------------- FIXEDSID --------------------------//
// FIXEDSID - fixed street for FirstInFirstGreen optimization , eqvivalent to Mutation 
typedef struct
{
    int nid; // node id
    int sid; // sid = nodesSchedule[nodesIndex[nid] + i]
    int i;   // i - fixed position of SID in nodesSchedule
    int criteria;
    int score;  
} FIXEDSID;

//----------END FIXEDSID ------------------//






#endif /* FIXEDSID_H_ */





