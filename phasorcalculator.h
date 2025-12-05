#ifndef PHASORCALCULATOR_H
#define PHASORCALCULATOR_H

#include "dataManagement.h"
class phasorCalculator
{
public:
    phasorCalculator(dataManagement& dataMng);

    phasor_data calcPhasor() const;

private:
    dataManagement& dataMng;
};

#endif // PHASORCALCULATOR_H
