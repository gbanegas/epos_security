#ifndef __ieee1451_objects_h

// This work is licensed under the EPOS Software License v1.0.
// A copy of this license is available at the EPOS system source tree root.
// A copy of this license is also available online at:
// http://epos.lisha.ufsc.br/EPOS+Software+License+v1.0
// Note that EPOS Software License applies to both source code and executables.
#define __ieee1451_objects_h

#include <machine.h>

__BEGIN_SYS


struct Command
{
	unsigned short channelNumber;
	unsigned short command;
	unsigned short length;
};

struct Reply
{
	bool success;
	unsigned short length;
};

struct TEDSQueryReply
{
	Reply header;
	unsigned char atributes;
	unsigned char status;
	unsigned long size;
	unsigned short checksum;
	unsigned long maxSize;
};

struct TEDSReadReply
{
	Reply header;
	unsigned long offset;
};

struct DataSetReadReply
{
	Reply header;
	unsigned long offset;
};

enum CommandClass //IEEE 1451.0 (2007) -> Cap. 7
{
	//COMMAND_CLASS_QUERY_TEDS								= 0x0101, //Query TEDS
	//COMMAND_CLASS_UPDATE_TEDS								= 0x0104, //Update TEDS
	COMMAND_CLASS_READ_TEDS_SEGMENT							= 0x0102, //Read TEDS segment
	COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT	= 0x0301, //Read TransducerChannel data-set segment
	COMMAND_CLASS_TRANSDUCER_CHANNEL_OPERATE				= 0x0401, //TransducerChannel Operate
	COMMAND_CLASS_TRANSDUCER_CHANNEL_IDLE					= 0x0402  //TransducerChannel Idle
};

enum AddressClass //IEEE 1451.0 (2007) -> Cap. 5.3
{
	ADDRESS_CLASS_TIM						= 0x0000,
	ADDRESS_CLASS_TRANSDUCER_CHANNEL_FIRST	= 0x0001,
	ADDRESS_CLASS_TRANSDUCER_CHANNEL_LAST	= 0x7FFF,
	ADDRESS_CLASS_GROUP_FIRST				= 0x8000,
	ADDRESS_CLASS_GROUP_LAST				= 0xFFFE,
	ADDRESS_CLASS_GLOBAL					= 0xFFFF
};

#define IEEE1451_PORT 55667

struct IEEE1451_Packet
{
	unsigned short transId;
	unsigned int len;
};

__END_SYS

#endif
