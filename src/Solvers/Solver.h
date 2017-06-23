#ifndef SOLVER_H
#define SOLVER_H

#include "FiniteVolumeGrid2D.h"
#include "Input.h"
#include "ScalarFiniteVolumeField.h"
#include "VectorFiniteVolumeField.h"
#include "SparseMatrixSolver.h"
#include "Circle.h"
#include "ImmersedBoundary.h"
#include "VolumeIntegrator.h"
#include "ForceIntegrator.h"

class Solver
{
public:
    //- Constructors
    Solver(const Input &input,
           std::shared_ptr<FiniteVolumeGrid2D>& grid);

    //- Info
    virtual std::string info() const;

    //- Print
    void printf(const char* format, ...) const;

    //- Solve
    virtual Scalar solve(Scalar timeStep) = 0;

    //- Time step methods
    virtual Scalar computeMaxTimeStep(Scalar maxCo, Scalar prevTimeStep) const = 0;

    Scalar maxTimeStep() const
    { return maxTimeStep_; }

    //- Field management
    void registerField(std::shared_ptr<ScalarFiniteVolumeField> field)
    { scalarFields_[field->name()] = field; }

    void registerField(std::shared_ptr<VectorFiniteVolumeField> field)
    { vectorFields_[field->name()] = field; }

    FiniteVolumeField<int> &addIntegerField(const std::string &name);

    ScalarFiniteVolumeField &addScalarField(const Input &input, const std::string &name);

    ScalarFiniteVolumeField &addScalarField(const std::string &name);

    VectorFiniteVolumeField &addVectorField(const Input &input, const std::string &name);

    VectorFiniteVolumeField &addVectorField(const std::string &name);

    //- Field data structures
    const std::map<std::string, std::shared_ptr<FiniteVolumeField<int>> >& integerFields() const
    { return integerFields_; }

    const std::map<std::string, std::shared_ptr<ScalarFiniteVolumeField>>& scalarFields() const
    { return scalarFields_; }

    const std::map<std::string, std::shared_ptr<VectorFiniteVolumeField>>& vectorFields() const
    { return vectorFields_; }

    //- Field lookup
    FiniteVolumeField<int>& integerField(const std::string& name)
    { return *integerFields_.find(name)->second; }

    const FiniteVolumeField<int>& integerField(const std::string& name) const
    { return *integerFields_.find(name)->second; }

    ScalarFiniteVolumeField& scalarField(const std::string& name)
    { return *scalarFields_.find(name)->second; }

    const ScalarFiniteVolumeField& scalarField(const std::string& name) const
    { return *scalarFields_.find(name)->second; }

    VectorFiniteVolumeField& vectorField(const std::string& name)
    { return *vectorFields_.find(name)->second; }

    const VectorFiniteVolumeField& vectorField(const std::string& name) const
    { return *vectorFields_.find(name)->second; }

    //- Grid
    FiniteVolumeGrid2D &grid()
    { return *grid_; }

    const FiniteVolumeGrid2D &grid() const
    { return *grid_; }

    //- ICs/IBs
    void setInitialConditions(const Input &input);

    const ImmersedBoundary& ib() const { return ib_; }

    std::vector<Ref<const ImmersedBoundaryObject>> ibObjs() const
    { return ib_.ibObjs(); }

    virtual void initialize() {}

    //- Integrators
    const std::vector<VolumeIntegrator> &volumeIntegrators() const
    { return volumeIntegrators_; }

    std::vector<VolumeIntegrator> &volumeIntegrators()
    { return volumeIntegrators_; }

    const std::vector<ForceIntegrator> &forceIntegrators() const
    { return forceIntegrators_; }

    std::vector<ForceIntegrator> &forceIntegrators()
    { return forceIntegrators_; }

protected:

    void setCircle(const Circle &circle, Scalar innerValue, ScalarFiniteVolumeField &field);

    void setCircle(const Circle &circle, const Vector2D &innerValue, VectorFiniteVolumeField &field);

    void setCircleSector(const Circle &circle, Scalar thetaMin, Scalar thetaMax, Scalar innerValue,
                         ScalarFiniteVolumeField &field);

    void setBox(const Polygon &box, Scalar innerValue, ScalarFiniteVolumeField &field);

    void setBox(const Polygon &box, const Vector2D &innerValue, VectorFiniteVolumeField &field);

    void setRotating(const std::string &function, Scalar amplitude, const Vector2D &center, ScalarFiniteVolumeField &field);

    void setRotating(const std::string &xFunction, const std::string &yFunction, const Vector2D &amplitude,
                     const Vector2D &center, VectorFiniteVolumeField &field);

    std::shared_ptr<FiniteVolumeGrid2D> grid_;

    //- Fields and geometries
    mutable std::map<std::string, std::shared_ptr<FiniteVolumeField<int>> > integerFields_;
    mutable std::map<std::string, std::shared_ptr<ScalarFiniteVolumeField>> scalarFields_;
    mutable std::map<std::string, std::shared_ptr<VectorFiniteVolumeField>> vectorFields_;

    Scalar timeStepRelaxation_, maxTimeStep_;

    ImmersedBoundary ib_;

    std::vector<VolumeIntegrator> volumeIntegrators_;
    std::vector<ForceIntegrator> forceIntegrators_;
};

#endif
