#include <Arduino.h>
#include <Servo.h>
#define base_FW 28
#define base_BW 22
#define la_FW 29
#define la_BW 23
#define ua_FW 30
#define ua_BW 24
#define w_FW 31
#define w_BW 25
#define iee_FW 32
#define iee_BW 26
#define fee_FW 33
#define fee_BW 27

#define ADD 34
#define EXEC 35
#define HOME 36
#define EXEC_R 37
#define ERASE 38

#define STATE 12

Servo base;
Servo lower_arm;
Servo upper_arm;
Servo wrist;
Servo initial_endeffector;
Servo final_endeffector;

int instruction_count=0,add_count=0;
bool add=false;
struct position{
  byte base;
  byte lower_arm;
  byte upper_arm;
  byte wrist;
  byte initial_endeffector;
  byte final_endeffector;
};



struct position home;
struct position targets[500];




void pos_servos(struct position *p){
  base.write(p->base);
  lower_arm.write(p->lower_arm);
  upper_arm.write(p->upper_arm);
  wrist.write(p->wrist);
  initial_endeffector.write(p->initial_endeffector);
  final_endeffector.write(p->final_endeffector);
  delay(400);
}


void move_servo(char dir,Servo *s){
  int pos=s->read();
  if(dir=='F'){
    if(pos+1<=100)
    s->write(pos+1);
    else
    s->write(100);
  }
  if(dir=='B'){
    if(pos-1>=03)
    s->write(pos-1);
    else
    s->write(03);
  }
  // Serial.println(pos);
  delay(10);
}

void add_instruction(){
  targets[instruction_count].base=base.read();
  targets[instruction_count].lower_arm=lower_arm.read();
  targets[instruction_count].upper_arm=upper_arm.read();
  targets[instruction_count].wrist=wrist.read();
  targets[instruction_count].initial_endeffector=initial_endeffector.read();
  targets[instruction_count].final_endeffector=final_endeffector.read();
  Serial.print("Added Instruction:");
  Serial.println(instruction_count++);
  delay(500);
}

void execute_instructions(){
  for(int i=0;i<instruction_count;i++){
    pos_servos(&targets[i]);
    delay(100);
    Serial.println("Executed Instruction");
  }
  pos_servos(&home);
}

void erase(){
  instruction_count=0;
  pos_servos(&home);
  Serial.println("Erased Instruction");
}

void Manual(){
  // Serial.println("Manual");
  if(digitalRead(base_FW)){move_servo('F', &base);Serial.println("BASE-F");}
  if(digitalRead(base_BW)){move_servo('B', &base);Serial.println("BASE-B");}
  if(digitalRead(la_FW)){move_servo('F', &lower_arm);Serial.println("L1-F");}
  if(digitalRead(la_BW)){move_servo('B', &lower_arm);Serial.println("L1-B");}
  if(digitalRead(ua_FW)){move_servo('F', &upper_arm);Serial.println("L2-F");}
  if(digitalRead(ua_BW)){move_servo('B', &upper_arm);Serial.println("L2-B");}
  if(digitalRead(w_FW)){move_servo('F', &wrist);Serial.println("L3-F");}
  if(digitalRead(w_BW)){move_servo('B', &wrist);Serial.println("L3-B");}
  if(digitalRead(iee_FW)){move_servo('F', &initial_endeffector);Serial.println("L4-F");}
  if(digitalRead(iee_BW)){move_servo('B', &initial_endeffector);Serial.println("L4-B");}
  if(digitalRead(fee_FW)){move_servo('F', &final_endeffector);Serial.println("L5-F");}
  if(digitalRead(fee_BW)){move_servo('B', &final_endeffector);Serial.println("L5-B");}

}

void setup()
{
  home.base = 70;
  home.lower_arm=30;
  home.upper_arm=140;
  home.wrist=90;
  home.initial_endeffector=90;
  home.final_endeffector=90;
  targets[0]=home;

  pinMode(base_FW,INPUT);
  pinMode(base_BW,INPUT);
  pinMode(la_FW,INPUT);
  pinMode(la_BW,INPUT);
  pinMode(ua_FW,INPUT);
  pinMode(ua_BW,INPUT);
  pinMode(w_FW,INPUT);
  pinMode(w_BW,INPUT);
  pinMode(iee_FW,INPUT);
  pinMode(iee_BW,INPUT);
  pinMode(fee_FW,INPUT);
  pinMode(fee_BW,INPUT);
  pinMode(ADD,INPUT);
  pinMode(EXEC,INPUT);
  pinMode(HOME,INPUT);
  pinMode(ERASE,INPUT);

  pinMode(STATE, OUTPUT);



  base.attach(2);
  lower_arm.attach(3);
  upper_arm.attach(4);
  wrist.attach(5);
  initial_endeffector.attach(6);
  final_endeffector.attach(7);

  pos_servos(&home);

  Serial.begin(9600);
  Serial.println("Home Position");

  digitalWrite(STATE, HIGH);
  delay(1000);
  digitalWrite(STATE, LOW);


}


void loop() {
  bool exec_loop=false;
  if(digitalRead(39)){
    digitalWrite(STATE, HIGH);
    Manual();
  }
  else{
    digitalWrite(STATE, LOW);
    if(digitalRead(HOME)){
      Serial.println("HOME");
      pos_servos(&home);}
    else if(digitalRead(ERASE))erase();
    else if(digitalRead(EXEC))execute_instructions();
    else if(digitalRead(EXEC_R)){
      exec_loop=true;
      Serial.println("EXEC_LOOP");
      while(exec_loop){
        execute_instructions();
        delay(500);
        exec_loop=(digitalRead(EXEC_R)&& exec_loop)? false:true;
      }
    }
  }

  if((digitalRead(ADD)) && !add){
    add_instruction();
    add=true;
  }
  if(add)add_count++;
  if(add_count>=1000)add=false;
}
