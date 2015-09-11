#ifndef ON_THE_FLY_VARIABLES
#define ON_THE_FLY_VARIABLES

struct OnTheFlyVariables
{
    float someCoolVariable;
};

OnTheFlyVariables onTheFlyVariables;

void ComputeOnTheFlyVariables()
{
    onTheFlyVariables.someCoolVariable = 3.14;
}

#endif
