/* math_packet.h */

// Define ways build and use a math packet

#ifndef MATH_PACKET_H
#define MATH_PACKET_H

#include <pcap.h>
#include <algorithm>
#include <string>

const u_int8_t
	MATH_TYPE_REQUEST     = 1, // Request for an expression to be solved
	MATH_TYPE_ACK_REQUEST = 2, // Acknowledge request and start solving
	MATH_TYPE_SEND_ANSWER = 4, // Send answer
	MATH_TYPE_ACK_ANSWER  = 8; // Acknowledge received answer

const u_int32_t MATH_MAGIC = 9770010;

struct MathPacketHeader {
	u_int32_t magic_number; // Must be set to MATH_MAGIC
	u_int8_t type_of_packet; // MATH_TYPE_*
	u_int32_t user_id_of_requester; // Single unique integer, upto 4294967295
	u_int32_t user_id_of_sender; // Single unique integer, upto 4294967295
	u_int32_t request_id; // Single unique integer, upto 4294967295
	u_int16_t number_of_operands;
};

const u_int8_t
	MATH_OPERATOR_PLUS        = 1,
	MATH_OPERATOR_MINUS       = 2,
	MATH_OPERATOR_MULTIPLY    = 3,
	MATH_OPERATOR_DIVIDE      = 4,
	MATH_OPERATOR_MODULO      = 5,
	MATH_OPERATOR_BITWISE_AND = 6,
	MATH_OPERATOR_BITWISE_OR  = 7,
	MATH_OPERATOR_BITWISE_XOR = 8;

/*

The MathPacket is defined by the following:

int32_t operands[number_of_operands];
u_int8_t operators[number_of_operands-1]; // Defined by the MATH_OPERATOR_* constants
u_int8_t number_of_operators_after_operand[number_of_operands]; // The positions of the operators is as required for Reverse Polish Notation.
int32_t answer; // Expected to be 0 when requesting for an answer
u_int16_t end_packet_magic_number; // Must be set to 21845

*/

typedef std::pair<u_char*,int> Packet;

Packet make_packet_from_expression(std::string math_expression);
void make_ack_packet(Packet &packet);
Packet make_answer_packet(u_char* request_packet);
int read_answer(Packet &packet);

#endif
