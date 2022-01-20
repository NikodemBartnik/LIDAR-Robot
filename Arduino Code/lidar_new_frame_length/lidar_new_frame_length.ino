int lidar_frame[140];
int distances[240];
void setup() {
  Serial.begin(230400);
  Serial1.begin(230400);
  for(int a = 0; a < 240; a++){
    distances[a] = 0;
  }
}

void loop() {
 // while(!Serial.available());
  if(Serial1.read() == 170){
    GetLidarData();

    if(lidar_frame[2] == 1 && lidar_frame[3] == 97){
      int circleSegment = ((lidar_frame[10]*256+lidar_frame[11])/2250);
      int points = (lidar_frame[6] - 5) / 3;
      if(points > 30){
        for(int p = 0; p < 15; p++){
          
          int distance = (lidar_frame[13 + p*6]*256 + lidar_frame[14 + p*6]) * 0.25;
          if(distance < 6000){
            distances[circleSegment*15+p] = distance;
          }
        }
      }
    }

          
      String toPython = "";
      for(int a = 0; a < 240; a++){
        toPython.concat(distances[a]);
        toPython.concat(",");
      }
      Serial.println(toPython);
    } 
  }

void GetLidarData(){
  for(int a = 0; a< 130; a++){
    while(!Serial1.available());
    lidar_frame[a] = Serial1.read();
  }
}
