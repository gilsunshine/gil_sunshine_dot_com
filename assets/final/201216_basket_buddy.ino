//physical pin 11
#define stepPin1 0
//physical pin 12:
#define dirPin1 2

//physical pin 7:
#define stepPin2 4
//physical pin 8:
#define dirPin2 3

//physical pin 3:
#define stepPin3 1
//physical pin 13:
#define dirPin3 13

#define MMPERSTEP 0.1696

float lengths[32];

int numLengths = sizeof(lengths) / sizeof(lengths[0]);
int count = 0;
int markDist = 500;
int cutSteps = 600;
int lengthCount = 0;
int buffCount = 0;

char floatBuffer[32];
char inByte;

float cutDist = 77 / MMPERSTEP;

bool runProgram = false;
bool cut = false;
bool start = true;
bool incoming = false;

#define stepsPerRevolution 200

void setup() {
  Serial.begin(115200);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
}

void loop() {

  if (Serial.available() > 0) {
    inByte = Serial.read();
    if (inByte == '<') {
      incoming = true;
      lengths[lengthCount] = 5;
      lengthCount++;
    }

    if (inByte == '/') {
      incoming = false;
      lengths[lengthCount] = atof(floatBuffer);
      lengthCount++;
      buffCount = 0;
      memset(floatBuffer, 0, sizeof(floatBuffer));
    } else {
      incoming = true;
    }

    if (incoming == true && lengthCount != 0 && inByte != '<' && inByte != '>' && inByte != '/') {
      floatBuffer[buffCount] = inByte;
      buffCount++;
    }

    if (inByte == '>') {
      lengths[lengthCount] = atof(floatBuffer);
      lengthCount++;
      buffCount = 0;
      memset(floatBuffer, 0, sizeof(floatBuffer));
      lengths[lengthCount] = 5;
      incoming = false;
      for (int i = 0; i < sizeof(lengths) / sizeof(float); i++) {
        lengths[i] = lengths[i] / MMPERSTEP;
      }
      lengthCount = 0;
    }

    if (inByte == 'c') {
      for (int i = 0; i < sizeof(lengths) / sizeof(float); i++) {
        Serial.println(lengths[i]);
      }
      memset(floatBuffer, 0, sizeof(floatBuffer));

    }
    if (inByte == 'x') {
      for (int i = 0; i < sizeof(lengths) / sizeof(float); i++) {
        lengths[i] = 0;
      }
      memset(floatBuffer, 0, sizeof(floatBuffer));

    }


    if (inByte == 'r') {
      runProgram = true;
      memset(floatBuffer, 0, sizeof(floatBuffer));

    }
  }

  if (runProgram) {
    if (start) {

      digitalWrite(dirPin1, LOW);
      for (int i = 0; i < 20 / MMPERSTEP; i++) {
        digitalWrite(stepPin1, HIGH);
        delayMicroseconds(2000);
        digitalWrite(stepPin1, LOW);
        delayMicroseconds(2000);
      }
      delay(500);
      digitalWrite(dirPin3, LOW);
      for (int i = 0; i < cutSteps; i++) {
        digitalWrite(stepPin3, HIGH);
        delayMicroseconds(8000);
        digitalWrite(stepPin3, LOW);
        delayMicroseconds(8000);
      }
      delay(500);
      digitalWrite(dirPin3, HIGH);
      for (int i = 0; i < cutSteps; i++) {
        digitalWrite(stepPin3, HIGH);
        delayMicroseconds(8000);
        digitalWrite(stepPin3, LOW);
        delayMicroseconds(8000);
      }
      delay(500);
      cut = false;

      if (cutDist > lengths[0]) {
        digitalWrite(dirPin1, HIGH);
        for (int i = 0; i < cutDist - lengths[0]; i++) {
          digitalWrite(stepPin1, HIGH);
          delayMicroseconds(2000);
          digitalWrite(stepPin1, LOW);
          delayMicroseconds(2000);
        }
      }

      if (cutDist <= lengths[0]) {
        digitalWrite(dirPin1, LOW);
        for (int i = 0; i < lengths[0] - cutDist; i++) {
          digitalWrite(stepPin1, HIGH);
          delayMicroseconds(2000);
          digitalWrite(stepPin1, LOW);
          delayMicroseconds(2000);
        }
      }
      //skip the first length during marking
      count++;

      start = false;

    } else {
      if (count < numLengths) {
        delay(250);
        digitalWrite(dirPin2, HIGH);
        for (int i = 0; i < markDist; i++) {
          digitalWrite(stepPin2, HIGH);
          delayMicroseconds(2000);
          digitalWrite(stepPin2, LOW);
          delayMicroseconds(2000);
        }

        delay(250);

        digitalWrite(dirPin2, LOW);
        for (int i = 0; i < markDist; i++) {
          digitalWrite(stepPin2, HIGH);
          delayMicroseconds(2000);
          digitalWrite(stepPin2, LOW);
          delayMicroseconds(2000);
        }
      }

      delay(250);

      digitalWrite(dirPin1, LOW);
      if (lengths[count] != 0) {
        for (int i = 0; i < lengths[count]; i++) {
          digitalWrite(stepPin1, HIGH);
          delayMicroseconds(2000);
          digitalWrite(stepPin1, LOW);
          delayMicroseconds(2000);
        }
        delay(500);
        count++;
      } else {
        cut = true;
      }
    }
    if (cut) {
      digitalWrite(dirPin1, LOW);
      for (int i = 0; i < cutDist; i++) {
        digitalWrite(stepPin1, HIGH);
        delayMicroseconds(2000);
        digitalWrite(stepPin1, LOW);
        delayMicroseconds(2000);
      }
      delay(500);
      digitalWrite(dirPin3, LOW);
      for (int i = 0; i < cutSteps; i++) {
        digitalWrite(stepPin3, HIGH);
        delayMicroseconds(8000);
        digitalWrite(stepPin3, LOW);
        delayMicroseconds(8000);
      }
      delay(500);
      digitalWrite(dirPin3, HIGH);
      for (int i = 0; i < cutSteps; i++) {
        digitalWrite(stepPin3, HIGH);
        delayMicroseconds(8000);
        digitalWrite(stepPin3, LOW);
        delayMicroseconds(8000);
      }
      cut = false;
      start = true;
      runProgram = false;
      count = 0;
    }
  }
}
