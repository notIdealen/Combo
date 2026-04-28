#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <cmath>
#include "includes/Utils.hpp"
#include "includes/Melzak.hpp"
#include "includes/Boruvka.hpp"
#include "includes/Const.hpp"
#include "includes/Structs.hpp"


using namespace std;

TEST_CASE("Melzak")
{
    SECTION("rectangle")
    {
        const int k = 10000;
        auto terminals = GetTerminals("_in/rect.txt");
        double bestLength = 9.4641;
        auto MBT = BoruvkaAlgorithm(terminals);
        MBT = round(MBT * k) / k;
        auto MST = MelzakAlgorithm(terminals);
        MST.length = round(MST.length * k) / k;
        REQUIRE(MST.length == bestLength);
        cout << "--- rectangle ---\n";
        ToGraphviz(terminals, MST);
    }

    SECTION("Zigzag")
    {
        const int k = 1000;
        auto terminals = GetTerminals("_in/zigzag.txt");
        double bestLength = 7.999;
        auto MBT = BoruvkaAlgorithm(terminals);
        auto MST = MelzakAlgorithm(terminals);
        MST.length = round(MST.length * k) / k;
        MBT = round(MBT * k) / k;
        REQUIRE((MST.length == bestLength && MBT == bestLength));
        cout << "--- zigzag ---\n";
        ToGraphviz(terminals, MST);
    }

    SECTION("line")
    {
        const int k = 10;
        auto terminals = GetTerminals("_in/line.txt");
        double bestLength = 4;
        auto MBT = BoruvkaAlgorithm(terminals);
        auto MST = MelzakAlgorithm(terminals);
        MST.length = round(MST.length * k) / k;
        MBT = round(MBT * k) / k;
        REQUIRE((MST.length == bestLength && MBT == bestLength));
        cout << "--- line ---\n";
        ToGraphviz(terminals, MST);
    }

    SECTION("7 points")
    {
        const int k = 10000;
        auto terminals = GetTerminals("_in/7.txt");
        double bestLength = 55.6877;
        auto MBT = BoruvkaAlgorithm(terminals);
        auto MST = MelzakAlgorithm(terminals);
        MST.length = round(MST.length * k) / k;
        MBT = round(MBT * k) / k;
        REQUIRE((MST.length == bestLength && MBT == 57.9167));
        cout << "--- 7 points ---\n";
        ToGraphviz(terminals, MST);
    }

    SECTION("circle 5")
    {
        const int k = 10000;
        auto terminals = GetTerminals("_in/star.txt");
        double bestLength = 13.722;
        auto MBT = BoruvkaAlgorithm(terminals);
        auto MST = MelzakAlgorithm(terminals);
        MST.length = round(MST.length * k) / k;
        MBT = round(MBT * k) / k;
        REQUIRE((MST.length == bestLength && MBT == 14.1059));
        cout << "--- circle 5 ---\n";
        ToGraphviz(terminals, MST);
    }

    SECTION("circle 6")
    {
        const int k = 10000;
        auto terminals = GetTerminals("_in/6star.txt");
        double bestLength = 14.9997;
        auto MBT = BoruvkaAlgorithm(terminals);
        MBT = round(MBT * k) / k;
        auto MST = MelzakAlgorithm(terminals);
        MST.length = round(MST.length * k) / k;
        cout << "!!!!" << MST.length << ' ' << MBT << endl;
        REQUIRE((MST.length == bestLength && MBT == bestLength));
        cout << "--- circle 6 ---\n";
        ToGraphviz(terminals, MST);
    }
}
