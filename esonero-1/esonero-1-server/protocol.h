/*
 * protocol.h
 *
 *  Created on: 20 nov 2024
 *      Author: pierd
 */
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#ifdef MIN_LENGTH
#undef MIN_LENGTH
#endif

#ifdef MAX_LENGTH
#undef MAX_LENGTH
#endif

#ifdef PROTOCOL_PORT
#undef PROTOCOL_PORT
#endif

#ifdef DATA_MAX_LEN
#undef DATA_MAX_LEN
#endif

#ifdef TRANSPORT_PROTOCOL
#undef TRANSPORT_PROTOCOL
#endif

#define MIN_LENGTH 6
#define MAX_LENGTH 32
#define PROTOCOL_PORT 59690
#define DATA_MAX_LEN 999
#define TRANSPORT_PROTOCOL TCP

#endif /* PROTOCOL_H_ */
