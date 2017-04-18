/*
 * messages.c
 *
 *  Created on: 7. apr. 2017
 *      Author: GladeEdderkop
 */
#include "emp_type.h"
#include "events.h"
#define MAX_MSG 32

INT8U msg_event[MAX_MSG];
INT16S msg_data[MAX_MSG];

void put_msg_event( INT8U id, INT8U event)
{
    if(id < MAX_MSG)
        msg_event[id] = event;
}

INT8U get_msg_event( INT8U id)
{
    INT8U result = GE_NO_EVENT;
    if(id < MAX_MSG)
    {
      result = msg_event[id];
      msg_event[id] = GE_NO_EVENT;
    }
    return result;
}

void put_msg_data(INT8U id, INT16S data )
{
    if(id < MAX_MSG)
           msg_data[id] = data;
}
INT16S get_msg_data(INT8U id)
{
    INT16S result = 0;
    if(id < MAX_MSG)
    {
      result = msg_data[id];
      msg_event[id] = 0;
    }
    return result;
}
