int b=10;
int a=0; int v=0;
int time1;
int time2;
int q;



void setup() {

pinMode(b, INPUT); digitalWrite(b, HIGH);
Serial.begin(9600); 

}

void loop () 
{

a=digitalRead(b);
if (a==1)
{
while (a==1){
  a=digitalRead(b);
  v=v+1;}
}



Serial.println (v);
delay(1000);
v=0;
}
