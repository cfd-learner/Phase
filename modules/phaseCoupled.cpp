#include <iostream>

#include "Input.h"
#include "CommandLine.h"
#include "ConstructGrid.h"
#include "Coupled.h"
#include "RunControl.h"

int main(int argc, const char *argv[])
{
    using namespace std;

    Input input;
    CommandLine(argc, argv);

    input.parseInputFile();

    shared_ptr<FiniteVolumeGrid2D> gridPtr(constructGrid(input));
    Coupled solver(input, *gridPtr);

    solver.setSparseMatrixSolver(new EigenSparseMatrixSolver());

    Viewer viewer(solver, input);
    RunControl runControl;

    runControl.run(input, solver, viewer);
}

