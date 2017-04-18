/*
 * messages.h
 *
 *  Created on: 7. apr. 2017
 *      Author: GladeEdderkop
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

void put_msg_event( INT8U id, INT8U event);

INT8U get_msg_event( INT8U id);

void put_msg_data(INT8U id, INT16S data );
INT16S get_msg_data(INT8U id);

#endif /* MESSAGES_H_ */
