#include "simpletools.h"
#include "servo.h"

//for distance
void calc_dist();
volatile float distance;
int trig=6,echo=7;
float stack1[150];

//for color
void detect_color();
int detect(float,float,float);
char col();
char colors[40];
float wRe = -5.7046146,wRr =-52.360493, wRg =5.0879936 , wRb = 14.590164;
float wGe=36.54086 ,wGr = 24.84385 , wGg =-32.582184, wGb =-1.1889476 ;
float wBe = 14.040528,wBr =2.0341995, wBg = 28.822338, wBb = -49.0948;
float be=-28.374115 ,br=22.197927 ,bg=0.24372701 ,bb=14.312374,R,G,B;
 int s0=0,s1=1,s2=2,s3=3,out=4,led=5,nxt_idx;
 float stack2[150]; 

//for manipulator
void pick_n_place();
void move();
 int a[4]={0,900,1800,600}, p[4]={14,15,16,17}, nxt_box; 
 float stack3[150];

//for conveyor
void travel();
int steppin=11,dirpin=12,m0pin=8,m1pin=9,m2pin=10,stepdelay,runmotor=0;
 float stack4[150];
 
 //for conveyor timing
 void track();
 unsigned int conveyor_dist;
unsigned int stack5[100];

int main(){
  distance=15;
  conveyor_dist=0;
  for(int i=0;i<40;i++){
    colors[i]='n';
  }    
  
  cogstart(&calc_dist,NULL,stack1,sizeof(stack1));
  pause(2);
  cogstart(&detect_color,NULL,stack2,sizeof(stack2));
  pause(2);
 cogstart(&pick_n_place,NULL,stack3,sizeof(stack3));
  pause(2);
  cogstart(&travel,NULL,stack4,sizeof(stack4));
  pause(2);
  cogstart(&track,NULL,stack5,sizeof(stack5));
  pause(2);

}  

//for distance measurement
void calc_dist(){
  float dist=0;
  while(1){
    dist=0;
    for(int i=0;i<3;i++){
      high(trig);
      pause(5);
      low(trig);
      dist+=(pulse_in(echo,1))/70;
      pause(5);
    }  
    distance = dist/3;    
  }    
}  


//FOR COLOR DETECTION
void detect_color(){
char pc, cc;
unsigned int tstart;
nxt_idx=0; //index of the colour to be added

 //Calling functions
  high(s0);
  high(s1);
  pc='n';
  while(1)
  {
    cc=col();
    if(cc!='n'){
      if(pc!=cc){
        colors[nxt_idx]=cc;
        nxt_idx+=1;
        tstart=conveyor_dist;
      }else if(conveyor_dist - tstart>1200){
        colors[nxt_idx]=cc;
        nxt_idx+=1;
        tstart=conveyor_dist;
      }                       
    }     
    pc=cc; 
  }    
}

char col(){
  int nn=0,nr=0,ng=0,nb=0;
  char c='n';
  for(int i=0;i<3;i++){  
   //RED 
   low(s2);
  low(s3);
  R=pulse_in(out,0);
  pause(5);
  //GREEN
  high(s2);
  high(s3);
  G=pulse_in(out,0);
  pause(5);
  //BLUE
  low(s2);
  high(s3);
  B=pulse_in(out,0);
  pause(5);
  
   //Decide colour based on calibrated values
     float m3 = (R>G)?R:G;
   m3 = (m3>B)?m3:B;
   R=R/m3;
   G=G/m3;
   B=B/m3;
    switch(detect(R,G,B)){
      case 0: nn+=1;
      break;
      case 1: nr+=1;
      break;
      case 2: ng+=1;
      break;
      case 3: nb+=1;
      break;
      default: break;
    }      
  }
  if(nr>=2){
    c='r';
    }else if(ng>=2){
      c='g';
    }else if(nb>=2){
      c='b';
    }else{c='n';
    }      
  return c;  }  


int detect( float r, float g, float b){
float ee = exp(wRe*r+wGe*g+wBe*b+be);
float er =exp(wRr*r+wGr*g+wBr*b+br);
float eg =exp(wRg*r+wGg*g+wBg*b+bg);
float eb = exp(wRb*r+wGb*g+wBb*b+bb);
float m1= (ee>er)?ee:er;
float m2=(eg>eb)?eg:eb;
float m = (m1>m2)?m1:m2;
if(m==er){
  return 1;
}else if(m==eg){
  return 2;
}else if(m==eb){
  return 3;
}else{
  return 0;
}  
}


//FOR MANIPULATOR
void pick_n_place(){
  int change;
  nxt_box=0; 
  
  //manipulator initial position
    for(int i=0;i<=3;i++){
    servo_angle(p[i],a[i]);
  }   
  
 while(1){
  if(distance<=4){
  switch(colors[nxt_box]){
  case 'r': change=700;
  break;
  case 'g': change=1200;
  break;
  case 'b': change=1500;
  break;
  default: break;
  }
   
   if(colors[nxt_box]!='n'){
   //bend to pick
   move(0,0);
   pause(20);
   move(1,520);
   pause(20);
   move(2,1800);
   pause(20);
   move(3,0);
    pause(20);
    
    //rise
    move(1,900);
   pause(20);
   
   //turn
   move(0,change);
   pause(20);
   
   //bend to place
   move(2,1500);
   pause(20);
   move(1,200);
   pause(20);
   
   
   //release
   move(3,600);
   pause(20);
   
   //rise back
   move(1,900);
   pause(20);
   
   //go to default
   move(0,0);
   pause(20);
   move(1,900);
   pause(20);
   move(2,1800);
   pause(20);
   move(3,600);
   pause(20);
   
    nxt_box+=1;
  }    
}   
}
}  

void move(int s, int ang){
  int inc=10;
  if(ang<a[s]){
      inc=-10;
    }
    while(a[s]!=ang){
      a[s]+=inc;
      servo_angle(p[s],a[s]);
      pause(1);
    }      
} 


//FOR CONVEYOR ACTUATION
void travel(){
  low(m0pin);
  high(m1pin);
  low(m2pin);
  high(dirpin);     
  stepdelay = 1;
  while(1){
    if(distance>=3.5){ runmotor=1;
    }      
    while(runmotor){
      high(steppin);
      pause(stepdelay);
      low(steppin);
      pause(stepdelay);
      if(distance<3.5){runmotor=0;
      }           
    }      
  }    
}  


//FOR "TRACKING " BOX ON CONVEYOR
void track(){
  while(1){
    if(distance>4){
      conveyor_dist+=1;
    }     
    pause(1); 
  }    
}  
 