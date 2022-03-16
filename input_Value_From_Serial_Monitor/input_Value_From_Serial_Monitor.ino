String input = "";  
 
void setup()   
{  
    Serial.begin(9600);  
}  
  
void loop()   
{  
    while (Serial.available() == 0)   
    { /*Wait for user input*/ }  
    input = Serial.readString(); //Reading the Input string from Serial port.
    int adc=input.toInt();  
    int jumlah=adc*100;
    Serial.println("-------------------------"); //Showing the details  
    Serial.println("YOUR NAME:" + input);
    Serial.println(jumlah);
    Serial.println("");  
    while (Serial.available() == 0) {}  
} 
