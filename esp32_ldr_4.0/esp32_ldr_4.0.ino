const int S0 = 33;
const int S1 = 25;
const int S2 = 26;
const int S3 = 27;
const int SIG = 32;

int valorLDR[16];
int ajusteUmbral = 300;

//HC-SR04 Pins
const uint8_t trig_pin = 18;
const uint8_t echo_pin = 19;

float float_distance;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(sig_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17); // RX = 16, TX = 17
  Serial2.begin(115200, SERIAL_8N1, 21, 22); // RX = 21, TX = 22
}

void loop() {
  for (int i = 0; i < 16; i++) {
    seleccionarCanal(i);
    delay(5);
    valorLDR[i] = analogRead(SIG);
  }

  if (valorDetectado(0, 3, 5000, 5000, 890, 1100)) {
    enviarComando("ARRIBA");
  } else if (valorDetectado(4, 7, 1588, 1554, 1328, 1532)) {
    enviarComando("DERECHA");
  } else if (valorDetectado(8, 11, 1841, 3989, 5000, 2288)) {
    enviarComando("ATRAS");
  } else if (valorDetectado(12, 15, 1136, 912, 1300, 1478)) {
    enviarComando("IZQUIERDA");
  } else if (valorDetectado(0, 3, 5000, 5000, 890, 1100) && valorDetectado(4, 7, 1588, 1554, 1328, 1532)) {
    enviarComando("ARRIBA DERECHA");
  } else if (valorDetectado(0, 3, 5000, 5000, 890, 1100) && valorDetectado(12, 15, 1136, 912, 1300, 1478)) {
    enviarComando("ARRIBA IZQUIERDA");
  } else if (valorDetectado(8, 11, 1841, 3989, 5000, 2288) && valorDetectado(4, 7, 1588, 1554, 1328, 1532)) {
    enviarComando("ATRAS DERECHA");
  } else if (valorDetectado(8, 11, 1841, 3989, 5000, 2288) && valorDetectado(12, 15, 1136, 912, 1300, 1478)) {
    enviarComando("ATRAS IZQUIERDA");
  } else if (valorDetectado(0, 3, 5000, 5000, 890, 1100) && valorDetectado(4, 7, 1588, 1554, 1328, 1532) && valorDetectado(12, 15, 1136, 912, 1300, 1478)) {
    enviarComando("ARRIBA DERECHA IZQUIERDA");
  } else if (valorDetectado(8, 11, 1841, 3989, 5000, 2288) && 
             valorDetectado(4, 7, 1588, 1554, 1328, 1532) && 
             valorDetectado(12, 15, 1136, 912, 1300, 1478)) {
    enviarComando("ATRAS DERECHA IZQUIERDA");
  }

  int int_distance = (int)readDistance();
  String mensajeDistancia = "DIS:" + String(int_distance);
  Serial2.println(mensajeDistancia);
  Serial.println("Distancia enviada: " + String(int_distance));


  if (Serial1.available()) {
    String camara = Serial1.readStringUntil('\n');
    camara.trim();
    Serial.println("Mensaje OpenMV " + camara);
    String camara2 = "CAM:" + camara;
    Serial2.println(camara2);
  }

  delay(10);
}

void seleccionarCanal(int canal) {
  digitalWrite(S0, canal & 1);
  digitalWrite(S1, (canal >> 1) & 1);
  digitalWrite(S2, (canal >> 2) & 1);
  digitalWrite(S3, (canal >> 3) & 1);
}

bool valorDetectado(int inicio, int fin, int um1, int um2, int um3, int um4) {
  int umbrales[] = {um1, um2, um3, um4};
  for (int i = inicio; i <= fin; i++) {
    if (valorLDR[i] > umbrales[i - inicio] + ajusteUmbral) {
      return true;
    }
  }
  return false;
}

void enviarComando(String comando) {
  String mensaje = "LDR:" + comando;
  Serial2.println(mensaje);
  Serial.println("Enviado al esclavo: " + comando);
}


//Calculation of distance in cm
float readDistance()
{
  //Variables to calculate de distance 
  float sound_speed = 0.034;
  int32_t duration;
  float distance_cm;
  
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  duration = pulseIn(echo_pin, HIGH);
  distance_cm = duration * sound_speed / 2;

  return distance_cm;
}