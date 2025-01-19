#include "mbed.h"
#include "LITE_CDH.h"
#include "LITE_EPS.h"
#include "LITE_SENSOR.h"
#include "LITE_COM.h"
LITE_CDH cdh(PB_5, PB_4, PB_3, PA_8, "sd", PA_3);
LITE_EPS eps(PA_0,PA_4);
LITE_SENSOR sensor(PA_7,PB_7,PB_6);
LITE_COM com(PA_9,PA_10,9600);
DigitalOut cond[]={PB_1, PA_5};
RawSerial sat(USBTX,USBRX,9600);
Timer sattime;
int rcmd = 0,cmdflag = 0; //command variable

int main() {
    sat.printf("From Sat : Nominal Operation\r\n");
    com.printf("From Sat : Nominal Operation\r\n");
    int flag = 0; //condition flag
    float batvol, temp; //voltage, temperature 
    int rcmd=0,cmdflag=0;  //command variable
    sattime.start();
    eps.turn_on_regulator();//turn on 3.3V conveter
    cdh.turn_on_analogSW();//turn on transceiver
    for(int i = 0; i < 100; i++) {
        com.xbee_receive(&rcmd,&cmdflag);//interupting by ground station command
        
        //satellite condition led
        cond[0] = !cond[0];
        
        //senssing HK data(dummy data)
        eps.vol(&batvol);
        sensor.temp_sense(&temp);
        
        //Transmitting HK data to Ground Station(GS)
        com.printf("HEPTASAT::Condition = %d, Time = %f [s], batVol = %.2f [V],Temp = %.2f [C]\r\n",flag,sattime.read(),batvol,temp);
        wait_ms(1000);
                
        //Power Saving Mode 
        if((batvol <= 3.5)  | (temp > 35.0)){
            eps.shut_down_regulator();
            com.printf("Power saving mode ON\r\n"); 
            flag = 1;
        } else if((flag == 1) & (batvol > 3.7) & (temp <= 25.0)) {
            eps.turn_on_regulator();
            com.printf("Power saving mode OFF\r\n");
            flag = 0;
        }
        //Contents of command
        if (cmdflag == 1) {
            if (rcmd == 'a') {
                sat.printf("rcmd=%c,cmdflag=%d\r\n",rcmd,cmdflag);
                com.printf("Hepta-Sat Lite Uplink Ok\r\n");
                for(int j=0;j<5;j++){
                    com.printf("Hello World!\r\n");
                    cond[0] = 1;
                    wait_ms(1000);
                }
            }else if (rcmd == 'b') {
                sat.printf("rcmd=%c,cmdflag=%d\r\n",rcmd,cmdflag);
                com.printf("Hepta-Sat Lite Uplink Ok\r\n");
                wait(1);
                char str[100];
                mkdir("/sd/mydir", 0777);
                FILE *fp = fopen("/sd/mydir/test.txt","w");
                if(fp == NULL) {
                    error("Could not open file for write\r\n");
                }
                for(int i = 0; i < 10; i++) {
                    eps.vol(&batvol);
                    fprintf(fp,"%f\r\n",batvol);
                }
                fclose(fp);
                fp = fopen("/sd/mydir/test.txt","r");
                for(int j = 0; j < 10; j++) {
                    fgets(str,100,fp);
                    com.puts(str);
                }
                fclose(fp);                
            }else if (rcmd == 'c') {        
                sat.printf("rcmd=%c,cmdflag=%d\r\n",rcmd,cmdflag);
                com.printf("Hepta-Sat Lite Uplink Ok\r\n");
                float ax,ay,az;
                sensor.set_up();
                for(int i = 0; i<10; i++){
                    sensor.sen_acc(&ax,&ay,&az);
                    com.printf("acc : %f,%f,%f\r\n",ax,ay,az);
                    wait_ms(1000);
                }
            }else if (rcmd == 'd') {
                sat.printf("rcmd=%c,cmdflag=%d\r\n",rcmd,cmdflag);
                com.printf("Hepta-Sat Lite Uplink Ok\r\n");
                float gx,gy,gz;
                sensor.set_up();
                for(int i = 0; i<10; i++) {
                    sensor.sen_gyro(&gx,&gy,&gz);
                    com.printf("gyro: %f,%f,%f\r\n",gx,gy,gz);
                    wait_ms(1000);
                }
            }else if (rcmd == 'e') {
                //Please insert your answer

            }
            com.initialize();
        }
    }
    sattime.stop();
    sat.printf("From Sat : End of operation\r\n");
    com.printf("From Sat : End of operation\r\n");
}