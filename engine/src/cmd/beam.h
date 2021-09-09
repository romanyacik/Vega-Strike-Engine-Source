/**
* beam.h
*
* Copyright (c) 2001-2002 Daniel Horn
* Copyright (c) 2002-2019 pyramid3d and other Vega Strike Contributors
* Copyright (c) 2019-2021 Stephen G. Tuggy, and other Vega Strike Contributors
*
* https://github.com/vegastrike/Vega-Strike-Engine-Source
*
* This file is part of Vega Strike.
*
* Vega Strike is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* Vega Strike is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Vega Strike. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _CMD_BEAM_H_
#define _CMD_BEAM_H_
#include "gfx/mesh.h"
#include "unit_collide.h"
#include "gfx/matrix.h"
#include "gfx/quaternion.h"
#include <vector>
class GFXVertexList;
class Texture;
struct GFXColor;
using std::vector;

class Beam
{
private:
    int sound;
    Transformation local_transformation;
    unsigned int   decal;
    GFXVertexList *vlist;
    LineCollide    CollideInfo;
    
    unsigned int   numframes;
    float    speed; //lite speed
    float    texturespeed;
    float    curlength;
    float    range;
    float    radialspeed;
    float    curthick;
    float    thickness;
    float    lastthick;
    float    lastlength;
    float    stability;
    float    damagerate;
    float    phasedamage;
    float    rangepenalty;
    float    refire;
    float    refiretime;
    float    texturestretch;
    GFXColor Col;
    enum Impact
    {
        ALIVE   =0,
        IMPACT  =1,
        UNSTABLE=2,
        IMPACTANDUNSTABLE=3
    }; //is it right now blowing the enemy to smitheri
    unsigned char impact;
    bool    listen_to_owner;
    void   *owner; //may be a dead pointer...never dereferenced
    QVector center; //in world coordinates as of last physics frame...
    Vector  direction;

    void RecalculateVertices( const Matrix &trans );
    void CollideHuge( const LineCollide&, Unit *targetToCollideWith, Unit *firer, Unit *superunit );
public:
    void ListenToOwner( bool listen )
    {
        listen_to_owner = listen;
    }
    Beam( const Transformation &trans, const weapon_info &clne, void *own, Unit *firer, int sound );
    void Init( const Transformation &trans, const weapon_info &clne, void *own, Unit *firer );
    ~Beam();
    void RemoveFromSystem( bool eradicate );
    float refireTime();
    QVector GetPosition() const
    {
        return local_transformation.position;
    }
    void SetPosition( const QVector& );
    void SetOrientation( const Vector &p, const Vector &q, const Vector &r );
    void UpdatePhysics( const Transformation &,
                        const Matrix &,
                        class Unit*target,
                        float trackingcone,
                        Unit*targetToCollideWith /*prevent AI friendly fire--speed up app*/,
                        float HeatSink,
                        Unit*firer,
                        Unit*superunit );
    void Draw( const Transformation &, const Matrix &, class Unit*target, float trackingcone );
    void Destabilize()
    {
        impact = UNSTABLE;
    }
    bool Dissolved()
    {
        return curthick == 0;
    }
    bool Ready()
    {
        return curthick == 0 && refiretime > refire;
    }
    bool Collide( class Unit*target, Unit*firer, Unit*superunit /*for cargo*/ );
    static void ProcessDrawQueue();
};
#endif

