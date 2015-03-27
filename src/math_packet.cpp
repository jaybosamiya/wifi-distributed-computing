/* math_packet.cpp */

// Define ways build and use a math packet

#include "math_packet.h"
#include "util.h"

#include <vector>

using namespace std;

extern u_int32_t own_id;

class ReversePolishExpression {
	vector<int32_t> operands;
	vector<u_int8_t> operators;
	vector<u_int8_t> number_of_operators_after_operand;
public:
	ReversePolishExpression(std::string math_expression) {
		// TODO
	}
	ReversePolishExpression(u_char* math_packet, u_int16_t number_of_operands) {
		// TODO
	}
	int32_t solve() {
		return 0;
		// TODO
	}
	Packet conv_to_packet(int ans) {
		// TODO
	}
	Packet conv_to_packet() {
		// TODO
	}
	u_int16_t get_number_of_operands() const {
		return operands.size();
	}
};

void make_ack_packet(Packet &packet) {
	MathPacketHeader *mph = (MathPacketHeader*) packet.first;
	if ( mph->type_of_packet == MATH_TYPE_REQUEST )
		mph->type_of_packet = MATH_TYPE_ACK_REQUEST;
	else if ( mph->type_of_packet == MATH_TYPE_SEND_ANSWER )
		mph->type_of_packet = MATH_TYPE_ACK_ANSWER;
}

Packet wrap_header(Packet math_packet, u_int16_t number_of_operands, u_int8_t type_of_packet) {
	MathPacketHeader mph;
	mph.magic_number = 9770010;
	mph.type_of_packet = type_of_packet;
	mph.user_id_of_requester = own_id;
	mph.user_id_of_sender = 0;
	mph.request_id = generate_random(1,4294967295);
	mph.number_of_operands = number_of_operands;
	Packet ret;
	ret.second = math_packet.second + sizeof(mph);
	ret.first = new u_char[ret.second];

	int header_size = sizeof(mph);
	int math_packet_size = math_packet.second;

	for ( int i = 0 ; i < header_size ; i++ ) {
		ret.first[i] = ((u_char*)&mph)[i];
	}
	for ( int i = 0 ; i < math_packet_size ; i++ ) {
		ret.first[i+header_size] = math_packet.first[i];
	}

	return ret;
}

Packet make_packet_from_expression(std::string math_expression) {
	ReversePolishExpression rpe(math_expression);
	Packet wrapped_packet = wrap_header(rpe.conv_to_packet(),rpe.get_number_of_operands(),MATH_TYPE_REQUEST);
	return wrapped_packet;
}

Packet make_answer_packet(u_char* request_packet) {
	// TODO
}