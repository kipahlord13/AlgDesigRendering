color[] colorBuffer = new color[100];
float[] zBuffer = new float[100];

final color[] lineColors = {#0000FF, #FF0000};

final float camX = 300, camY = 300;

Point[] points = {new Point(camX, camY), new Point(450, 55), new Point(570, 265), new Point(450, 105), new Point(570, 165)};
Point screen0 = new Point(300, 200);
Point screen1 = new Point(400, 300);

final Line screenSpace = new Line(screen0, screen1);

Line[] lines = {new Line(points[1], points[2]), new Line(points[3], points[4])};

void setup() {
  size(600, 600);
}

void draw() {
  background(255);
  changeColor(0);
  ellipse(camX, camY, 10, 10);
  line(camX, camY, 300, 0);
  line(camX, camY, 600, 300);
  line(300, 200, 400, 300);
  changeColor(#0000FF);
  line(points[1].x, points[1].y, points[2].x, points[2].y);
  line(points[0].x, points[0].y, points[2].x, points[2].y);
  line(points[1].x, points[1].y, points[0].x, points[0].y);
  ellipse(points[1].x, points[1].y, 5, 5);
  ellipse(points[2].x, points[2].y, 5, 5);
  changeColor(#FF0000);
  line(points[3].x, points[3].y, points[4].x, points[4].y);
  line(points[0].x, points[0].y, points[4].x, points[4].y);
  line(points[3].x, points[3].y, points[0].x, points[0].y);
  ellipse(points[3].x, points[3].y, 5, 5);
  ellipse(points[4].x, points[4].y, 5, 5);

  //get line segment points
  for (int i = 0; i < 100; i++) {
    colorBuffer[i] = 255;
    zBuffer[i] = Float.MAX_VALUE;
  }
  int k = 0;
  for (Line l : lines) {
    Point intersect = new Line(points[0], l.p1).intersect(screenSpace);

    int screenLoc = int((intersect.x - screenSpace.p1.x) / (screenSpace.p2.x - screenSpace.p1.x) * 100);

    Point intersect2 = new Line(points[0], l.p2).intersect(screenSpace);

    int screenLoc2 = int((intersect2.x - screenSpace.p1.x) / (screenSpace.p2.x - screenSpace.p1.x) * 100);

    for (int i = min(screenLoc, screenLoc2); i <= max(screenLoc, screenLoc2); i++) {
      if (i < 0) i = 0;
      if (i >= colorBuffer.length) break;
      //calc z-value
      float xVal = screen1.x + ((screenSpace.p1.x - screenSpace.p2.x) * (100 - i)/100f);
      float yVal = screen1.y + ((screenSpace.p1.y - screenSpace.p2.y) * (100 - i)/100f);
      Point p = new Point(xVal, yVal);
      changeColor(0);
      line(points[0].x, points[0].y, xVal, yVal);
      float zVal = Float.MAX_VALUE;
      Point p1 = l.intersect(new Line(points[0], p));
      ellipse(p1.x, p1.y, 5, 5);
      changeColor(#00FF00);
      Line myLine = new Line(points[0], p);
      line(l.p1.x, l.p1.y, l.p2.x, l.p2.y);
      zVal = p.distance(l.intersect(new Line(points[0], p)));

      //assign new color if better z-value
      if (zVal < zBuffer[i]) {
        if (zBuffer[i] != Float.MAX_VALUE);
        zBuffer[i] = zVal;
        colorBuffer[i] = lineColors[k];
      }
    }
    k++;
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
      points[0].x --;
    }
    if (key == 'l') {
      points[0].x ++;
    }
    if (key == 'i') {
      points[0].y --;
    }
    if (key == 'k') {
      points[0].y ++;
    }
    screenSpace.p1.x = points[0].x;
    screenSpace.p1.y = points[0].y - 100;
    screenSpace.p2.x = points[0].x + 100;
    screenSpace.p2.y = points[0].y;
    lines[0].reSlope();
  }

  changeColor(#00FF00);
}

void changeColor(color c) {
  fill(c);
  stroke(c);
}

private class Point {
  float x, y;
  public Point(float x, float y) {
    this.x = x;
    this.y = y;
  }
  public float distance(Point that) {
    return sqrt(pow(this.x - that.x, 2) + pow(this.y - that.y, 2));
  }
}

private class Line {
  Point p1, p2;
  float m;
  public Line(float x1, float y1, float x2, float y2) {
    p1 = new Point(x1, y1);
    p2 = new Point(x2, y2);
    m = (y2 - y1) / (x2 - x1);
  }
  public Line(Point p1, Point p2) {
    this.p1 = p1;
    this.p2 = p2;
    m = (p2.y - p1.y) / (p2.x - p1.x);
  }
  public Point intersect(Line that) {
    float xF, yF;
    xF = (this.p1.y + (that.m * that.p1.x) - (that.p1.y + (this.m * this.p1.x))) / (that.m - this.m); 
    yF = this.p1.y + this.m * (xF - this.p1.x);
    return new Point(xF, yF);
  }
  public void reSlope() {
    m = (p2.y - p1.y) / (p2.x - p1.x);
  }
}