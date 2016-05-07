#ifndef POISSON_H
#define POISSON_H

#include "Solver.h"
#include "Equation.h"

class Poisson : public Solver
{
public:

    Poisson(const FiniteVolumeGrid2D& grid, const Input& input);
    virtual Scalar solve(Scalar timeStep, Scalar prevTimeStep);
    virtual Scalar computeMaxTimeStep(Scalar maxCo) const { return INFINITY; }

    ScalarFiniteVolumeField& phi, &gamma;

protected:

    Equation<ScalarFiniteVolumeField> phiEqn_;
};

#endif
