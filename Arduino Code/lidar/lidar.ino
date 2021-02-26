int lidar_frame[156];
int distances[188];
void setup() {
  Serial.begin(230400);
  Serial1.begin(230400);
  for(int a = 0; a < 188; a++){
    distances[a] = 0;
  }
}

void loop() {
 // while(!Serial.available());
  if(Serial1.read() == 170){
    GetLidarData();

    if(lidar_frame[2] == 1 && lidar_frame[3] == 97){
      int circleSegment = ((lidar_frame[10]*256+lidar_frame[11])/2250);
      for(int a = 0; a< 11; a++){
        int distance = (lidar_frame[13 + a*12]*256 + lidar_frame[14 + a*12]) * 0.25;
        if(distance < 8000){
        distances[circleSegment*11+a] = distance;
        }
      }
      

      
      String toPython = "";
      for(int a = 0; a < 188; a++){
        toPython.concat(distances[a]);
        toPython.concat(",");
      }
      Serial.println(toPython);
    }
    
  }
  
}

void GetLidarData(){
  for(int a = 0; a< 156; a++){
    while(!Serial1.available());
    lidar_frame[a] = Serial1.read();
  
  }
}
