#include "gfx_box.h"
#include "xml_support.h"

using XMLSupport::tostring;

/*
inline ostream &operator<<(ostrstream os, const Vector &obj) {
  return os << "(" << obj.i << "," << obj.j << "," << obj.k << ")";
}
*/
string tostring(const Vector &v) {
  return string("(") + tostring(v.i) + ", " + tostring(v.j) + ", " + tostring(v.k) + ")";
}

Box::Box(const Vector &corner1, const Vector &corner2) : corner_min(corner1), corner_max(corner2) {
  InitUnit();
  draw_sequence = 3;

  Box *oldmesh;
  string hash_key = string("@@Box") + "#" + tostring(corner1) + "#" + tostring(corner2);
  //  cerr << "hashkey: " << hash_key << endl;
  if(0 != (oldmesh = (Box*)meshHashTable.Get(hash_key)))
    {
      *this = *oldmesh;
      oldmesh->refcount++;
      orig = oldmesh;
      return;
    }
  int a=0;
  GFXVertex *vertices = new GFXVertex[8];
#define VERTEX(i,j,k) { vertices[a].x = i; vertices[a].y = j; vertices[a].z = k;a++;}

  VERTEX(corner_max.i,corner_min.j,corner_max.k);
  VERTEX(corner_min.i,corner_min.j,corner_max.k);
  VERTEX(corner_min.i,corner_min.j,corner_min.k);
  VERTEX(corner_max.i,corner_min.j,corner_min.k);

  VERTEX(corner_max.i,corner_max.j,corner_min.k);
  VERTEX(corner_min.i,corner_max.j,corner_min.k);
  VERTEX(corner_min.i,corner_max.j,corner_max.k);
  VERTEX(corner_max.i,corner_max.j,corner_max.k);

  vlist = new GFXVertexList(8,0,1,vertices);
  delete [] vertices;

  a = 0;
  vertices = new GFXVertex[10];

  VERTEX(corner_max.i,corner_min.j,corner_max.k);
  VERTEX(corner_min.i,corner_min.j,corner_max.k);
  VERTEX(corner_min.i,corner_max.j,corner_max.k);
  VERTEX(corner_max.i,corner_max.j,corner_max.k);

  VERTEX(corner_max.i,corner_max.j,corner_min.k);
  VERTEX(corner_min.i,corner_max.j,corner_min.k);

  VERTEX(corner_min.i,corner_min.j,corner_min.k);
  VERTEX(corner_max.i,corner_min.j,corner_min.k);


  VERTEX(corner_max.i,corner_min.j,corner_max.k);
  VERTEX(corner_min.i,corner_min.j,corner_max.k);


  numQuadstrips = 1;
  quadstrips = new GFXQuadstrip*[1];
  quadstrips[0] = new GFXQuadstrip(10,vertices);
  delete [] vertices;

  meshHashTable.Put(hash_key, this);
  orig = this;
  refcount++;
  draw_queue = new vector<DrawContext>;
#undef VERTEX
}

void Box::ProcessDrawQueue() {
  if(!draw_queue->size()) return;
  GFXBlendMode(SRCALPHA,INVSRCALPHA);
  //GFXColor(0.0,.90,.3,0.9);
  GFXDisable(LIGHTING);
  GFXDisable(TEXTURE0);
  GFXDisable(TEXTURE1);
  GFXDisable(CULLFACE);
  //GFXBlendMode(ONE, ONE);

  while(draw_queue->size()) {
    DrawContext c = draw_queue->back();
    draw_queue->pop_back();
    Matrix m;
    c.transformation.to_matrix(m);
    GFXLoadMatrix(MODEL, m);


  GFXBegin(QUADS);
  GFXColor4f(0.0,1.0,0.0,0.2);

  GFXVertex3f(corner_max.i,corner_min.j,corner_max.k);
  GFXVertex3f(corner_max.i,corner_max.j,corner_max.k);
  GFXVertex3f(corner_min.i,corner_max.j,corner_max.k);
  GFXVertex3f(corner_min.i,corner_min.j,corner_max.k);

  GFXColor4f(0.0,1.0,0.0,0.2);
  GFXVertex3f(corner_min.i,corner_min.j,corner_min.k);
  GFXVertex3f(corner_min.i,corner_max.j,corner_min.k);
  GFXVertex3f(corner_max.i,corner_max.j,corner_min.k);
  GFXVertex3f(corner_max.i,corner_min.j,corner_min.k);

  GFXColor4f(0.0,.70,0.0,0.2);

  GFXVertex3f(corner_max.i,corner_min.j,corner_max.k);
  GFXVertex3f(corner_min.i,corner_min.j,corner_max.k);
  GFXVertex3f(corner_min.i,corner_min.j,corner_min.k);
  GFXVertex3f(corner_max.i,corner_min.j,corner_min.k);

  GFXColor4f(0.0,.70,0.0,0.2);
  GFXVertex3f(corner_max.i,corner_max.j,corner_min.k);
  GFXVertex3f(corner_min.i,corner_max.j,corner_min.k);
  GFXVertex3f(corner_min.i,corner_max.j,corner_max.k);
  GFXVertex3f(corner_max.i,corner_max.j,corner_max.k);

  GFXColor4f(0.0,.90,.3,0.2);
  GFXVertex3f(corner_max.i,corner_max.j,corner_max.k);
  GFXVertex3f(corner_max.i,corner_min.j,corner_max.k);
  GFXVertex3f(corner_max.i,corner_min.j,corner_min.k);
  GFXVertex3f(corner_max.i,corner_max.j,corner_min.k);

  GFXColor4f(0.0,.90,.3,0.2);
  GFXVertex3f(corner_min.i,corner_max.j,corner_min.k);
  GFXVertex3f(corner_min.i,corner_min.j,corner_min.k);
  GFXVertex3f(corner_min.i,corner_min.j,corner_max.k);
  GFXVertex3f(corner_min.i,corner_max.j,corner_max.k);
  GFXEnd();


  /*
    vlist->Draw();
    if(quadstrips!=NULL) {
      for(int a=0; a<numQuadstrips; a++)
	quadstrips[a]->Draw()
	  ;
	  }
  */

  }
}
