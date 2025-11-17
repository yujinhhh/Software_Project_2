#include <Arduino.h>

#define PIN_IR A0

const int numPoints = 7;
float xVals[numPoints] = {0, 50, 100, 150, 200, 250, 300};
float yVals[numPoints];

int degree;

// ---------------------------------------------
// Polynomial Curve Fitting (Normal Equation)
// ---------------------------------------------
void polynomialFit(int degree, float x[], float y[], int n, float coef[]) {
  int m = degree + 1;

  float X[10] = {0};

  for (int i = 0; i < 2 * m; i++) {
    X[i] = 0.0;
    for (int j = 0; j < n; j++) {
      X[i] += pow(x[j], i);
    }
  }

  float B[10] = {0};
  for (int i = 0; i < m; i++) {
    B[i] = 0.0;
    for (int j = 0; j < n; j++) {
      B[i] += y[j] * pow(x[j], i);
    }
  }

  float A[5][5] = {0};
  for (int i = 0; i < m; i++)
    for (int j = 0; j < m; j++)
      A[i][j] = X[i + j];

  // Gaussian elimination
  for (int i = 0; i < m; i++) {
    for (int k = i + 1; k < m; k++) {
      float t = A[k][i] / A[i][i];
      for (int j = 0; j < m; j++) {
        A[k][j] -= t * A[i][j];
      }
      B[k] -= t * B[i];
    }
  }

  for (int i = m - 1; i >= 0; i--) {
    coef[i] = B[i];
    for (int j = i + 1; j < m; j++) {
      coef[i] -= A[i][j] * coef[j];
    }
    coef[i] /= A[i][i];
  }
}

// ---------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Curve fitting 차수를 입력하세요 (1 또는 2):");

  while (Serial.available()) Serial.read();
  while (Serial.available() == 0);
  degree = Serial.parseInt();
  if (Serial.peek() == '\n') Serial.read();
  if (degree < 1 || degree > 2) degree = 2;

  Serial.print("선택된 차수: ");
  Serial.println(degree);
  delay(500);

  for (int i = 0; i < numPoints; i++) {
    Serial.print(xVals[i]);
    Serial.println(" mm 위치로 이동한 뒤 엔터를 누르세요.");

    while (Serial.available() == 0);
    Serial.read();

    int sensor = analogRead(PIN_IR);
    yVals[i] = sensor;

    Serial.print("Distance ");
    Serial.print(xVals[i]);
    Serial.print(" mm → Sensor = ");
    Serial.println(sensor);

    delay(300);
  }

  float coef[5];
  polynomialFit(degree, xVals, yVals, numPoints, coef);

  Serial.println("\n=== Fitted Equation ===");
  Serial.print("distance = ");

  for (int i = 0; i <= degree; i++) {
    Serial.print(coef[i], 6);
    Serial.print("*x");
    if (i > 0) Serial.print("^" + String(i));
    if (i < degree) Serial.print(" + ");
  }
  Serial.println(";");

  Serial.println("\nCoefficients:");
  for (int i = 0; i <= degree; i++) {
    Serial.print("a");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println(coef[i], 6);
  }
}

void loop() {}
