#include <SPI.h>

#define CE 9
#define SS 10
#define IRQ 2  // defining pin connections

/*
  SPI pin connections on arduino
  MISO-12
  MOSI-11
  SCK-13
*/


int DATA_RECEIVED_FLAG =0;

void data_received()
{
  DATA_RECEIVED_FLAG =1;
}

void setup()
{
  SPI.begin();
  Serial.begin(9600);
  pinMode(SS,OUTPUT);
  pinMode(CE,OUTPUT);
  pinMode(IRQ,INPUT);
  
  
  // for small code
  attachInterrupt(0,status_method,FALLING);
  // OR
  // for larger code
  attachInterrupt(0,data_received,FALLING);
}

byte recievedPAYLOAD[32];
byte transmissionPAYLOAD[32];
byte ack_pipe=0;

const byte R_REG=B00000000;
const byte W_REG=B00100000;
const byte ACTIVATE=B01010000;
const byte R_RX_PLD=B01100001;
const byte W_TX_PLD=B10100000;
const byte W_ACK_PLD=B10101000;
const byte F_RX = B11100010;
const byte F_TX = B11100001;          // Commands to be sent for setting up nrf see datasheet for more info

byte data_byte[5];

byte status_reg;

void flushrx()
{
  delay(1);
  digitalWrite(SS,LOW);
  SPI.transfer(F_RX);
  digitalWrite(SS,HIGH);
  delay(1);
}

void flushtx()
{
  delay(1);
  digitalWrite(SS,LOW);
  SPI.transfer(F_TX);
  digitalWrite(SS,HIGH);
  delay(1);
}

byte read_1_byte_reg(byte addr)
{
  byte b;
  digitalWrite(SS,LOW);
  status_reg=SPI.transfer(addr);
  b=SPI.transfer(0x00);
  digitalWrite(SS,HIGH);
  return b;
}

void read_5_byte_reg(byte addr)
{
  digitalWrite(SS,LOW);
  status_reg=SPI.transfer(addr);
  data_byte[0]=SPI.transfer(0x00);
  data_byte[1]=SPI.transfer(0x00);
  data_byte[2]=SPI.transfer(0x00);
  data_byte[3]=SPI.transfer(0x00);
  data_byte[4]=SPI.transfer(0x00);
  digitalWrite(SS,HIGH);
}

byte activate()
{
  digitalWrite(SS,LOW);
  status_reg=SPI.transfer(ACTIVATE);
  SPI.transfer(0x73);
  digitalWrite(SS,HIGH);
}

void read_setup() //  utilize this function for reading the registers in the nrf24l01 hardware
{
  byte temp;
  for(temp=0;temp<=0x17;temp++)
  {
    
    if(temp==0x0A || temp==0x0B || temp==0x10)
    {
      read_5_byte_reg(temp);
      Serial.print(temp,HEX);
      Serial.print(" -- ");
      Serial.print(data_byte[4],HEX);
      Serial.print(data_byte[3],HEX);
      Serial.print(data_byte[2],HEX);
      Serial.print(data_byte[1],HEX);
      Serial.println(data_byte[0],HEX);
      continue;
    }
    
    Serial.print(temp,HEX);
    Serial.print(" -- ");
    Serial.println(read_1_byte_reg(temp),BIN);
  }
  
  temp=0x1C;
  Serial.print(temp,HEX);
  Serial.print(" -- ");
  Serial.println(read_1_byte_reg(temp),BIN);
  
  temp=0x1D;
  Serial.print(temp,HEX);
  Serial.print(" -- ");
  Serial.println(read_1_byte_reg(temp),BIN);
}

void write_1_byte_reg(byte addr,byte data)
{
  digitalWrite(SS,LOW);
  status_reg=SPI.transfer(W_REG+addr);
  SPI.transfer(data);
  digitalWrite(SS,HIGH);
}

void write_5_byte_reg(byte addr)
{
  digitalWrite(SS,LOW);
  status_reg=SPI.transfer(W_REG+addr);
  SPI.transfer(data_byte[0]);
  SPI.transfer(data_byte[1]);
  SPI.transfer(data_byte[2]);
  SPI.transfer(data_byte[3]);
  SPI.transfer(data_byte[4]);
  digitalWrite(SS,HIGH);
}

const byte RX_MODE = B00000001;
const byte TX_MODE = B00000000;
byte MODE;

void set_rf_channel(byte channel)
{
  write_1_byte_reg(0x05,channel);
}



//utilize this function for setting up the registers
// change the values according to your requirement 
/*
  step 1: read DATA sheet completely
  step 2: check what you want ot change
  know its register hex code and the byte you want to set
  step 3: find out its register position below( arranged in increasing order)
  step 4: replace the byte value
  
  write_1_byte_reg( <address of the register> , <byte value to write> );   change the byte value to write as required
  
  or 
  
  
  [below use recomended only for pipe 0 ,pipe 1 rx addresses and tx address on device]
  change the data_byte array for changing the 5 bytes at once
  write_5_byte_reg( <address of the register>);   
*/

void init(byte mode)
{
  MODE=mode;
  write_1_byte_reg(0x00,B00001010+mode); // config register all interupts turned on
  
  write_1_byte_reg(0x01,B00111111); // auto ack enabled for all
  
  write_1_byte_reg(0x02,B00111111); // all rx addresses have been enabled
  
  write_1_byte_reg(0x03,B00000011); // address width is 5 bytes
  
  write_1_byte_reg(0x04,B00011111);  // auto re transmit deay is 0.5 ms and 15 times
  
  write_1_byte_reg(0x06,B00100110);  // 
  
  data_byte[4]=0xA1; data_byte[3]=0xA2; data_byte[2]=0xA3; data_byte[1]=0xA4; data_byte[0]=0xA5;
  write_5_byte_reg(0x0A); // pipe 0 rx addr
  write_5_byte_reg(0x10); // tx addr
  
  data_byte[4]=0xB1; data_byte[3]=0xB2; data_byte[2]=0xB3; data_byte[1]=0xB4; data_byte[0]=0xB5;
  write_5_byte_reg(0x0B); // pipe 1 rx addr
  
    write_1_byte_reg(0x0C,0xC5);  // p2
    write_1_byte_reg(0x0D,0xD5);  // p3
    write_1_byte_reg(0x0E,0xE5);  // p4
    write_1_byte_reg(0x0F,0xF5);  // p5
    
    //size of payload
    write_1_byte_reg(0x11,32);  // p0
    write_1_byte_reg(0x12,32);  // p1
    write_1_byte_reg(0x13,32);  // p2
    write_1_byte_reg(0x14,32);  // p3
    write_1_byte_reg(0x15,32);  // p4
    write_1_byte_reg(0x16,32);  // p5
    
    write_1_byte_reg(0x1C,B00111111);  //
    
    write_1_byte_reg(0x1D,B00000111);  // 
    
    activate();
    
    if(mode==RX_MODE)
    {
      digitalWrite(CE,HIGH);
    }
    else if(mode==TX_MODE)
    {
      digitalWrite(CE,LOW);
    }
  
}



// call this function everytime the data is received
void status_method()
{
  byte correction=B00000000;
  
  digitalWrite(SS,LOW);
  status_reg=SPI.transfer(B11111111);
  digitalWrite(SS,HIGH);
  
  byte pipe =  ( (status_reg & B00000010) + (status_reg & B00000100) + (status_reg & B00001000) )/2;
  
  if( (status_reg & B01000000) == B01000000 )
  {
    Serial.print("data to read is there in pipe"); 
    Serial.println(pipe,DEC);
    read_rx_payload();
    DATA_RECEIVED_FLAG=0;
    correction=B01000000;
  }
  else
  {
    Serial.println("no data to be read");
  }
  
  if( (status_reg & B00100000) == B00100000 )
  {
    Serial.println("data transmission successfull");
    correction=correction+B00100000;
  }
  else
  {
    Serial.println("unsucessfull communication");
  }
  
  if( (status_reg & B00010000) == B00010000 )
  {
    Serial.println("no ack recieved on max retransmits");
    correction=correction+B00010000;
  }
  
  write_1_byte_reg(0x07,correction);
  
}

void read_rx_payload()
{
  int temp;
  digitalWrite(SS,LOW);
  status_reg=SPI.transfer(R_RX_PLD);
  
  for(temp=0;temp<32;temp++)
  {
    recievedPAYLOAD[temp]=SPI.transfer(0x00);
  }
  
  digitalWrite(SS,HIGH);
  
  delayMicroseconds(10);
  
  flushrx();
  
  delayMicroseconds(10);
  
  receivedPAYLOADhandler(); // your function that handles received data
  
  if(MODE==RX_MODE)
  {
    digitalWrite(SS,LOW);
    status_reg=SPI.transfer(W_ACK_PLD+ack_pipe);
  
    for(temp=0;temp<32;temp++)
    {
      SPI.transfer(transmissionPAYLOAD[temp]);
    }
  
    digitalWrite(SS,HIGH);
  }
  
}

void transmit() // call this function after setting transmissionPAYLOAD array to send the corresponding 32 bytes
{
  if(MODE==TX_MODE)
  {
    flushtx();
    
    int temp;
    digitalWrite(SS,LOW);
    status_reg=SPI.transfer(W_TX_PLD);
  
    for(temp=0;temp<32;temp++)
    {
      SPI.transfer(transmissionPAYLOAD[temp]);
    }
  
    digitalWrite(SS,HIGH);
    
    digitalWrite(CE,HIGH);
    delayMicroseconds(100); // this a required delay donot remove
    digitalWrite(CE,LOW);
    
  }
}

void receivedPAYLOADhandler(){}// define this function to set what to do after data is received

void loop(){}
