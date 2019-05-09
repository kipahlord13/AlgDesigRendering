//compile with gcc mThreadRendering.c -o mThreadRendering -lm -lX11 -lpthread
//compile for debugger: gcc -g mThreadRendering.c -o mThreadRendering -lm -lX11 -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <sys/param.h>
#include <sys/time.h>
#include <unistd.h>
#include <float.h>
#include <pthread.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define WHITE_PIXEL 16777215
#define GRAPH_PRECISION precision
#define GRAPH_TRIS GRAPH_PRECISION * GRAPH_PRECISION * 16
#define GRAPH_LOCS GRAPH_TRIS * 3
#define NUM_TRIS GRAPH_TRIS
#define R_THREADS 4
//formerly 14

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
  Point normalVector;
} Plane;

int graphNo;
int precision;
char graphString[3];

pthread_t xdrawThread;
pthread_mutex_t pixelLock;

char threadStarts[R_THREADS];

pthread_t renderThreads[R_THREADS];

struct timeval tv;

Display *d;
Window w;
XEvent e;
int s;

int animCounter;
char verbose;

Point camera = {0, 0, 0};

Point screenPoints[4] = {
  {1, 1, 1},
  {1, 1, -1},
  {1, -1, 1},
  {1, -1, -1}
};

Plane screenSpace = {&screenPoints[0], &screenPoints[1], &screenPoints[2]};

Plane frustumTop = {&camera, &screenPoints[0], &screenPoints[2]};
Plane frustumBot = {&camera, &screenPoints[3], &screenPoints[1]};
Plane frustumRight = {&camera, &screenPoints[2], &screenPoints[3]};
Plane frustumLeft = {&camera, &screenPoints[1], &screenPoints[0]};

//0 gets black
//1 gets white
int pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
int drawPixels[SCREEN_HEIGHT][SCREEN_WIDTH];
XImage* image;

float zBuf[SCREEN_HEIGHT][SCREEN_WIDTH];

int tPixels[R_THREADS][SCREEN_HEIGHT][SCREEN_WIDTH];
float tZBuf[R_THREADS][SCREEN_HEIGHT][SCREEN_WIDTH];

Point* vertexes;

short int* tris;

Plane* planes;

float uMin;
float uMax;
float vMin;
float vMax;
float uDiff;
float vDiff;

long long timeInMilliseconds(void) {
    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

long long timeInMicroseconds(void) {
  gettimeofday(&tv, NULL);
  return tv.tv_usec;
}

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

float dot(Point* v1, Point* v2) {
  return (v1->x * v2->x) +
         (v1->y * v2->y) +
         (v1->z * v2->z);
}

//upwards of the plane is when 1,2,3 are in clockwise order
void calcNormal(Plane* p) {
  cross(p->p1, p->p3, p->p2, &p->normalVector);
}

void calcLineVector(Line* l) {
  l->vector.x = l->p2->x - l->p1->x;
  l->vector.y = l->p2->y - l->p1->y;
  l->vector.z = l->p2->z - l->p1->z;
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

  pOut->x = line->p1->x + (line->vector.x * distFromStart);
  pOut->y = line->p1->y + (line->vector.y * distFromStart);
  pOut->z = line->p1->z + (line->vector.z * distFromStart);
}

float dist(Point* p1, Point* p2) {
  return sqrt(((p1->x - p2->x) * (p1->x - p2->x)) +
              ((p1->y - p2->y) * (p1->y - p2->y)) +
              ((p1->z - p2->z) * (p1->z - p2->z)));
}

int buildColor(double red, double green, double blue) {
    return(
        (((int)(red*255)%256)<<16)+
        (((int)(green*255)%256)<<8)+
        (((int)(blue*255)%256)));
}

void pVector(Point* p) {
  printf("vector: <%.2f, %.2f, %.2f>\n", p->x, p->y, p->z);
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

  pVector(&plane.normalVector);
}

int setupX11() {
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
  return 0;
}

int readPolyData() {
  vertexes = malloc(sizeof(Point) * GRAPH_TRIS);

  tris = malloc(sizeof(short int) * GRAPH_LOCS);

  planes = malloc(sizeof(Plane) * NUM_TRIS);

  FILE *file;
  char fileName[21];
  sprintf(fileName, "data/%dmeshVerts%d.txt", graphNo, precision);
  if((file = fopen(fileName, "r")) == NULL) {
    char cmd[24];
    sprintf(cmd, "java GeneratePolys %d %d", precision, graphNo);
    system(cmd);
    if((file = fopen(fileName, "r")) == NULL) {
      printf("verts not found");
      return 1;
    }
  }
  int row = 0;
  float firstF, secndF, thirdF;
  float cosTheta = cos(.15), sinTheta = sin(.15);
  while(fscanf(file, "%f %f %f", &firstF, &secndF, &thirdF) == 3) {

    //rotate round x axis
    // float  f2 = firstF, f3 = thirdF;
    // firstF = cosTheta * f2 - sinTheta * f3;
    // thirdF = sinTheta * f2 + cosTheta * f3;

    vertexes[row].x = firstF + 40;
    vertexes[row].y = secndF;
    vertexes[row].z = thirdF - 15;
    row++;
  }
  sprintf(fileName, "data/%dtriangles%d.txt", graphNo, precision);
  if((file = fopen(fileName, "r")) == NULL) {
    printf("tris not found");
    return 1;
  }
  row = 0;
  int firstI;
  while(fscanf(file, "%d", &firstI) == 1) {
    tris[row] = (short int)firstI;
    row++;
  }
}

void calcPlanes() {
  calcNormal(&screenSpace);
  calcNormal(&frustumTop);
  calcNormal(&frustumBot);
  calcNormal(&frustumRight);
  calcNormal(&frustumLeft);

  for(int i = 0; i < NUM_TRIS; i++) {
    planes[i].p1 = &vertexes[tris[3 * i]];
    planes[i].p2 = &vertexes[tris[3 * i + 1]];
    planes[i].p3 = &vertexes[tris[3 * i + 2]];
    calcNormal(&planes[i]);
  }
}

void setFarWhite() {
  for(int i = 0; i < SCREEN_WIDTH; i++) {
    for(int j = 0; j < SCREEN_HEIGHT; j++) {
      for(int k = 0; k < R_THREADS; k++) {
        tPixels[k][i][j] = WHITE_PIXEL;
        tZBuf[k][i][j] = FLT_MAX;
      }
      pixels[i][j] = WHITE_PIXEL;
      zBuf[i][j] = FLT_MAX;
    }
  }
}

void* renderLoop(void* threadNo) {
  char tNo = *(char*) threadNo;
  for(int i = tNo; i < NUM_TRIS; i+= R_THREADS) {
    //check polygon facing direction
    //check that normal vector dot product is greater than zero
    if((planes[i].normalVector.x * screenSpace.normalVector.x) +
       (planes[i].normalVector.y * screenSpace.normalVector.y) +
       (planes[i].normalVector.z * screenSpace.normalVector.z) > 0) continue;

    //check all three points within viewing
    char pointsOut = 3;
    Point distToScreenSpace = {planes[i].p1->x - camera.x,
      planes[i].p1->y - camera.y,
      planes[i].p1->z - camera.z};
    if(dot(&distToScreenSpace, &frustumTop.normalVector) > 0 ||
       dot(&distToScreenSpace, &frustumBot.normalVector) > 0 ||
       dot(&distToScreenSpace, &frustumRight.normalVector) > 0 ||
       dot(&distToScreenSpace, &frustumLeft.normalVector) > 0) pointsOut--;
    Point distToScreenSpace2 = {planes[i].p2->x - camera.x,
       planes[i].p2->y - camera.y,
       planes[i].p2->z - camera.z};
    if(dot(&distToScreenSpace2, &frustumTop.normalVector) > 0 ||
       dot(&distToScreenSpace2, &frustumBot.normalVector) > 0 ||
       dot(&distToScreenSpace2, &frustumRight.normalVector) > 0 ||
       dot(&distToScreenSpace2, &frustumLeft.normalVector) > 0) pointsOut--;
    Point distToScreenSpace3 = {planes[i].p3->x - camera.x,
       planes[i].p3->y - camera.y,
       planes[i].p3->z - camera.z};
    if(dot(&distToScreenSpace3, &frustumTop.normalVector) > 0 ||
       dot(&distToScreenSpace3, &frustumBot.normalVector) > 0 ||
       dot(&distToScreenSpace3, &frustumRight.normalVector) > 0 ||
       dot(&distToScreenSpace3, &frustumLeft.normalVector) > 0) pointsOut--;
    if(!pointsOut) continue;

    Point inter1;
    Line ray = {&camera, planes[i].p1};
    calcLineVector(&ray);
    intersect(&screenSpace, &ray, &inter1);
    Point inter2;
    Line ray2 = {&camera, planes[i].p2};
    calcLineVector(&ray2);
    intersect(&screenSpace, &ray2, &inter2);
    Point inter3;
    Line ray3 = {&camera, planes[i].p3};
    calcLineVector(&ray3);

    // pVector(planes[i].p1);
    // pVector(planes[i].p2);
    // pVector(planes[i].p3);

    intersect(&screenSpace, &ray3, &inter3);
    int p1y = floor((inter1.y - uMin) / vDiff * SCREEN_WIDTH);
    int p1z = floor((inter1.z - vMin) / vDiff * SCREEN_HEIGHT);
    int p2y = floor((inter2.y - uMin) / vDiff * SCREEN_WIDTH);
    int p2z = floor((inter2.z - vMin) / vDiff * SCREEN_HEIGHT);
    int p3y = floor((inter3.y - uMin) / vDiff * SCREEN_WIDTH);
    int p3z = floor((inter3.z - vMin) / vDiff * SCREEN_HEIGHT);

    Point point1 = {p1y, p1z, 0};
    Point point2 = {p2y, p2z, 0};
    Point point3 = {p3y, p3z, 0};

    int minY = MIN(p1y, MIN(p2y, p3y));
    int minZ = MIN(p1z, MIN(p2z, p3z));
    int maxY = MAX(p1y, MAX(p2y, p3y));
    int maxZ = MAX(p1z, MAX(p2z, p3z));

    //printf("maxY: %d minY: %d maxZ: %d minZ: %d\n", maxY, minY, maxZ, minZ);

    Point pixel = {0, 0, 0};
    for(int j = minY; j <= maxY; j++) {
      if(j < 0) {
        j = 0;
        continue;
      } else if(j >= SCREEN_HEIGHT) break;
      for(int k = minZ; k <= maxZ; k++) {
        if(k < 0) {
          k = 0;
          continue;
        } else if(k >= SCREEN_HEIGHT) break;
        pixel.x = j;
        pixel.y = k;
        if(cross2D(&point1, &point2, &pixel) >= 0 &&
           cross2D(&point2, &point3, &pixel) >= 0 &&
           cross2D(&point3, &point1, &pixel) >= 0) {

          float zVal;
          Point intersection;
          float realScreenY = screenPoints[3].y +
          ((float)(screenPoints[0].y - screenPoints[3].y) / SCREEN_HEIGHT * j);
          float realScreenZ = screenPoints[3].z +
          ((float)(screenPoints[0].z - screenPoints[3].z) / SCREEN_WIDTH * k);
          Point realScreenPoint = {camera.x + 1, realScreenY, realScreenZ};
          Line pixelRay = {&camera, &realScreenPoint, {0, 0, 0}};
          calcLineVector(&pixelRay);
          intersect(&planes[i], &pixelRay, &intersection);
          zVal = dist(&camera, &intersection);

          if(zVal <= tZBuf[tNo][j][k]) {
            tZBuf[tNo][j][k] = zVal;
            float grad = (float)j / SCREEN_HEIGHT;
            grad = MAX(0, MIN(1, grad));
            float zG = (zVal - 26) / 30.0;
            tPixels[tNo][j][k] = buildColor(zG, zG, zG);
          }
        }
      }
    }
  }
}

void animate() {
  float cosTheta = cos(.05), sinTheta = sin(.05);
  animCounter++;
  for(int i = 0; i < GRAPH_TRIS; i++) {
    //vertexes[i].z += animCounter % 100 < 50 ? .04 : -.04;

    float f2 = vertexes[i].x - 40, f3 = vertexes[i].y;
    vertexes[i].x = 40 + cosTheta * f2 - sinTheta * f3;
    vertexes[i].y = sinTheta * f2 + cosTheta * f3;
  }
}

void updateXBuffer() {
  pthread_mutex_lock(&pixelLock);
  memcpy(&drawPixels, &pixels, sizeof(pixels));
  pthread_mutex_unlock(&pixelLock);
  usleep(100);
}

void mergeBuffers() {
  memcpy(&pixels, &tPixels[0], sizeof(pixels));
  memcpy(&zBuf, &tZBuf[0], sizeof(zBuf));
  for(int i = 0; i < SCREEN_WIDTH; i++) {
    for(int j = 0; j < SCREEN_HEIGHT; j++) {
      for(int k = 1; k < R_THREADS; k++) {
        if(tZBuf[k][i][j] < zBuf[i][j]) {
          pixels[i][j] = tPixels[k][i][j];
          zBuf[i][j] = tZBuf[k][i][j];
        }
      }
    }
  }
}

void* mainLoop(void* in) {
  uMin = screenPoints[2].y;
  uMax = screenPoints[0].y;
  vMin = screenPoints[3].z;
  vMax = screenPoints[2].z;
  uDiff = uMax - uMin;
  vDiff = vMax - vMin;
  int frameCount = 0;
  long frameTime = 0, t = 0, avg = 0;
  long notXAvg = 0;
  while (1) {
    if(XCheckMaskEvent(d, ExposureMask | KeyPressMask, &e)) {
      if (e.type == KeyPress) {
        break;
      }
    }
    t = timeInMilliseconds();
    animate();
    setFarWhite();
    calcPlanes();
    for(int i = 0; i < R_THREADS; i++) {
      if(pthread_create(&renderThreads[i], NULL, renderLoop, (void*)&threadStarts[i])) {
        printf("Error making my thread, man!");
        return NULL;
      }
    }
    for(int i = 0; i < R_THREADS; i++) {
      if(pthread_join(renderThreads[i], NULL)) {
        printf("Error joining threads for %d", i);
        return NULL;
      }
    }
    mergeBuffers();
    if(verbose && frameCount) {
      frameTime = timeInMilliseconds() - t;
      notXAvg += frameTime;
    }
    updateXBuffer();
    if(verbose) {
      frameTime = timeInMilliseconds() - t;
      frameCount++;
      if(frameCount % 100 == 0) {
        printf("Frame number: %d \t Frame time: %ld", frameCount, frameTime);
      }
      avg += frameTime;
      if(frameCount % 100 == 0) {
        printf("\t Avg: %ld  \tRender time: %ld\n", notXAvg / frameCount, notXAvg);
        printf("\tTotal time: %ld\t\tAvg fps %.2f\n", avg, frameCount / (avg * .001));
      }
    }
  }
  XCloseDisplay(d);
}

void* xDrawLoop(void* in) {
  long drawStart = timeInMilliseconds();
  while(1) {
    pthread_mutex_lock(&pixelLock);
    if(image == NULL) {
      for(int i = 0; i < SCREEN_WIDTH; i++) {
        for(int j = 0; j < SCREEN_HEIGHT; j++) {
          int r = XSetForeground(d, DefaultGC(d, s), drawPixels[i][j]);
          if(r != 1) printf("Bad set foreground %d\n", r);
          r = XDrawPoint(d, w, DefaultGC(d, s), 599 - i, 599 - j);
          if(r != 1) printf("Bad draw point %d\n", r);
        }
      }
      image = XGetImage(d, w, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, AllPlanes, ZPixmap);
    } else {
      for(int i = 0; i < SCREEN_WIDTH; i++) {
        for(int j = 0; j < SCREEN_HEIGHT; j++) {
          XPutPixel(image, 599 - i, 599 - j, drawPixels[i][j]);
        }
      }
      XPutImage(d, w, DefaultGC(d, s), image, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    pthread_mutex_unlock(&pixelLock);
    XFlush(d);
    //printf("draw time: %lld\n", timeInMicroseconds() - drawStart);
    usleep(1666);
  }
}


int main(int argc, char* argv[]) {
  printf("Choose a function: ");
  scanf("%d", &graphNo);
  printf("Choose a precision: ");
  scanf("%d", &precision);
  readPolyData();
  verbose = argc != 1;
  XInitThreads();
  setupX11();
  for(int i = 0; i < SCREEN_WIDTH; i++) {
    for(int j = 0; j < SCREEN_HEIGHT; j++) {
      drawPixels[i][j] = WHITE_PIXEL;
    }
  }
  for(int i = 0; i < R_THREADS; i++) {
    threadStarts[i] = i;
  }
  if(pthread_mutex_init(&pixelLock, NULL)) {
    printf("Error getting mutex lul");
  }

  if(pthread_create(&xdrawThread, NULL, xDrawLoop, NULL)) {
    printf("Error making my thread, man!");
    return 1;
  }

  mainLoop(NULL);
  return 0;
}
