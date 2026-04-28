#include <iostream>
#include "includes/Melzak.hpp"
#include "includes/Boruvka.hpp"
#include "includes/Utils.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    vector<Terminal> terminals = GetTerminals("_in/terminals.txt");
    auto MBTLength = BoruvkaAlgorithm(terminals);
    cout << "MBTLength: " << MBTLength << "\n";

    auto MST = MelzakAlgorithm(terminals);
    cout << "MSTength: " << MST.length << "\n";
    cout << "Length was improved by: " << int(100 - 100 * MST.length / MBTLength) << "%\n";
    ToGraphviz(terminals, MST);

    return 0;
}
//g++ main.cpp -o START
