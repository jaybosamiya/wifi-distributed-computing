#include <iostream>
#include "math_packet.h"

using namespace std;

int main() {

    cout << "Enter a math expression: ";
    string expression;
    getline(cin,expression);
    Packet p = make_packet_from_expression(expression);

    cerr << "Done understanding\n";

    p = make_answer_packet(p.first);

    cerr << "Done calculating\n";

    cout << "Answer : " << read_answer(p) << endl;

}
