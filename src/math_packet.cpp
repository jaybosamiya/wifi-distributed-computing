/* math_packet.cpp */

// Define ways build and use a math packet

#include "math_packet.h"
#include "util.h"
#include "pcap_manager.h"

#include <vector>
#include <stack>
#include <cstring>

using namespace std;

extern u_int32_t own_id;

class ReversePolishExpression {
	vector<int32_t> operands;
	vector<u_int8_t> operators;
	vector<u_int8_t> number_of_operators_after_operand;

	static const char * read_int ( const char * str, int & v ) {
		bool is_negative = false;
		v = 0;
		int i = 0;
		while ( ! (str[i] >= '0' && str[i] <= '9' || str[i] == '-') && str[i] ) {
			i++;
		}
		if ( str[i] == '-' ) {
			is_negative = true;
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
		if ( is_negative ) {
			v = -v;
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
		if ( oper.empty() && o != END_OF_EXPRESSION ) {
			oper.push(o);
			return;
		}
		while ( !oper.empty() ) {
			if ( o == END_OF_EXPRESSION ) {
				int t = oper.top();
				oper.pop();
				operators.push_back(t);
				(*number_of_operators_after_operand.rbegin())++;
				continue;
			}
			if ( precedence(oper.top()) >= precedence(o) ) {
				if ( oper.top() != OPEN_BRACKET ) {
					int t = oper.top();
					oper.pop();
					operators.push_back(t);
					(*number_of_operators_after_operand.rbegin())++;
				} else {
					break;
				}
			} else {
				break;
			}
		}
		if ( o == CLOSE_BRACKET ) {
			if ( !oper.empty() && oper.top() == OPEN_BRACKET ) {
				oper.pop();
				return;
			} else {
				error("Bracketing messed up");
				abort();
			}
		}
		oper.push(o);
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
		int k = 0;
		for ( int i = 0 ; i < operands.size() ; i++ ) {
			s.push(operands[i]);
			for ( int j = 0 ; j < number_of_operators_after_operand[i] ; j++ ) {
				u_int8_t o = operators[k++];
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
				int32_t ans = run_op(a,o,b);
				s.push(ans);
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
		if ( operators.size() != operands.size() - 1 ) {
			error("Wrong number of operators/operands (%d/%d)",operators.size(),operands.size());
			abort();
		}
	}
	ReversePolishExpression(u_char* math_packet, u_int16_t number_of_operands) {
		operands.resize(number_of_operands);
		operators.resize(number_of_operands-1);
		number_of_operators_after_operand.resize(number_of_operands);

		memcpy(&operands[0],math_packet,4*number_of_operands);
		memcpy(&operators[0],math_packet+4*number_of_operands,number_of_operands-1);
		memcpy(&number_of_operators_after_operand[0],math_packet+5*number_of_operands-1,number_of_operands);

		u_int16_t end_packet_magic_number;
		memcpy(&end_packet_magic_number,math_packet+6*number_of_operands+3,2);

		if ( end_packet_magic_number != 21845 ) {
			error("Mismatched end magic number. Found %d instead.",end_packet_magic_number);
			abort();
		}

		debug("Finished reading packet");
	}
	Packet conv_to_packet() {
		Packet ret;
		int number_of_operands = get_number_of_operands();
		int answer = 0;
		int end_packet_magic_number = 21845;
		u_char* location = ret.first = new u_char[6*number_of_operands+5];
		ret.second = 6*number_of_operands+5;
		memcpy(location,&operands[0],number_of_operands*4);
		location += number_of_operands*4;
		memcpy(location,&operators[0],number_of_operands-1);
		location += number_of_operands-1;
		memcpy(location,&number_of_operators_after_operand[0],number_of_operands);
		location += number_of_operands;
		memcpy(location,&answer,4);
		location += 4;
		memcpy(location,&end_packet_magic_number,2);
		location += 2;
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
	mph->user_id_of_sender = own_id;
}

Packet wrap_header(Packet math_packet, u_int16_t number_of_operands, u_int8_t type_of_packet) {
	MathPacketHeader mph;
	mph.magic_number = MATH_MAGIC;
	mph.type_of_packet = type_of_packet;
	mph.user_id_of_requester = own_id;
	mph.user_id_of_sender = 0;
	mph.request_id = generate_random(1,4294967295);
	mph.number_of_operands = number_of_operands;

	Packet ret;
	int header_size = 19;
	ret.second = math_packet.second + header_size;
	ret.first = new u_char[ret.second];

	memcpy(ret.first,&mph,header_size);
	memcpy(ret.first+header_size,math_packet.first,math_packet.second);

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

int read_answer(Packet &packet) {
	MathPacketHeader *mph = (MathPacketHeader*) packet.first;
	return *(int32_t*)(packet.first+sizeof(MathPacketHeader)+6*mph->number_of_operands-2);
}

Packet capture_math_packet() {
	Packet p;
	while ( ! is_capture_math_packet(p) ) {}
	return p;
}

Packet capture_math_packet(u_int8_t type) {
	Packet p;
	while ( ! is_capture_math_packet(p,type) ) {}
	return p;
}

Packet capture_math_packet(u_int8_t type, u_int32_t requester) {
	Packet p;
	while ( ! is_capture_math_packet(p,type,requester) ) {}
	return p;
}

Packet capture_math_packet(u_int8_t type, u_int32_t requester, u_int32_t req_id) {
	Packet p;
	while ( ! is_capture_math_packet(p,type,requester,req_id) ) {}
	return p;
}

Packet capture_math_packet(u_int8_t type, u_int32_t requester, u_int32_t req_id, u_int32_t sender) {
	Packet p;
	while ( ! is_capture_math_packet(p,type,requester,req_id,sender) ) {}
	return p;
}

bool is_capture_math_packet(Packet &p) {
	pcap_pkthdr hdr;
	u_char* packet = const_cast<u_char*> (pcap_next(handle,&hdr));
	int length = hdr.len;

	p.first = packet;
	p.second = length;

	p = unwrap_datalink(p);

	MathPacketHeader *mph = (MathPacketHeader*)p.first;

	if ( ! mph ) {
		return false;
	}
	if ( mph->magic_number != MATH_MAGIC ) {
		return false;
	}
	return true;
}

bool is_capture_math_packet(Packet &p, u_int8_t type) {
	bool ret = is_capture_math_packet(p);
	bool condition;
	if ( ret ) {
		MathPacketHeader *mph = extract_math_packet_header(p);
		condition = (mph->type_of_packet == type);
	}
	return ret && condition;
}

bool is_capture_math_packet(Packet &p, u_int8_t type, u_int32_t requester) {
	bool ret = is_capture_math_packet(p,type);
	bool condition;
	if ( ret ) {
		MathPacketHeader *mph = extract_math_packet_header(p);
		condition = (mph->user_id_of_requester == requester);
	}
	return ret && condition;
}

bool is_capture_math_packet(Packet &p, u_int8_t type, u_int32_t requester, u_int32_t req_id) {
	bool ret = is_capture_math_packet(p,type,requester);
	bool condition;
	if ( ret ) {
		MathPacketHeader *mph = extract_math_packet_header(p);
		condition = (mph->request_id == req_id);
	}
	return ret && condition;
}

bool is_capture_math_packet(Packet &p, u_int8_t type, u_int32_t requester, u_int32_t req_id, u_int32_t sender) {
	bool ret = is_capture_math_packet(p,type,req_id);
	bool condition;
	if ( ret ) {
		MathPacketHeader *mph = extract_math_packet_header(p);
		condition = (mph->user_id_of_sender == sender);
	}
	return ret && condition;
}

MathPacketHeader* extract_math_packet_header(Packet p) {
	return (MathPacketHeader*)p.first;
}
