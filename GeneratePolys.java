import java.util.ArrayList;
import java.util.Arrays;
import java.io.PrintWriter;
import java.util.Scanner;

public class GeneratePolys{

  public static int precision;
  private static Vector3[][] vectors;
  private static int dimension;
  private static int whichF = -1;

  private static ArrayList<Vector3> linePoints;
  public static Vector3[] meshVerts;
  private static int[] triangles;

  private static class Vector3 {
    float x, y, z;
    public Vector3(float x, float y, float z) {
      this.x = x; this.y = y; this.z = z;
    }
    public String toString() {
      return x + " " + y + " " + z + " ";
    }
  }

  public static void main(String[] args) throws Exception{
    if(args.length == 2) {
      precision = Integer.parseInt(args[0]);
      whichF = Integer.parseInt(args[1]);
    } else {
      System.out.println("Enter precision and then choose a fxn");
      Scanner scIn = new Scanner(System.in);
      precision = scIn.nextInt();
      whichF = scIn.nextInt();
    }
    dimension = 1 + (2 * precision);
    vectors = new Vector3[dimension][dimension];
    graphPoints();


    PrintWriter fileOut = new PrintWriter("data/" + whichF + "meshVerts" + precision + ".txt");
    for(Vector3 e : meshVerts) {
      fileOut.println(e);
    }
    fileOut.close();
    fileOut = new PrintWriter("data/" + whichF + "triangles" + precision + ".txt");
    for(int e : triangles) {
      fileOut.println(e);
    }
    fileOut.close();
  }

  static float f(float x, float y) {
    switch(whichF) {
      case 0: return 1 / (x + 10.1f);
      case 1: return 4 + ((.3f * x * x) / 4) - ((.3f * y * y) / 9);
      case 2: return (float)Math.sqrt((200) - (x * x) - (y * y));
      case 3: return (float)Math.sin(((x * x) + (y * y)) / 16);
    }
    return 0;

  }

  static private float findIteration() {
    return 10f / precision;
  }

  static void graphPoints() {
    float iteration = findIteration();
    int i, j = 0;
    //calculate all points
    for (float x = -10; x <= 10; x += iteration) {
      i = 0;
      for (float y = -10; y <= 10; y += iteration) {
        float value = f(x, y);
        //if (value > 10) value = 10;
        //if (value < -10) value = -10;
        vectors[i][j] = new Vector3(x, y, value);
        i++;
      }
      j++;
    }

    //create list for lineRenderer
    linePoints = new ArrayList<Vector3>();
    //horizonal lines
    for(i = 0; i <= precision * 2 ; i++) {
      for(j = 0; j <= precision * 2; j++) {
        linePoints.add(vectors[i][j]);
      }
      if (i++ == precision * 2) continue;
      for(j--; j >= 0; j--) {
        linePoints.add(vectors[i][j]);
      }
    }

    j = precision * 2; i = precision * 2;
    //vertical lines
    for(j = precision * 2; j >= 0; j--) {
      for(i = precision * 2; i >= 0; i--) {
        linePoints.add(vectors[i][j]);
      }
      if (j-- == 0) continue;
      for (i++; i <= precision * 2; i++) {
        linePoints.add(vectors[i][j]);
      }

    }

    //create mesh
    meshVerts = new Vector3[4 * (dimension - 1) * (dimension - 1)];
    for(int x = 0; x < dimension - 1; x++) {
      for (int y = 0; y < dimension - 1; y++) {
        meshVerts[4 * ((x * (dimension - 1)) + y)] = vectors[x][y];
        meshVerts[4 * ((x * (dimension - 1)) + y) + 1] = vectors[x + 1][y];
        meshVerts[4 * ((x * (dimension - 1)) + y) + 2] = vectors[x][y + 1];
        meshVerts[4 * ((x * (dimension - 1)) + y) + 3] = vectors[x+ 1][y + 1];
      }
    }

    triangles = new int[(dimension - 1) * (dimension - 1) * 12];
    for(i = 0; i < dimension - 1; i++) {
      for(j = 0; j < dimension - 1; j++) {
        int squareNo = ((i * (dimension - 1)) + j);
        triangles[(12 * squareNo) + 0] = 4 * squareNo;
        triangles[(12 * squareNo) + 1] = 4 * squareNo + 2;
        triangles[(12 * squareNo) + 2] = 4 * squareNo + 1;
        triangles[(12 * squareNo) + 3] = 4 * squareNo + 2;
        triangles[(12 * squareNo) + 4] = 4 * squareNo + 2 + 1;
        triangles[(12 * squareNo) + 5] = 4 * squareNo + 1;
        triangles[(12 * squareNo) + 6] = 4 * squareNo;
        triangles[(12 * squareNo) + 8] = 4 * squareNo + 2;
        triangles[(12 * squareNo) + 7] = 4 * squareNo + 1;
        triangles[(12 * squareNo) + 9] = 4 * squareNo + 2;
        triangles[(12 * squareNo) + 11] = 4 * squareNo + 2 + 1;
        triangles[(12 * squareNo) + 10] = 4 * squareNo + 1;
      }
    }
    return;
  }
}
