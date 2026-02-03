// ---------- CONFIG ----------
const int SENSOR_PIN = A0;
const int RELAY_PIN  = 2;

const int DRY_THRESHOLD = 550;
const int WET_THRESHOLD = 430;

const bool PUMP_ENABLED = false;

const int SAMPLE_COUNT = 5;
const unsigned long SAMPLE_INTERVAL_MS = 1000;

// ---------- STATE ----------
int samples[SAMPLE_COUNT];
int sampleIndex = 0;

bool pumpOn = false;
unsigned long lastSampleMs = 0;

// ---------- RELAY ----------
void relayOn()  { digitalWrite(RELAY_PIN, LOW);  } // active-LOW
void relayOff() { digitalWrite(RELAY_PIN, HIGH); }

// ---------- SAMPLING ----------
void recordSample(int value) {
  samples[sampleIndex] = value;
  sampleIndex = (sampleIndex + 1) % SAMPLE_COUNT;
}

int sampleCount() {
  return sampleIndex == 0 ? SAMPLE_COUNT : sampleIndex;
}

int averageMoisture() {
  int count = sampleCount();
  if (count == 0) return 0;

  long sum = 0;
  for (int i = 0; i < count; i++) {
    sum += samples[i];
  }
  return sum / count;
}

// ---------- CONTROL ----------
void updatePumpState(int avgMoisture) {
  bool wantOn  = (!pumpOn && avgMoisture >= DRY_THRESHOLD);
  bool wantOff = ( pumpOn && avgMoisture <= WET_THRESHOLD);

  if (!PUMP_ENABLED) {
    pumpOn = false;
    relayOff();
    return;
  }

  if (wantOn) {
    pumpOn = true;
    relayOn();
  }
  else if (wantOff) {
    pumpOn = false;
    relayOff();
  }
}

// ---------- LOGGING ----------
void logStatus(int raw, int avg) {
  Serial.print("raw=");
  Serial.print(raw);
  Serial.print(" avg=");
  Serial.print(avg);
  Serial.print(" pump=");
  Serial.println(pumpOn ? "ON" : "OFF");
}

// ---------- ARDUINO ----------
void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  relayOff();
}

void loop() {
  unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_INTERVAL_MS) return;

  lastSampleMs = now;

  int raw = analogRead(SENSOR_PIN);
  recordSample(raw);

  int avg = averageMoisture();
  updatePumpState(avg);
  logStatus(raw, avg);
}
