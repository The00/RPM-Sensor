#include <rpm.h>

rpm_data data;

void setup() {
rpm_init(1);
Serial.begin(9600);
}

void loop() {
  
  data = get_rpm_data(30,100); // perform a rpm measure. Target 30 sample
                                // timeout = 100ms.
  Serial.print("standard deviation :");
  Serial.println(data.sd);
  
  Serial.print("measure time :");
  Serial.println(data.time);
  
  Serial.print("nbr of sample :"); // the number of sample will be lower than the target because of bad sample took off
  Serial.println(data.nbr_sample_r);
  
  Serial.print("rpm :");
  Serial.println(data.rpm);
  
  Serial.println();
  delay(1000);
}
