/* math_packet.cpp */

// Define ways build and use a math packet

#include "math_packet.h"

#include <vector>

using namespace std;

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

Packet wrap_header(Packet math_packet, u_int16_t number_of_operands) {
	// TODO
}

Packet make_packet_from_expression(std::string math_expression) {
	ReversePolishExpression rpe(math_expression);
	Packet math_packet = rpe.conv_to_packet();
	return wrap_header(math_packet,rpe.get_number_of_operands());
}

Packet make_answer_packet(u_char* request_packet) {
	// TODO
}