color[] colorBuffer = new color[100];
float[] zBuffer = new float[100];

final color[] lineColors = {#FF0000, #00FF00, #0000FF};

final float camX = 300, camY = 300;

Point[] points = {new Point(camX, camY), new Point(380, 180), new Point(500, 180), new Point(400, 100), new Point(400, 200), new Point(500, 200), new Point(380, 100)};
Point screen0 = new Point(300, 200);
Point screen1 = new Point(400, 300);

//nice comment, does it work?d

final Line screenSpace = new Line(screen0, screen1);

Line[] lines = {new Line(points[1], points[2]), 
//                new Line(points[3], points[4]), 
//                new Line(points[5], points[6])
              };

float thetaRad = 0;

void setup() {
  size(600, 600);
}

void draw() {
  background(255);
  changeColor(0);
  ellipse(points[0].x, points[0].y, 10, 10);
  line(points[0].x + (900 * sin(thetaRad)), points[0].y + (900 * cos(thetaRad)), points[0].x - (900 * sin(thetaRad)), points[0].y - (900 * cos(thetaRad)));
  line(points[0].x + (900 * sin(thetaRad + (PI / 2))), points[0].y + (900 * cos(thetaRad + (PI / 2))), points[0].x - (900 * sin(thetaRad + (PI / 2))), points[0].y - (900 * cos(thetaRad + (PI / 2))));
  line(screenSpace.p1.x, screenSpace.p1.y, screenSpace.p2.x, screenSpace.p2.y);
  changeColor(#FF0000);
  line(points[1].x, points[1].y, points[2].x, points[2].y);
  line(points[0].x, points[0].y, points[2].x, points[2].y);
  line(points[1].x, points[1].y, points[0].x, points[0].y);
  ellipse(points[1].x, points[1].y, 5, 5);
  ellipse(points[2].x, points[2].y, 5, 5);
  //changeColor(#00FF00);
  //line(points[3].x, points[3].y, points[4].x, points[4].y);
  //line(points[0].x, points[0].y, points[4].x, points[4].y);
  //line(points[3].x, points[3].y, points[0].x, points[0].y);
  //ellipse(points[3].x, points[3].y, 5, 5);
  //ellipse(points[4].x, points[4].y, 5, 5);
  //changeColor(#0000FF);
  //line(points[5].x, points[5].y, points[6].x, points[6].y);
  //line(points[0].x, points[0].y, points[6].x, points[6].y);
  //line(points[5].x, points[5].y, points[0].x, points[0].y);
  //ellipse(points[5].x, points[5].y, 5, 5);
  //ellipse(points[6].x, points[6].y, 5, 5);

  //get line segment points
  for (int i = 0; i < 100; i++) {
    colorBuffer[i] = 255;
    zBuffer[i] = Float.MAX_VALUE;
  }
  int k = 0;
  for (Line l : lines) {
    k++;
    
    //clamp lines outside
    if(Point.cross(points[0], l.p1, screenSpace.p1) > 0) {
      println(l.m);
      println(new Line(points[0], screenSpace.p1).m);
      l = new Line(l.intersect(new Line(points[0], screenSpace.p1)), l.p2);
      println(l.p1.x + "   " + l.p1.y);
    }
    
    //cull lines outside
    if ((Point.cross(points[0], screenSpace.p1, l.p1) < 0 || Point.cross(points[0], screenSpace.p2, l.p1) > 0) &&
      (Point.cross(points[0], screenSpace.p1, l.p2) < 0 || Point.cross(points[0], screenSpace.p2, l.p2) > 0)) continue;

    //cull back face
    if (Point.cross(points[0], l.p1, l.p2) < 0) continue;


    Point intersect = new Line(points[0], l.p1).intersect(screenSpace);

    int screenLoc = int((intersect.x - screenSpace.p1.x) / (screenSpace.p2.x - screenSpace.p1.x) * 100);

    Point intersect2 = new Line(points[0], l.p2).intersect(screenSpace);

    int screenLoc2 = int((intersect2.x - screenSpace.p1.x) / (screenSpace.p2.x - screenSpace.p1.x) * 100);

    //if (screenLoc2 < screenLoc) {
    // if (screenLoc <= 100) screenLoc2 = 100;
    // else screenLoc = 0;
    //}
    

    for (int i = screenLoc; i <= screenLoc2; i++) {
      if (i < 0) i = 0;
      if (i >= colorBuffer.length) break;
      //calc z-value
      float xVal = screen1.x + ((screenSpace.p1.x - screenSpace.p2.x) * (100 - i)/100f);
      float yVal = screen1.y + ((screenSpace.p1.y - screenSpace.p2.y) * (100 - i)/100f);
      Point p = new Point(xVal, yVal);
      changeColor(0);

      float zVal = Float.MAX_VALUE;

      changeColor(#00FF00);
      
      //println(screenLoc);

      zVal = points[0].distance(l.intersect(new Line(points[0], p)));
      //println(zVal);
      //assign new color if better z-value
      if (zVal < zBuffer[i] && zVal > 100) {
        if (zBuffer[i] != Float.MAX_VALUE);
        zBuffer[i] = zVal;
        colorBuffer[i] = lineColors[k - 1];
      }
    }
  }

  //read from output buffer
  stroke(0);
  fill(255);
  rect(100, 400, 400, 4);
  noStroke();
  for (int i = 0; i < 100; i++) {
    fill(colorBuffer[i]);
    rect(101 + (4 * i), 401, 4, 4);
  }
  stroke(0);
  //for (int i = 100; i < 500; i += 4) line(i, 400, i, 404);

  if (keyPressed) {
    if (key == 'a') {
      points[1].x --;
    }
    if (key == 'd') {
      points[1].x ++;
    }
    if (key == 'w') {
      points[1].y --;
    }
    if (key == 's') {
      points[1].y ++;
    }
    if (key == 'j') {
      thetaRad += .01;
    }
    if (key == 'l') {
      thetaRad -= .01;
    }
    if (key == 'i') {
      points[0].y -= cos(thetaRad - (PI / 4));
      points[0].x -= sin(thetaRad - (PI / 4));
    }
    if (key == 'k') {
      points[0].y += cos(thetaRad - (PI / 4));
      points[0].x += sin(thetaRad - (PI / 4));
    }
    screenSpace.p1.x = points[0].x + (100 * cos(thetaRad + (PI / 2)));
    screenSpace.p1.y = points[0].y - (100 * sin(thetaRad + (PI / 2)));
    screenSpace.p2.x = points[0].x + (100 * cos(thetaRad));
    screenSpace.p2.y = points[0].y - (100 * sin(thetaRad));
    screenSpace.reSlope();
    lines[0].reSlope();
  }

  changeColor(#00FF00);
}

void changeColor(color c) {
  fill(c);
  stroke(c);
}

private static class Point {
  float x, y;
  public Point(float x, float y) {
    this.x = x;
    this.y = y;
  }
  public float distance(Point that) {
    return sqrt(pow(this.x - that.x, 2) + pow(this.y - that.y, 2));
  }
  //note that a is the shared point
  public static float cross(Point a, Point b, Point c) {
    float u1 = b.x - a.x, u2 = b.y - a.y, v1 = c.x - a.x, v2 = c.y - a.y;
    return (u1 * v2) - (v1 * u2);
  }
}

private class Line {
  Point p1, p2;
  float m;
  public Line(float x1, float y1, float x2, float y2) {
    p1 = new Point(x1, y1);
    p2 = new Point(x2, y2);
    reSlope();
  }
  public Line(Point p1, Point p2) {
    this.p1 = p1;
    this.p2 = p2;
    reSlope();
  }
  public Point intersect(Line that) {
    float xF, yF;
    if (this.m != Float.POSITIVE_INFINITY && that.m != Float.POSITIVE_INFINITY) {
      xF = (this.p1.y + (that.m * that.p1.x) - (that.p1.y + (this.m * this.p1.x))) / (that.m - this.m);
      yF = this.p1.y + this.m * (xF - this.p1.x);
    } else {
      if (this.m == Float.POSITIVE_INFINITY) {
        if (that.m == Float.POSITIVE_INFINITY) {
          return null;
        }
        xF = this.p1.x;
        yF = that.p1.y + that.m * (xF - that.p1.x);
      } else {
        xF = that.p1.x;
        yF = this.p1.y + this.m * (xF - this.p1.x);
      }
    }
    return new Point(xF, yF);
  }
  public void reSlope() {
    m = (p2.y - p1.y) / (p2.x - p1.x);
    if(m == Float.NEGATIVE_INFINITY) m = Float.POSITIVE_INFINITY;
  }
}