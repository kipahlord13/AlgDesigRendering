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
  Point vector;
} Line;

typedef struct {
  Point* p1;
  Point* p2;
  Point* p3;
  float d;
  Point normalVector;
} Plane;

void cross(Point* p1, Point* p2, Point* p3, Point* vectorOut) {
  Point v1, v2;
  v1.x = p2->x - p1->x;
  v1.y = p2->y - p1->y;
  v1.z = p2->z - p1->z;
  v2.x = p3->x - p1->x;
  v2.y = p3->y - p1->y;
  v2.z = p3->z - p1->z;
  vectorOut->x = (v1.y * v2.z) - (v1.z * v2.y);
  vectorOut->y = (v1.z * v2.x) - (v1.x * v2.z);
  vectorOut->z = (v1.x * v2.y) - (v1.y * v2.x);
  return;
}

//upwards of the plane is when 1,2,3 are in clockwise order
void calcNormal(Plane* p) {
  cross(p->p1, p->p3, p->p2, &p->normalVector);
  p->d = (p->p1->x * p->normalVector.x) + (p->p1->y * p->normalVector.y) + (p->p1->z * p->normalVector.z);
}

void calcLineVector(Line* l) {
  l->vector.x = l->p1->x - l->p2->x;
  l->vector.y = l->p1->y - l->p2->y;
  l->vector.z = l->p1->z - l->p2->z;
}

void intersect(Plane* plane, Line* line, Point* pOut) {
  float distFromStart;
  float dot = (plane->normalVector.x * line->vector.x) +
              (plane->normalVector.y * line->vector.y) +
              (plane->normalVector.z * line->vector.z);
  distFromStart = -1 * (plane->d + (plane->normalVector.x * (plane->p1->x - line->p1->x)) +
                                   (plane->normalVector.y * (plane->p1->y - line->p1->y)) +
                                   (plane->normalVector.z * (plane->p1->z - line->p1->z)));
  distFromStart /= dot;
  printf("%f\n", distFromStart);
}


int main() {
  Point* p = malloc(sizeof(Point));
  Point* p2 = malloc(sizeof(Point));
  Point* p3 = malloc(sizeof(Point));
  Point* lp1 = malloc(sizeof(Point));
  Point* lp2 = malloc(sizeof(Point));
  if(p == NULL || p2 == NULL || p3 == NULL || lp1 == NULL || lp2 == NULL) {
    printf("some malloc failed");
    return -1;
  }
  p->x = 0.5;
  p->y = 2;

  p2->x = -1;
  p2->y = 2;

  p3->x = 0;
  p3->y = 0;
  Plane plane;
  plane.p1 = p3;
  plane.p2 = p;
  plane.p3 = p2;
  calcNormal(&plane);


  lp1->z = 1;

  lp2->z = 2;
  Line line;
  line.p1 = lp1;
  line.p2 = lp2;
  calcLineVector(&line);

  Point myInt;

  intersect(&plane, &line, &myInt);

  printf("normalVector: <%.2f, %f, %f>\n", plane.normalVector.x, plane.normalVector.y, plane.normalVector.z);
  return 0;
}
