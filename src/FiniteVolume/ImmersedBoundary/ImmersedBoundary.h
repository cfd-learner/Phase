#ifndef IMMERSED_BOUNDARY_H
#define IMMERSED_BOUNDARY_H

#include "ImmersedBoundaryObject.h"
#include "CutCell.h"

class Solver;

class ImmersedBoundary
{
public:

    enum {FLUID_CELLS = 1, IB_CELLS = 2, SOLID_CELLS = 3, FRESH_CELLS = 4, DEAD_CELLS = 5, BUFFER_CELLS = 6};

    ImmersedBoundary(const Input &input, Solver &solver);

    void initCellZones(CellZone &zone);

    const CellZone& zone() const
    { return *zone_; }

    std::shared_ptr<const ImmersedBoundaryObject> ibObj(const Point2D& pt) const;

    void update(Scalar timeStep);

    template<class T>
    void copyBoundaryTypes(const FiniteVolumeField<T>& srcField, const FiniteVolumeField<T>& destField)
    {
        for(auto& ibObj: ibObjs_)
            ibObj->addBoundaryType(destField.name(), ibObj->boundaryType(srcField.name()));
    }

    template<class T>
    Equation<T> bcs(FiniteVolumeField<T>& field) const
    {
        Equation<T> eqn(field);

        for(const auto& ibObj: ibObjs_)
            eqn += ibObj->bcs(field);

        return eqn;
    }

    void clearFreshCells();

    std::vector<CutCell> constructCutCells(const CellGroup& cellGroup) const;

    std::vector<Ref<const ImmersedBoundaryObject>> ibObjs() const;

    std::vector<std::shared_ptr<ImmersedBoundaryObject>>::const_iterator begin() const
    { return ibObjs_.begin(); }

    std::vector<std::shared_ptr<ImmersedBoundaryObject>>::const_iterator end() const
    { return ibObjs_.end(); }

    bool isIbCell(const Cell &cell) const;

    void computeForce(const ScalarFiniteVolumeField& rho, const ScalarFiniteVolumeField& mu, const VectorFiniteVolumeField& u, const ScalarFiniteVolumeField& p);

protected:

    void setCellStatus();

    const CellZone *zone_ = nullptr;

    Solver &solver_;
    FiniteVolumeField<int> &cellStatus_;
    std::vector<std::shared_ptr<ImmersedBoundaryObject>> ibObjs_;

};

#endif
