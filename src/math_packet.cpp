/* math_packet.cpp */

// Define ways build and use a math packet

#include "math_packet.h"
#include "util.h"

#include <vector>
#include <stack>

using namespace std;

extern u_int32_t own_id;

class ReversePolishExpression {
	vector<int32_t> operands;
	vector<u_int8_t> operators;
	vector<u_int8_t> number_of_operators_after_operand;

	static const char * read_int ( const char * str, int & v ) {
		v = 0;
		int i = 0;
		while ( ! (str[i] >= '0' && str[i] <= '9') && str[i] ) {
			i++;
		}
		for ( ; str[i] ; i++ ) {
			if ( str[i] >= '0' && str[i] <= '9' ) {
				v *= 10;
				v += (str[i] - '0');
			} else {
				break;
			}
		}
		return (str+i);
	}

	static const int OPEN_BRACKET = -1, CLOSE_BRACKET = -2, END_OF_EXPRESSION = -3;

	static int conv_to_operator(char op) {
		if (op == '+') return MATH_OPERATOR_PLUS;
		if (op == '-') return MATH_OPERATOR_MINUS;
		if (op == '*') return MATH_OPERATOR_MULTIPLY;
		if (op == '/') return MATH_OPERATOR_DIVIDE;
		if (op == '%') return MATH_OPERATOR_MODULO;
		if (op == '&') return MATH_OPERATOR_BITWISE_AND;
		if (op == '|') return MATH_OPERATOR_BITWISE_OR;
		if (op == '^') return MATH_OPERATOR_BITWISE_XOR;
		if (op == '(') return OPEN_BRACKET;
		if (op == ')') return CLOSE_BRACKET;
		if (op == '\0') return END_OF_EXPRESSION;
		error("Unknown operator %c", op);
		abort();
	}

	static int precedence(int op) {
		switch(op) {
			case END_OF_EXPRESSION:
			case OPEN_BRACKET:
			case CLOSE_BRACKET:
				return 0;
			case MATH_OPERATOR_MULTIPLY:
			case MATH_OPERATOR_DIVIDE:
			case MATH_OPERATOR_MODULO:
				return -1;
			case MATH_OPERATOR_PLUS:
			case MATH_OPERATOR_MINUS:
				return -2;
			case MATH_OPERATOR_BITWISE_AND:
				return -3;
			case MATH_OPERATOR_BITWISE_OR:
				return -4;
			case MATH_OPERATOR_BITWISE_XOR:
				return -4;
		}
		// if the control reaches here
		error("Unknown operator type %d", op);
		abort();
	}

	void handle_operator(char op) {
		static stack<int> oper;
		int o = conv_to_operator(op);
		while ( !oper.empty() ) {
			if ( oper.top() == OPEN_BRACKET ) {
				oper.pop();
			} else if ( precedence(oper.top()) <= precedence(o) ) {
				int t = oper.top();
				oper.pop();
				operators.push_back(t);
				*(number_of_operators_after_operand.rbegin())++;
			} else {
				if ( o != CLOSE_BRACKET ) {
					oper.push(o);
				}
				break;
			}
		}
	}

	static int32_t run_op(int32_t a, u_int8_t o, int32_t b) {
		switch(o) {
			case MATH_OPERATOR_PLUS:
				return a + b;
			case MATH_OPERATOR_MINUS:
				return a - b;
			case MATH_OPERATOR_MULTIPLY:
				return a * b;
			case MATH_OPERATOR_DIVIDE:
				return a / b;
			case MATH_OPERATOR_MODULO:
				return a % b;
			case MATH_OPERATOR_BITWISE_AND:
				return a & b;
			case MATH_OPERATOR_BITWISE_OR:
				return a | b;
			case MATH_OPERATOR_BITWISE_XOR:
				return a ^ b;
		}
		// If it reaches here, unknown operator
		error("Unknown operator %d", o);
		abort();
	}

	int32_t get_answer() {
		stack<int32_t> s;
		int i = 0, j = 0;
		while ( true ) {
			s.push(operands[i]);
			for ( int k = 0 ; k < number_of_operators_after_operand[i] ; k++ ) {
				u_int8_t o = operators[j++];
				if ( s.empty() ) {
					error("Stack Underflow");
					abort();
				}
				int32_t b = s.top();
				s.pop();
				if ( s.empty() ) {
					error("Stack Underflow");
					abort();
				}
				int32_t a = s.top();
				s.pop();
				s.push(run_op(a,o,b));
			}
		}
		if ( ! s.size() == 1 ) {
			error("Improper stack size %d", s.size());
			abort();
		}
		return s.top();
	}
public:
	ReversePolishExpression(std::string math_expression) {
		const char * c = math_expression.c_str();
		while ( *c ) {
			int v;
			c = read_int(c,v);
			operands.push_back(v);
			number_of_operators_after_operand.push_back(0);
			while ( *c == ' ' || *c == '\t' ) {
				c++;
			}
			handle_operator(*c);
			c++;
		}
		handle_operator('\0');
		if ( operators.size() != operands.size() - 1 ) {
			error("Wrong number of operators/operands");
			abort();
		}
	}
	ReversePolishExpression(u_char* math_packet, u_int16_t number_of_operands) {
		int32_t* operands_from_math_packet = (int32_t*) math_packet;
		for ( int i = 0 ; i < number_of_operands ; i++ ) {
			operands.push_back(*operands_from_math_packet);
			operands_from_math_packet++;
		}
		u_int8_t* operators_from_math_packet = (u_int8_t*) operands_from_math_packet;
		for ( int i = 0 ; i < number_of_operands - 1 ; i++ ) {
			operators.push_back(*operators_from_math_packet);
			operators_from_math_packet++;
		}
		for ( int i = 0 ; i < number_of_operands - 1 ; i++ ) {
			number_of_operators_after_operand.push_back(*operators_from_math_packet);
			operators_from_math_packet++;
		}
		operands_from_math_packet = (int32_t*) operands_from_math_packet;
		operands_from_math_packet++;
		u_int16_t end_packet_magic_number = *(u_int16_t*)operands_from_math_packet;
		if ( end_packet_magic_number != 21845 ) {
			error("Mismatched end magic number. Found %d instead.",end_packet_magic_number);
			abort();
		}
	}
	Packet conv_to_packet() {
		Packet ret;
		int number_of_operands = get_number_of_operands();
		int32_t* operands_from_math_packet = (int32_t*) ret.first;
		for ( int i = 0 ; i < number_of_operands ; i++ ) {
			*operands_from_math_packet = operands[i];
			operands_from_math_packet++;
		}
		u_int8_t* operators_from_math_packet = (u_int8_t*) operands_from_math_packet;
		for ( int i = 0 ; i < number_of_operands - 1 ; i++ ) {
			*operators_from_math_packet = operators[i];
			operators_from_math_packet++;
		}
		for ( int i = 0 ; i < number_of_operands - 1 ; i++ ) {
			*operators_from_math_packet = number_of_operators_after_operand[i];
			operators_from_math_packet++;
		}
		int32_t* answer = (int32_t*) operands_from_math_packet;
		*answer = 0;
		answer++;
		u_int16_t* end_packet_magic_number = (u_int16_t*)answer;
		*end_packet_magic_number = 21845;
		return ret;
	}
	Packet conv_to_ans_packet() {
		Packet temp = conv_to_packet();
		int number_of_operands = get_number_of_operands();
		*(int32_t*)(temp.first+6*number_of_operands-2) = get_answer();
		return temp;
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
	MathPacketHeader *mph = (MathPacketHeader*) request_packet;
	ReversePolishExpression rpe(request_packet+sizeof(MathPacketHeader),mph->number_of_operands);
	Packet wrapped_packet = wrap_header(rpe.conv_to_ans_packet(),rpe.get_number_of_operands(),MATH_TYPE_SEND_ANSWER);
	return wrapped_packet;
}