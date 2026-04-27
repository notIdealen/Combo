#include <iostream>
#include "includes/Melzak.hpp"
#include "includes/Utils.hpp"


int main(int argc, char const *argv[])
{
    vector<Terminal> terminals = GetTerminals("_in/terminals.txt");
    auto graph = MelzakAlgorithm(terminals);
    std::cout << "BESTLength: " << graph.length << "\n";
    ToGraphviz(terminals, graph);

    return 0;
}
//g++ main.cpp -o START
