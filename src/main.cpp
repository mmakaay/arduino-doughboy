#include "main.h"

Dough::Logger logger("MAIN");

void setup()
{
    Dough::App::Instance()->setup();
}

void loop()
{
    Dough::App::Instance()->loop();
}