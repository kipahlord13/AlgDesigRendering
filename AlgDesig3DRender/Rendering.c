#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float x;
  float y;
  float z;
} Point;

typedef struct {
  Point* p1;
  Point* p2;
  Point* p3;
  Point* normalVector;
} Plane;

Point cross(Point* p1, Point* p2, Point* p3) {
  Point v1, v2;
  v1.x = p2->x - p1->x;
  v1.y = p2->y - p1->y;
  v1.z = p2->z - p1->z;
  v2.x = p3->x - p1->x;
  v2.y = p3->y - p1->y;
  v2.z = p3->z - p1->z;
  Point result;
  result.x = (v1.y * v2.z) - (v1.z * v2.y);
  result.y = (v1.z * v2.x) - (v1.x * v2.z);
  result.z = (v1.x * v2.y) - (v1.y * v2.x);
  return result;
}

//upwards of the plane is when 1,2,3 are in clockwise order
void calcNormal(Plane* p) {
  Point newP = cross(p->p1, p->p3, p->p2);
  p->normalVector = &newP;
}


void main() {
  Point * p = malloc(sizeof(Point));
  p->x = 0.5;
  p->y = 2;
  Point * p2 = malloc(sizeof(Point));
  p2->x = -1;
  p2->y = 2;
  Point * p3 = malloc(sizeof(Point));
  p3->x = 0;
  p3->y = 0;
  Plane plane;
  plane.p1 = p3;
  plane.p2 = p;
  plane.p3 = p2;
  calcNormal(&plane);
  printf("normalVector: <%f, %f, %f>\n", plane.normalVector->x, plane.normalVector->y, plane.normalVector->z);
  return;
}
