#ifndef _COLLIDE_MAP_H_
#define _COLLIDE_MAP_H_
#include "key_mutable_set.h"
#include "vegastrike.h"
#include "gfx/vec.h"
#include <limits>
class Unit;
class Bolt;
class Collidable{
  float key;
  QVector position;
public:
  float radius;//radius == 0, bolt  radius <0 beam, radius >0 unit, radius == nan  null
  
  union CollideRef{
    Unit * unit;
    unsigned int bolt_index;
  }ref;
  QVector GetPosition() const{
    return position;
  }
  void SetPosition(const QVector &bpos) {
    //in case we want to drop in an xtra radius parameter when we get performance testing    
    this->position=bpos;
    key=bpos.MagnitudeSquared();
    if (ISNAN(key)) {
      key=0;//hack for now
    }
  }
  Collidable &operator* () {return *this;}
  Collidable *operator-> () {return this;}

  float GetMagnitudeSquared()const {return key;}
  bool operator <(const Collidable &other) const {
    return key<other.key;
  }

  Collidable() : radius(std::numeric_limits<float>::quiet_NaN()) {}
  Collidable(Unit * un);
  Collidable(unsigned int bolt_index, float speed, const QVector &p){
    ref.bolt_index=bolt_index;
    radius=-speed*SIMULATION_ATOM;
    this->SetPosition(p);
  }
};
/* Arbitrarily use Set for ALL PLATFORMS -hellcatv */
#define VS_ENABLE_COLLIDE_KEY

#ifdef VS_ENABLE_COLLIDE_KEY
class CollideMap:public KeyMutableSet<Collidable> {
#else
class CollideMap:public ListMutableSet<Collidable> {
#endif
public:
//Check collisions takes an item to check collisions with, and returns whether that item collided with a Unit only
  bool CheckCollisions(Bolt * bol, const Collidable & updated);
  bool CheckCollisions(Unit * un, const Collidable & updated);//will be handed off to a templated function
};
extern CollideMap null_collide_map;
extern CollideMap::iterator null_collide_iter;
extern bool null_collide_iter_initialized;

inline void init_null_collide_iter()
{
	if (!null_collide_iter_initialized) {
		null_collide_map.insert(Collidable());
		null_collide_iter = null_collide_map.begin();
		null_collide_iter_initialized = true;
	}
}

inline bool is_null(const CollideMap::iterator &it)
{
	return ISNAN(it->get().radius);
}

inline void set_null(CollideMap::iterator &it)
{
	init_null_collide_iter();
	it = null_collide_iter;
}

#endif
