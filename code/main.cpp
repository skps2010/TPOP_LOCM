#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "tai-su.h"

int main()
{
    while (1)
    {
        GameManager g;
        g.setUp();

        // Draw Phase
        if (g.playerMana == 0)
        {
            g.draw();
        }
        // Battle Phase
        else
        {
            g.battle();
        }
        // Print actions
        g.print();
    }
    return 0;
}
