/**
* boltdrawmanager.h
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

#ifndef BOLTDRAWMANAGER_H
#define BOLTDRAWMANAGER_H

#include "gfx/decalqueue.h"
#include "bolt.h"
#include "vec.h"

#include <vector>

class Animation;

class BoltDrawManager
{
public:
    class DecalQueue boltdecals;
    static GFXVertexList * boltmesh;
    static QVector camera_position;
    static float pixel_angle;
    static float elapsed_time;

    vector <std::string> animationname;
    vector <Animation *> animations; // Balls are animated
    vector <vector <Bolt> > bolts; // The inner vector is all of the same type.
    vector <vector <Bolt> > balls;

    BoltDrawManager();
    ~BoltDrawManager();

    static BoltDrawManager& GetInstance();

    static void Draw();
};

#endif // BOLTDRAWMANAGER_H
