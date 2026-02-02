const int SENSOR_PIN = A0;
const int RELAY_PIN  = 2;

const int DRY_THRESHOLD = 550; // pump ON at/above this
const int WET_THRESHOLD = 430; // pump OFF at/below this

bool pumpOn = false;

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);

  // active-LOW relay: HIGH = OFF
  digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
  int moisture = analogRead(SENSOR_PIN);

  if (!pumpOn && moisture >= DRY_THRESHOLD) {
    pumpOn = true;
    digitalWrite(RELAY_PIN, LOW);   // relay ON → pump ON
  } 
  else if (pumpOn && moisture <= WET_THRESHOLD) {
    pumpOn = false;
    digitalWrite(RELAY_PIN, HIGH);  // relay OFF → pump OFF
  }

  Serial.print("raw=");
  Serial.print(moisture);
  Serial.print(" pump=");
  Serial.println(pumpOn ? "ON" : "OFF");

  delay(5000);
}
