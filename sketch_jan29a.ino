// ---------- CONFIG ----------
const int SENSOR_PIN = A0;
const int RELAY_PIN  = 2;

const int DRY_THRESHOLD = 550;
const int WET_THRESHOLD = 430;

const bool DRY_RUN_MODE = true;

const int SAMPLE_COUNT = 3;
const unsigned long SAMPLE_INTERVAL_MS = 800;

// ---------- STATE ----------
int samples[SAMPLE_COUNT];
int sampleIndex = 0;
bool samplesWrapped = false;

bool pumpOn = false;
unsigned long lastSampleMs = 0;

// ---------- RELAY ----------
inline void relayOn()  { digitalWrite(RELAY_PIN, LOW);  } // active-LOW
inline void relayOff() { digitalWrite(RELAY_PIN, HIGH); }

// ---------- SAMPLING ----------
void recordSample(int value) {
  samples[sampleIndex] = value;
  sampleIndex++;

  if (sampleIndex >= SAMPLE_COUNT) {
    sampleIndex = 0;
    samplesWrapped = true;
  }
}

int sampleCount() {
  return samplesWrapped ? SAMPLE_COUNT : sampleIndex;
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

bool shouldTurnPumpOn(int avgMoisture) {
  return !pumpOn && avgMoisture >= DRY_THRESHOLD;
}

bool shouldTurnPumpOff(int avgMoisture) {
  return pumpOn && avgMoisture <= WET_THRESHOLD;
}

// ---------- CONTROL ----------
void updatePumpState(int avgMoisture) {
  if (shouldTurnPumpOn(avgMoisture)) {
    pumpOn = true;

    if (DRY_RUN_MODE) {
      Serial.println("DRY-RUN: pump ON");
    } else {
      relayOn();
    }
    return;
  }

  if (shouldTurnPumpOff(avgMoisture)) {
    pumpOn = false;

    if (DRY_RUN_MODE) {
      Serial.println("DRY-RUN: pump OFF");
    } else {
      relayOff();
    }
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
