const uint8_t HEADER1 = 0xAB;
const uint8_t HEADER2 = 0xCD;
const uint8_t NUM_SENSORS = 16;
const uint8_t TOTAL_PACKET_SIZE = 3 + NUM_SENSORS + 1; // Header + size + data + checksum

uint8_t buffer[TOTAL_PACKET_SIZE];

unsigned long lastLDRTime = 0;
const unsigned long LDR_TIMEOUT = 500;
bool ldrActive = false;

String LDR;
float angle = 90.0;
float last_angle = 90.0;

// Motor pins
const uint8_t m1 = 23;
const uint8_t in1 = 17;
const uint8_t in2 = 16;

const uint8_t m2 = 19;
const uint8_t in3 = 25;
const uint8_t in4 = 26;

const uint8_t m3 = 18;
const uint8_t in5 = 27;
const uint8_t in6 = 14;

const uint8_t m4 = 5;
const uint8_t in7 = 12;
const uint8_t in8 = 13;

const uint8_t motor_pins[12] = {m1, m2, m3, m4, in1, in2, in3, in4, in5, in6, in7, in8};

uint8_t ir_data[NUM_SENSORS];

const uint8_t low_speed = 150;
const uint8_t high_speed = 200;
const uint8_t no_speed = 0;

void setMotor(const uint8_t enable, const uint8_t input_1, const uint8_t input_2, uint8_t pwm, bool is_clockwise_rotation) {
  if (pwm == 0) {
    analogWrite(enable, 0);
    digitalWrite(input_1, LOW);
    digitalWrite(input_2, LOW);
  } else if (is_clockwise_rotation == true) {
    analogWrite(enable, pwm);
    digitalWrite(input_1, HIGH);
    digitalWrite(input_2, LOW);
  } else {
    analogWrite(enable, pwm);
    digitalWrite(input_1, LOW);
    digitalWrite(input_2, HIGH);
  }
}

void move0Deg() {
  setMotor(m1, in1, in2, high_speed, true);
  setMotor(m2, in3, in4, high_speed, true);
  setMotor(m3, in5, in6, high_speed, false);
  setMotor(m4, in7, in8, high_speed, false);
}

void move45Deg() {
  setMotor(m1, in1, in2, no_speed, true);
  setMotor(m2, in3, in4, high_speed, true);
  setMotor(m3, in5, in6, high_speed, false);
  setMotor(m4, in7, in8, no_speed, true);
}

void move90Deg() {
  setMotor(m1, in1, in2, high_speed, false);
  setMotor(m2, in3, in4, high_speed, true);
  setMotor(m3, in5, in6, high_speed, false);
  setMotor(m4, in7, in8, high_speed, true);
}

void move135Deg() {
  setMotor(m1, in1, in2, high_speed, false);
  setMotor(m2, in3, in4, no_speed, false);
  setMotor(m3, in5, in6, no_speed, false);
  setMotor(m4, in7, in8, high_speed, true);
}

void move180Deg() {
  setMotor(m1, in1, in2, high_speed, false);
  setMotor(m2, in3, in4, high_speed, false);
  setMotor(m3, in5, in6, high_speed, true);
  setMotor(m4, in7, in8, high_speed, true);
}

void move225Deg() {
  setMotor(m1, in1, in2, no_speed, false);
  setMotor(m2, in3, in4, high_speed, false);
  setMotor(m3, in5, in6, high_speed, true);
  setMotor(m4, in7, in8, no_speed, false);
}

void move270Deg() {
  setMotor(m1, in1, in2, high_speed, true);
  setMotor(m2, in3, in4, high_speed, false);
  setMotor(m3, in5, in6, high_speed, true);
  setMotor(m4, in7, in8, high_speed, false);
}

void move315Deg() {
  setMotor(m1, in1, in2, high_speed, true);
  setMotor(m2, in3, in4, no_speed, false);
  setMotor(m3, in5, in6, no_speed, false);
  setMotor(m4, in7, in8, high_speed, false);
}

void moveAround() {
  setMotor(m1, in1, in2, high_speed, true);
  setMotor(m2, in3, in4, high_speed, true);
  setMotor(m3, in5, in6, high_speed, true);
  setMotor(m4, in7, in8, high_speed, true);
}

void move(float angle) {
  if (angle >= 360.0) angle -= 360.0;

  if (angle >= 337.5 || angle < 22.5) move0Deg();
  else if (angle >= 22.5 && angle < 67.5) move45Deg();
  else if (angle >= 67.5 && angle < 112.5) move90Deg();
  else if (angle >= 112.5 && angle < 157.5) move135Deg();
  else if (angle >= 157.5 && angle < 202.5) move180Deg();
  else if (angle >= 202.5 && angle < 247.5) move225Deg();
  else if (angle >= 247.5 && angle < 292.5) move270Deg();
  else if (angle >= 292.5 && angle < 337.5) move315Deg();
  else moveAround();
}

float getAngle(uint8_t ir_data[16]) {
  int16_t ir_off = 0;
  float sum_angles = 0.0;
  for (uint8_t i = 0; i < 16; i++) {
    if (ir_data[i] == 0) {
      sum_angles += i * 22.5;
      ir_off++;
    }
  }
  if (ir_off == 0 || ir_off >= 10) return -1;
  return sum_angles / ir_off;
}

void printIrReadings(uint8_t ir[16]) {
  Serial.println(F("IR states:"));
  for (uint8_t i = 0; i < 16; i++) {
    Serial.print("IR");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(ir[i]);
    if ((i + 1) % 4 == 0) Serial.println();
    else Serial.print(F(", "));
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 32, 33); // RX1/TX1 para IR
  Serial2.begin(115200, SERIAL_8N1, 21, 22); // RX2/TX2 para LDR
  for (uint8_t i = 0; i < 12; i++) pinMode(motor_pins[i], OUTPUT);
}

void loop() {
  if (Serial2.available()) {
    String serialInput = Serial2.readStringUntil('\n');
    serialInput.trim();
    serialInput.toUpperCase();

    if (serialInput.startsWith("LDR:")) {
      String ldrValue = serialInput.substring(4);
      if (ldrValue == "ARRIBA" || ldrValue == "DERECHA" || ldrValue == "IZQUIERDA" || ldrValue == "ATRAS" ||
          ldrValue == "ARRIBA DERECHA" || ldrValue == "ARRIBA IZQUIERDA" ||
          ldrValue == "ATRAS DERECHA" || ldrValue == "ATRAS IZQUIERDA" ||
          ldrValue == "ARRIBA DERECHA IZQUIERDA" || ldrValue == "ATRAS DERECHA IZQUIERDA") {
        LDR = ldrValue;
        lastLDRTime = millis(); // Actualizar tiempo del último comando
        ldrActive = true;
        Serial.print("✅ LDR válido: ");
        Serial.println(LDR);
      } else {
        Serial.print("⚠️ LDR inválido: ");
        Serial.println(ldrValue);
      }
    }
    else if (serialInput.startsWith("CAM:")) {
      String camValue = serialInput.substring(4);
      Serial.print("🎥 Mensaje de cámara recibido: ");
      Serial.println(camValue);
      // Aquí NO haces nada con camValue (solo mostrar)
    }
    else {
      Serial.print("⚠️ Comando desconocido en Serial2: ");
      Serial.println(serialInput);
    }
  }

  // Leer datos IR desde Serial1
  if (Serial1.available() >= TOTAL_PACKET_SIZE) {
    Serial1.readBytes(buffer, TOTAL_PACKET_SIZE);

    if (buffer[0] == HEADER1 && buffer[1] == HEADER2 && buffer[2] == NUM_SENSORS) {
      uint8_t checksum = 0;
      for (int i = 0; i < TOTAL_PACKET_SIZE - 1; i++) {
        checksum ^= buffer[i];
      }

      if (checksum == buffer[TOTAL_PACKET_SIZE - 1]) {
        memcpy(ir_data, &buffer[3], NUM_SENSORS);
        angle = getAngle(ir_data);
        if (angle >= 0) last_angle = angle;
      } else {
        Serial.println("❌ Checksum inválido");
      }
    } else {
      Serial.println("❌ Cabecera o tamaño inválido");
    }
  }

  // Ejecutar movimiento basado en LDR o IR
  if (ldrActive && (millis() - lastLDRTime <= LDR_TIMEOUT)) {
    if (LDR == "ARRIBA") {
      move(270);
      Serial.println("move 270");
    }
    else if (LDR == "DERECHA") {
      move(180);
      Serial.println("move 180");
    }
    else if (LDR == "IZQUIERDA") {
      move(0);
      Serial.println("move 0");
    }
    else if (LDR == "ATRAS") {
      move(90);
      Serial.println("move 90");
    }
    else if (LDR == "ARRIBA DERECHA") {
      move(225);
      Serial.println("move 225");
    }
    else if (LDR == "ARRIBA IZQUIERDA") {
      move(315);
      Serial.println("move 315");
    }
    else if (LDR == "ATRAS DERECHA") {
      move(135);
      Serial.println("move 135");
    }
    else if (LDR == "ATRAS IZQUIERDA") {
      move(45);
      Serial.println("move 45");
    }
    else if (LDR == "ARRIBA DERECHA IZQUIERDA") {
      move(270);
      Serial.println("move 270");
    }
    else if (LDR == "ATRAS DERECHA IZQUIERDA") {
      move(90);
      Serial.println("move 90");
    }
  } else {
    ldrActive = false; // Finalizó el tiempo de control por LDR
    move(last_angle);
    Serial.println("move last_angle (modo IR)");
  }
}
