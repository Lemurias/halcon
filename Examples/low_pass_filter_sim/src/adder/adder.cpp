#include "adder.hpp"

Adder::Adder()
{
    /* Ports */
    REFLECT(i_low);
    REFLECT(i_high);
    REFLECT(o_sum);
}

void Adder::Init()
{

}

void Adder::Connect()
{
    o_sum << sum << COMBINATIONAL_PORT;
}

void Adder::RunClockMaster()
{
    sum = i_low.GetData() + i_high.GetData();
}
