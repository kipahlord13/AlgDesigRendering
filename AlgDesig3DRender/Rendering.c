//compile with gcc Rendering.c -o Rendering -lm


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <string.h>
#include <sys/param.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define NUM_TRIS 4

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

Point camera = {0, 0, 0};

Point screenPoints[4] = {
  {1, 1, 1},
  {1, 1, -1},
  {1, -1, 1},
  {1, -1, -1}
};
Plane screenSpace = {&screenPoints[0], &screenPoints[1], &screenPoints[2]};

//0 gets black
//1 gets white
char pixels[SCREEN_HEIGHT][SCREEN_WIDTH];

short int tris[NUM_TRIS * 3] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

Point verticies[NUM_TRIS * 3] = {
  {4, -1, 2},
  {4, -3, 2},
  {2, -2, 2 - 1.732},

  {6, 1, 1},
  {3, 2, 1},
  {3, 1, 2},

  {8, -6, -6},
  {10, -6, -6},
  {10, -8, -6},

  {8, -8, -6},
  {8, -6, -6},
  {10, -8, -6}
};

Plane planes[NUM_TRIS];




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

float cross2D(Point* p1, Point* p2, Point* p3) {
  return ((p2->x - p1->x) * (p3->y - p1->y)) - ((p2->y - p1->y) * (p3->x - p1->x));
}

//upwards of the plane is when 1,2,3 are in clockwise order
void calcNormal(Plane* p) {
  cross(p->p1, p->p3, p->p2, &p->normalVector);
  p->d = (p->p1->x * p->normalVector.x) + (p->p1->y * p->normalVector.y) + (p->p1->z * p->normalVector.z);

}

void calcLineVector(Line* l) {
  l->vector.x = l->p2->x - l->p1->x;
  l->vector.y = l->p2->y - l->p1->y;
  l->vector.z = l->p2->z - l->p1->z;
  printf("lineVector: <%.2f, %.2f, %.2f>\n", l->vector.x, l->vector.y, l->vector.z);

}

void intersect(Plane* plane, Line* line, Point* pOut) {
  float distFromStart;
  float dot = (plane->normalVector.x * line->vector.x) +
              (plane->normalVector.y * line->vector.y) +
              (plane->normalVector.z * line->vector.z);
  distFromStart = ((plane->normalVector.x * (plane->p1->x - line->p1->x)) +
                   (plane->normalVector.y * (plane->p1->y - line->p1->y)) +
                   (plane->normalVector.z * (plane->p1->z - line->p1->z)));
  distFromStart /= dot;
  printf("%f\n", distFromStart);

  pOut->x = line->p1->x + (line->vector.x * distFromStart);
  pOut->y = line->p1->y + (line->vector.y * distFromStart);
  pOut->z = line->p1->z + (line->vector.z * distFromStart);
  printf("point: (%.2f, %.2f, %.2f)\n", pOut->x, pOut->y, pOut->z);
}

void testFunctions() {
  Point* p = malloc(sizeof(Point));
  Point* p2 = malloc(sizeof(Point));
  Point* p3 = malloc(sizeof(Point));
  Point* lp1 = malloc(sizeof(Point));
  Point* lp2 = malloc(sizeof(Point));
  if(p == NULL || p2 == NULL || p3 == NULL || lp1 == NULL || lp2 == NULL) {
    printf("some malloc failed\n");
    return;
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


  lp1->z = 2;

  lp2->z = 1;
  Line line;
  line.p1 = lp1;
  line.p2 = lp2;
  calcLineVector(&line);

  Point myInt;

  intersect(&plane, &line, &myInt);

  printf("normalVector: <%.2f, %.2f, %.2f>\n", plane.normalVector.x, plane.normalVector.y, plane.normalVector.z);
}

void calcPlanes() {
  calcNormal(&screenSpace);

  for(int i = 0; i < NUM_TRIS; i++) {
    planes[i].p1 = &verticies[3 * i];
    planes[i].p2 = &verticies[(3 * i) + 1];
    planes[i].p3 = &verticies[(3 * i) + 2];
    calcNormal(&planes[i]);
  }
}

int setupX11() {
  Display *d;
  Window w;
  XEvent e;
  int s;

  d = XOpenDisplay(NULL);
  if (d == NULL) {
     fprintf(stderr, "Cannot open display\n");
     exit(1);
  }

  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, SCREEN_WIDTH, SCREEN_HEIGHT, 40,
                          BlackPixel(d, s), WhitePixel(d, s));
  XSelectInput(d, w, ExposureMask | KeyPressMask);
  XMapWindow(d, w);

  while (1) {
     XNextEvent(d, &e);
     if (e.type == Expose) {
       for(int i = 0; i < SCREEN_HEIGHT; i++) {
         for(int j = 0; j < SCREEN_WIDTH; j++) {
           if(pixels[i][j] == 1) {
             XDrawPoint(d, w, DefaultGC(d, s), i, j);
           }
         }
       }
     }
     if (e.type == KeyPress)
        break;
  }

  XCloseDisplay(d);
  return 0;
}


int main() {
  calcPlanes();
  printf("screenSpace: <%.2f, %.2f, %.2f>\n", screenSpace.normalVector.x, screenSpace.normalVector.y, screenSpace.normalVector.z);
  //begin raster loop
  float uMin = screenPoints[2].y;
  float uMax = screenPoints[0].y;
  float vMin = screenPoints[3].y;
  float vMax = screenPoints[1].y;
  float uDiff = uMax - uMin;
  float vDiff = vMax - vMin;
  for(int i = 0; i < NUM_TRIS; i++) {
    printf("Triangle number %d\n", i);
    Point inter1;
    Line ray = {&camera, &verticies[3 * i]};
    calcLineVector(&ray);
    intersect(&screenSpace, &ray, &inter1);
    Point inter2;
    Line ray2 = {&camera, &verticies[3 * i + 1]};
    calcLineVector(&ray2);
    intersect(&screenSpace, &ray2, &inter2);
    Point inter3;
    Line ray3 = {&camera, &verticies[3 * i + 2]};
    calcLineVector(&ray3);
    intersect(&screenSpace, &ray3, &inter3);
    int p1y = floor((inter1.y - uMin) / vDiff * SCREEN_WIDTH);
    int p1z = floor((inter1.z - vMin) / vDiff * SCREEN_WIDTH);
    printf("%f ", inter1.y);
    printf("%f\n", inter1.z);
    int p2y = floor((inter2.y - uMin) / vDiff * SCREEN_WIDTH);
    int p2z = floor((inter2.z - vMin) / vDiff * SCREEN_WIDTH);
    printf("%f ", inter2.y);
    printf("%f\n", inter2.z);
    int p3y = floor((inter3.y - uMin) / vDiff * SCREEN_WIDTH);
    int p3z = floor((inter3.z - vMin) / vDiff * SCREEN_WIDTH);
    printf("%f ", inter3.y);
    printf("%f\n", inter3.z);

    Point point1 = {p1y, p1z, 0};
    Point point2 = {p2y, p2z, 0};
    Point point3 = {p3y, p3z, 0};

    int minY = MIN(p1y, MIN(p2y, p3y));
    int minZ = MIN(p1z, MIN(p2z, p3z));
    int maxY = MAX(p1y, MAX(p2y, p3y));
    int maxZ = MAX(p1z, MAX(p2z, p3z));
    Point pixel = {0, 0, 0};
    for(int j = minY; j <= maxY; j++) {
      if(j < 0 || j >= SCREEN_HEIGHT) continue;
      for(int k = minZ; k <= maxZ; k++) {
        if(k < 0 || k >= SCREEN_WIDTH) continue;
        pixel.x = j;
        pixel.y = k;
        if(cross2D(&point1, &point2, &pixel) >= 0 &&
           cross2D(&point2, &point3, &pixel) >= 0 &&
           cross2D(&point3, &point1, &pixel) >= 0) {
          //printf("%.2f\n", cross2D(&point1, &point2, &pixel));
          pixels[j][k] = 1;
        }
      }
    }

    pixels[p1y][p1z] = 1;
    pixels[p2y][p2z] = 1;
    pixels[p3y][p3z] = 1;
  }
  setupX11();
  //testFunctions();

  return 0;
}
