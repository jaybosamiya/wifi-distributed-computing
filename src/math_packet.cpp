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
	pair<u_char*,int> conv_to_packet(int ans) {
		// TODO
	}
	pair<u_char*,int> conv_to_packet() {
		// TODO
	}
};

pair<u_char*,int> wrap_header(pair<u_char*,int> math_packet) {
	// TODO
}

pair<u_char*,int> make_packet_from_expression(std::string math_expression) {
	ReversePolishExpression rpe(math_expression);
	pair<u_char*,int> math_packet = rpe.conv_to_packet();
	return wrap_header(math_packet);
}

pair<u_char*,int> make_answer_packet(u_char* request_packet) {
	// TODO
}