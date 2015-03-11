#ifndef MAGIC3D_BOX_H
#define MAGIC3D_BOX_H

#include <Mesh/TriangleMesh.h>
#include <CollisionShapes\CollisionShape.h>
#include <Mesh\TriangleMeshBuilder.h>
#include <Geometry\Geometry.h>

namespace Magic3D
{


class Box : public Geometry
{
    Vector3 dimensions;
    Matrix4 transform;
    bool transformApplied;

    mutable std::shared_ptr<CollisionShape> collisionShape;
    mutable std::shared_ptr<TriangleMesh> triangleMesh;

    inline void markDirty()
    {
        this->collisionShape = nullptr;
        this->triangleMesh = nullptr;
    }

public:
    inline Box(Scalar width, Scalar height, Scalar depth) :
        dimensions(width, height, depth), transformApplied(false){}

    virtual void positionTransform(const Geometry::Transform& transform)
    {
        Matrix4 tmp;
        transform.getCombinedMatrix(tmp);

        this->transform.multiply(tmp);
        this->transformApplied = true;
        this->markDirty();
    }

    virtual const CollisionShape& getCollisionShape() const;

    virtual const TriangleMesh& getTriangleMesh() const;

    virtual const Sphere& getBoundingSphere() const
    {
        // TODO: stop using triangle mesh for bounding sphere
        return this->getTriangleMesh().getBoundingSphere();
    }

};

};


#endif