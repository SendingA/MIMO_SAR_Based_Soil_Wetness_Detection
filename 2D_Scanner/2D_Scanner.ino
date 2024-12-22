const int DIRECTION_X_PIN = 7;  // X轴步进电机方向引脚
const int SPEED_PUL_X_PIN = 10; // X轴步进电机脉冲引脚
const int DIRECTION_Y_PIN = 8;  // Y轴步进电机方向引脚
const int SPEED_PUL_Y_PIN = 9; // Y轴步进电机脉冲引脚

String comdata = "";   // 串口接收的数据
int stepperSpeed = 0;  // 步进电机速度
double x_move_distance = 400000;  // X轴的移动幅  全长/建议大概45000
double y_move_distance = 7590;  // Y轴的移动幅度 全长大概500000，建议用480000


void setup() {
  // 初始化串口通讯
  Serial.begin(9600);
  
  // 设置引脚模式
  pinMode(DIRECTION_X_PIN, OUTPUT);
  pinMode(SPEED_PUL_X_PIN, OUTPUT);
  pinMode(DIRECTION_Y_PIN, OUTPUT);
  pinMode(SPEED_PUL_Y_PIN, OUTPUT);
}

void loop() {
  // 获取串口数据
  while (Serial.available() > 0) {
    char val = char(Serial.read());
    if ((val != '\r') && (val != '\n')) {
      comdata += val;
    }
    delay(2);
  }

  // 如果收到数据，进行处理
  if (comdata.length() > 0) {
    Serial.println(comdata + " rcv ok");
    
    if (comdata == "start") {
      // 开始弓形运动
      startArcMovement();
    } 
    else if(comdata.startsWith("resetxn")) {//向右
      // 比如：resetxn 450000
      String distanceStr = comdata.substring(7); // 提取距离
      double distance = distanceStr.toDouble();
      if (distance > 0) {
        resetXPosition(distance);
      }
    }
    else if(comdata.startsWith("resetx")) {//向左
      // 比如：resetx 450000
      String distanceStr = comdata.substring(6); // 提取距离
      double distance = distanceStr.toDouble();
      if (distance > 0) {
        resetXNPosition(distance);
      }
    } 
    else if(comdata.startsWith("resetyn")) {//向下
      // 比如：resetyn 500000
      String distanceStr = comdata.substring(7); // 提取距离
      double distance = distanceStr.toDouble();
      if (distance > 0) {
        resetYNPosition(distance);
      }
    }
    else if(comdata.startsWith("resety")) {//向上
      // 比如：resety 500000
      String distanceStr = comdata.substring(6); // 提取距离
      double distance = distanceStr.toDouble();
      if (distance > 0) {
        resetYPosition(distance);
      }
    }

    else {
      // 处理速度与幅度控制
      boolean is_number = true;
      for (int i = 0; i < comdata.length(); i++) {
        if (!(isDigit(comdata[i]))) {
          is_number = false;
        }
      }
      
      if (is_number) {
        // 设置速度和运动幅度
        stepperSpeed = comdata.toInt();  // 设置步进电机的速度
        Serial.println("stepperSpeed: " + comdata);
      }
    }
    comdata = "";  // 清空接收到的数据
  }
}

// 控制步进电机脉冲频率
void controlStepperSpeed(int pulsePin, int directionPin, int speed, int direction) {
  int pulseInterval = 20;  // 计算脉冲间隔
  if(direction == 1){//右上
    digitalWrite(directionPin, HIGH);
  }else{//左下
    digitalWrite(directionPin, LOW);     // 设置方向
  }
  digitalWrite(pulsePin, HIGH);         // 发送脉冲
  delayMicroseconds(pulseInterval / 2);
  digitalWrite(pulsePin, LOW);
  delayMicroseconds(pulseInterval / 2);
}

// 执行弓形运动的函数
void startArcMovement() {
  int xPosition = 0;  // X轴的位置

  // 循环运动直到X轴达到最大位置（假设最大为X轴的移动次数）
  while (xPosition < 50) { 

    // X轴向右移动
    moveX(x_move_distance);
    xPosition++; 
    // Y轴向上移动
    moveY(y_move_distance);
    delayMicroseconds(20500);
    // X轴再向右移动
    moveX(-x_move_distance);
    // delay(100);
    xPosition++;   
    moveY(y_move_distance);  // 负值表示向上
    delayMicroseconds(20500);
    // delay(100);


  }

  resetYNPosition(xPosition * y_move_distance); // 回原位
}

// 控制X轴的运动
void moveX(double distance) {
  if(distance < 0){
    distance = -1 * distance;
    for (double i = 0; i < distance; i++) {
      controlStepperSpeed(SPEED_PUL_X_PIN, DIRECTION_X_PIN, stepperSpeed, 0); 
    }
  }
  else{
    for (double i = 0; i < distance; i++) {
      controlStepperSpeed(SPEED_PUL_X_PIN, DIRECTION_X_PIN, stepperSpeed, 1);
    }
  }
  Serial.println("X moved by " + String(distance));
}

// 控制Y轴的运动
void moveY(double distance) {
  if(distance < 0){
    distance = -1 * distance;
    for (double i = 0; i < distance; i++) {
      controlStepperSpeed(SPEED_PUL_Y_PIN, DIRECTION_Y_PIN, stepperSpeed, 0);
    }
  }
  else{
    for (double i = 0; i < distance; i++) {
      controlStepperSpeed(SPEED_PUL_Y_PIN, DIRECTION_Y_PIN, stepperSpeed, 1);
    }
  }
  Serial.println("Y moved by " + String(distance));
}

// 复位X轴
void resetXPosition(double distance) {
  Serial.println("Resetting X position...");
  // X轴复位操作
  for (double i = 0; i < distance; i++) {
    controlStepperSpeed(SPEED_PUL_X_PIN, DIRECTION_X_PIN, stepperSpeed, 0);
  }
  Serial.println("X position reset complete.");
}

void resetXNPosition(double distance) {
  Serial.println("Resetting X position...");
  // X轴复位操作
  for (double i = 0; i < distance; i++) {
    controlStepperSpeed(SPEED_PUL_X_PIN, DIRECTION_X_PIN, stepperSpeed, 1);
  }
  Serial.println("X position reset complete.");
}

// 复位Y轴
void resetYPosition(double distance) {
  Serial.println("Resetting Y position...");
  // Y轴复位操作
  for (double i = 0; i < distance; i++) {
    controlStepperSpeed(SPEED_PUL_Y_PIN, DIRECTION_Y_PIN, stepperSpeed, 1);
  }
  Serial.println("Y position reset complete.");
}

void resetYNPosition(double distance) {
  Serial.println("Resetting Y position...");
  // Y轴复位操作
  for (double i = 0; i < distance; i++) {
    controlStepperSpeed(SPEED_PUL_Y_PIN, DIRECTION_Y_PIN, stepperSpeed, 0);
  }
  Serial.println("Y position reset complete.");
}
