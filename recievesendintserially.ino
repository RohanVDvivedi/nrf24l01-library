void setup()
{
  Serial.begin(9600);
}

//this is a function to send a 32 bit unsigned integer in binary over serial
//
void send_serial_long(unsigned long i)
{
       
        unsigned char uc[4];
        uc[3]=i;
        i=i>>8;
        uc[2]=i;
        i=i>>8;
        uc[1]=i;
        i=i>>8;
        uc[0]=i;
        Serial.write(uc,4);
  }
        
        

// this function to receive 4 byte unsigned integer serially
unsigned long recieve_serial_long()
{
  unsigned long i=0;
  unsigned char uc;
  while(1)
  {
    if(Serial.available()==4)
    {
      uc=Serial.read();
      i=i+((unsigned long)uc)<<24;
      uc=Serial.read();
      i=i+(((unsigned long)uc)<<16);
      uc=Serial.read();
      i=i+(((unsigned long)uc)<<8);
      uc=Serial.read();
      i=i+(unsigned long)uc;
     
      
      break;
    }
  }
  return i;
}

void loop()
{
}
