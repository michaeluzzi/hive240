int micValue = 0;
int baseline = 512; // equivalent to 0 in audio signal range 0 - 1024
int threshold = 25; // above this level is considered a spike
unsigned long time;
unsigned long previousTime;
int numSamples = 0; // for calculating avgAmplitude each minute
long cumulativeValue = 0; // cumulativeValue/numSamples = avgAmplitude
int numSpikes = 0; // tally numSpikes each minute

void setup()
{
  Serial.begin(115200);
  pinMode(4, OUTPUT); // LED data transmission spike indicator
  pinMode(6, OUTPUT); // LED data transmission every 60 seconds indicator
  time = millis();
}

void loop()
{
    time = millis(); // get current time
    numSamples++; // increment numSamples
    micValue = analogRead(A0); // get sensor value
    long amplitude = abs(baseline - micValue); // use absolute value of difference between baseline and micValue
    cumulativeValue += amplitude; // increment cumulativeValue
    
    // if spike, increment numSpikes, set LED indicator, transmit message to idigi
    if (amplitude > threshold)
    {
      numSpikes++;
      digitalWrite(4, HIGH);
      String name = "spikes_front";
      String units = "amplitude";
      String post_message = "idigi_data:names=" + name + "&values=" + amplitude*100 + "&units=" + units;
      Serial.println(post_message);
      delay(50);
      digitalWrite(4, LOW);
    }
    
    // every 60 seconds, set LED indicator, calculate and transmit avgAmplitude and numSpikes, reset vars to 0
    if ((time - previousTime) >= 60000)
    {
      digitalWrite(6, HIGH);
      float avgAmplitude = float(cumulativeValue) / float(numSamples);
      String names = "average_amplitude_front,num_spikes_front";
      String units = "amplitude,integer";
      String post_message = "idigi_data:names=" + names + "&values=" + int(avgAmplitude*100)+","+numSpikes + "&units=" + units;
      Serial.println(post_message);
      delay(50);
      numSamples = 0;
      cumulativeValue = 0;
      previousTime = time;
      numSpikes = 0;
      digitalWrite(6, LOW);
    }
    
    delay(10);
    
}
