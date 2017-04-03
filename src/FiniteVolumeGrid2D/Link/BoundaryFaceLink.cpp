#include "BoundaryFaceLink.h"
#include "Face.h"
#include "Cell.h"

BoundaryLink::BoundaryLink(const Cell &self, const Face &face)
    :
      Link(self),
      face_(face)
{
    init();
}

BoundaryLink::BoundaryLink(const BoundaryLink &other)
    :
      BoundaryLink(other.self_, other.face_)
{

}

void BoundaryLink::init()
{
    rFaceVec_ = face_.centroid() - self_.centroid();
    outwardNorm_ = face_.outwardNorm(self_.centroid());
}

const Face &BoundaryLink::face() const
{
    return face_;
}
