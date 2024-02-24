#include <stdio.h>
#include <stdlib.h>

int main(){
	int data_len = 240*320*3;
	int out_len = 240*320*2+1;
	int out0_len = 19*16*256;
	int out1_len = out_len - out0_len;


	FILE *fileH0 = fopen("entry0.h", "w");
	if(fileH0==0){printf("Dateifehler h0\n");return 0;}
	fprintf(fileH0, "#define ENTRY0_LEN (%d)\n",out0_len);
	fprintf(fileH0, "static uint8_t ENTRY0[ENTRY0_LEN] ={\n");
	fprintf(fileH0, "0X2C,\n");

	FILE *fileH1 = fopen("entry1.h", "w");
	if(fileH1==0){printf("Dateifehler h1\n");return 0;}
	fprintf(fileH1, "#define ENTRY1_LEN (%d)\n",out1_len);
	fprintf(fileH1, "static uint8_t ENTRY1[ENTRY1_LEN] ={\n");

	//5r6g5b pixelformat (3Ah =03h)
	char *filename = "entry.data";
	FILE *fileData = fopen(filename, "rb");
	if(fileData==0){printf("Dateifehler d\n");return 0;}
	unsigned char data[data_len];
	fread(data, 1, data_len, fileData);

	//read 3 bytes and write 1 word
	int data_count = 0;
	for(int m=0; m<data_len-2; m=m+3){
		if(data_count < out0_len){
			if(m%4==0&&m>0)fprintf(fileH0, " ");
			if(m%24==0&&m>0)fprintf(fileH0, "\n");
		}
		else{
			if(m%4==0&&m>0)fprintf(fileH1, " ");
			if(m%24==0&&m>0)fprintf(fileH1, "\n");
		}
		//word 0x0000;//RRRR RGGG GGGB BBBB
		u_int16_t blue = (data[m+2] >>3) & 0x1F;
		u_int16_t green = ((data[m+1] >>2) & 0x3F)<<5;
		u_int16_t red = ((data[m] >>3) & 0x1F)<<11;
		u_int16_t sum = red | green | blue;
		if(data_count < out0_len - 2)
			fprintf(fileH0, "0x%02X,0x%02X,", (sum&0xFF00)>>8, (sum&0x00FF));
		else if(data_count == out0_len - 2){
			fprintf(fileH0, "0x%02X,", (sum&0xFF00)>>8);
			fprintf(fileH1, "0x%02X,", (sum&0x00FF));
		}
		else
			fprintf(fileH1, "0x%02X,0x%02X,", (sum&0xFF00)>>8, (sum&0x00FF));
		data_count += 2;
	}
	fprintf(fileH0, "};\n");
	fprintf(fileH1, "};\n");
	fclose(fileH0);
	fclose(fileH1);
	fclose(fileData);

}
