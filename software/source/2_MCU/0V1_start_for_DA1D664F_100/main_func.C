
//////////////////////////////////////////////////////////////////////
U8 test_osc=1;
//ES9018
void ess_mute(){
  if(!key_func){
    mute_enable=!mute_enable;	//0 = mute, 	1 = unmute
    es9018_reg10=(es9018_reg10&0xce) + (!mute_enable);			//0xcf = mute, 			0xce = unmute
    I2C_Write(0x90, 0x0a, es9018_reg10);
    I2C_Write(0x92, 0x0a, es9018_reg10);
    dot_vol_hextodeci(vol_dB);
  }
 /*
 test_osc=!test_osc;
 osc80_enable=test_osc;
 
 if(test_osc) I2C_Write(0x20, 0x04, interface_ch);
 else I2C_Write(0x20, 0x04, 0xb8);
 */
}

//////////////////////////////////////////////////////////////////////
void phase_ess(){
  
  if(!phase_data) { //phase on
    I2C_Write(0x90, 13, 0xff);
    I2C_Write(0x92, 13, 0xff);
  }
  else{ //phase off
    I2C_Write(0x90, 13, 0);
    I2C_Write(0x92, 13, 0);
  }
  rom_write_multi();
}

//////////////////////////////////////////////////////////////////////

void volume_set(){		//I2C write, dot-matrix
  //ES9018, i2c write
  U8 i;
  for(i=0; i<8; i++){
    I2C_Write(0x90,i,vol_dB);	//Lch volume of DAC0
    I2C_Write(0x92,i,vol_dB);	//Rch volume of DAC0
  }
  rom_write_multi();
 
}

void init_vol_dn(U8 data){
  //gain
  I2C_Write(0x90, 23, 0x7f-ess_lch_master_trim);
  I2C_Write(0x92, 23, 0x7f-ess_rch_master_trim);
  //
  
  
  I2C_Write(0x90, 20, 0xfe);
  I2C_Write(0x92, 20, 0xfe);
  /*U8 i;
  if(data<199) data++;
  else           data=0xff;		//-127dB, display num = 00.0
  for(i=0; i<8; i++){
    I2C_Write(0x90,i,data);	//Lch volume of DAC0
    I2C_Write(0x92,i,data);	//Rch volume of DAC0
  }
 */
}

void init_vol(U8 data){
  I2C_Write(0x90, 20, 0xff);
  I2C_Write(0x92, 20, 0xff);
  /*
  U8 i;
  for(i=0; i<8; i++){
    I2C_Write(0x90,i,data);	//Lch volume of DAC0
    I2C_Write(0x92,i,data);	//Rch volume of DAC0
  }
*/
}


void audio_level_sp_down(){
  //display num = 100 - (reg/2)
  //audio_level++;	//reg(#0~#7)
  U8 temp;
  if(!key_func){
    temp=vol_dB;
  
    if(vol_dB<197) vol_dB+=3;
    else               vol_dB=0xff;		//-127dB, display num = 00.0
  
    if(temp!=vol_dB) {
      volume_set();
      dot_vol_hextodeci(vol_dB);
    }
  }
}

void audio_level_sp_up(){
  //display num = 100 - (reg/2)
  //audio_level--;	//reg(#0~#7)
  U8 temp;
  if(!key_func){
    temp=vol_dB;
  
    if(vol_dB==0xff) vol_dB=199;
    else if(vol_dB>2) vol_dB-=3;
    else vol_dB=0;
  
    if(temp!=vol_dB) {
      volume_set();
      dot_vol_hextodeci(vol_dB);
    }
  }
}


void audio_level_down(){
  //display num = 100 - (reg/2)
  //audio_level++;	//reg(#0~#7)
  U8 temp;
  if(!key_func){
    if(!mute_enable) ess_mute();    //mute condition
    temp=vol_dB;
  
    if(vol_dB<199) vol_dB++;
    else               vol_dB=0xff;		//-127dB, display num = 00.0
  
    if(temp!=vol_dB) {
      volume_set();
      dot_vol_hextodeci(vol_dB);
    }
  }
}

void audio_level_up(){
  //display num = 100 - (reg/2)
  //audio_level--;	//reg(#0~#7)
  U8 temp;
  if(!key_func){
    if(!mute_enable) ess_mute();    //mute condition
    temp=vol_dB;
  
    if(vol_dB==0xff) vol_dB=199;
    else if(vol_dB>0) vol_dB--;
  
    if(temp!=vol_dB) {
      volume_set();
      dot_vol_hextodeci(vol_dB);
    }
  }
}

void channel_change(){
  U8 temp;
  
  if(mute_enable) {			//0 = mute, 	1 = unmute
      I2C_Write(0x90, 0x0a, 0xcf);    //mute
      I2C_Write(0x92, 0x0a, 0xcf);    //mute
    }
  
  if(ch_led_data==7) temp=0;    //USB mode     : I2S_SEL=0;
  else temp=1;                       //CS8416 mode : I2S_SEL=1;
    
  if(temp!=i2s_flag){
    i2s_flag=temp;
    I2S_SEL=i2s_flag;
  }
  
  //ch_led_data : 0=coax1,  1=coax2,    2=aes1,   3=aes2,   4=bnc,  5=opt1,   6=opt2,   7=usb
  interface_ch = interface_ch&0xc7;		// D5~D3 = 0
  interface_ch+=(ch_led_data<<3);
  I2C_Write(0x20, 0x04, interface_ch);
  
  if(mute_enable) {			//0 = mute, 	1 = unmute
      I2C_Write(0x90, 0x0a, 0xce);    //unmute
      I2C_Write(0x92, 0x0a, 0xce);    //unmute
    }
  
  rom_write_multi();

  //dot_vol_hextodeci(vol_dB);
}

void channel_up(){
  //channel_vol_save();
  if(!key_func){
    if(ch_led_data<7) ch_led_data++;
    else ch_led_data=0;
    channel_change();
  }
}

void channel_down(){
  //channel_vol_save();
  if(!key_func){
    if(ch_led_data) ch_led_data--;
    else ch_led_data=7;
    channel_change();
  }
}

void ess_filter(){
  S32 coeff;
  U8 i,c,num; 
  
  rom_save_flag2=0;
  EIFR =  (0<<INTF3);   //INTF3 disable
  if(filter_flag==3) filter_flag=0;
  num=filter_flag; 
  
  rom_write_multi();
  DelayTime_ms(5);  //5msec  
  
/* 12-07-24 change 
  if(num==0) {   
    I2C_Write(0x90, 37, 0x00);
    I2C_Write(0x92, 37, 0x00);    
    I2C_Write(0x90, 14, 0x0b);
    I2C_Write(0x92, 14, 0x0b);
  }
  else if (num==1 || num==2) {
  
    I2C_Write(0x90, 37, 0x10); // enable stage 1 programming
    I2C_Write(0x92, 37, 0x10); // enable stage 1 programming
    for ( i = 0; i < 64; i++) // for all 64 stage 1 coefficients
    {
    
      if(num==1) coeff = f1_coeff_st1[i]; // get the stage1 coefficient
      else if(num==2) coeff = f2_coeff_st1[i]; // get the stage1 coefficient
      for (c = 0; c < 8; c++) // for all 8 channels
      {
        I2C_Write(0x90, 41, ((coeff >> 24) & 0xff));
        I2C_Write(0x92, 41, ((coeff >> 24) & 0xff));
        I2C_Write(0x90, 40, ((coeff >> 16) & 0xff));
        I2C_Write(0x92, 40, ((coeff >> 16) & 0xff));
        I2C_Write(0x90, 39, ((coeff >> 8) & 0xff));
        I2C_Write(0x92, 39, ((coeff >> 8) & 0xff));
        I2C_Write(0x90, 38, ((coeff) & 0xff));
        I2C_Write(0x92, 38, ((coeff) & 0xff));
      }
    }
    I2C_Write(0x90, 38, 0x00); // clear the stage 1 coefficient pipe
    I2C_Write(0x92, 38, 0x00); // clear the stage 1 coefficient pipe
    I2C_Write(0x90, 37, 0x00); // disable stage 1 programming
    I2C_Write(0x92, 37, 0x00); // disable stage 1 programming
    
    I2C_Write(0x90, 37, 0x01); // enable stage 2 programming
    I2C_Write(0x92, 37, 0x01); // enable stage 2 programming
    for (i = 0; i < 16; i++) // for all 16 stage 2 coefficients
    {
      if(num==1) coeff = f1_coeff_st2[i]; // get the stage2 coefficient
      else if(num==2) coeff = f2_coeff_st2[i]; // get the stage1 coefficient
      for (c = 0; c < 8; c++) // for all 8 channels
      {
        I2C_Write(0x90, 45, ((coeff >> 24) & 0xff));
        I2C_Write(0x92, 45, ((coeff >> 24) & 0xff));
        I2C_Write(0x90, 44, ((coeff >> 16) & 0xff));
        I2C_Write(0x92, 44, ((coeff >> 16) & 0xff));
        I2C_Write(0x90, 43, ((coeff >> 8) & 0xff));
        I2C_Write(0x92, 43, ((coeff >> 8) & 0xff));
        I2C_Write(0x90, 42, ((coeff) & 0xff));
        I2C_Write(0x92, 42, ((coeff) & 0xff));
      }
    }
    I2C_Write(0x90, 42, 0x00); // clear the stage 2 coefficient pipe
    I2C_Write(0x92, 42, 0x00); // clear the stage 2 coefficient pipe
    I2C_Write(0x90, 37, 0x00); // disable stage 2 programming
    I2C_Write(0x92, 37, 0x00); // disable stage 2 programming
    
    I2C_Write(0x90, 37, 0x22);
    I2C_Write(0x92, 37, 0x22);
    if(num==1)  { I2C_Write(0x90, 14, 0x0a);    I2C_Write(0x92, 14, 0x0c);}
    else if(num==2)  { I2C_Write(0x90, 14, 0x0a);    I2C_Write(0x92, 14, 0x0e);}

  }  */
  
  if(num==0){
    I2C_Write(0x90, 14, 0x0b);
    I2C_Write(0x92, 14, 0x0b);
  }
  else if(num==1){
    I2C_Write(0x90, 14, 0x0d);
    I2C_Write(0x92, 14, 0x0d);
  }
  else if(num==2){
    I2C_Write(0x90, 14, 0x0f);
    I2C_Write(0x92, 14, 0x0f);
  }
  
  if(num) DelayTime_ms(200);  //0.2sec
  
  rom_save_flag2=1;
  rom_tmr=0;
  
  EIFR =  (1<<INTF3);   //INTF3 enable
  
}


void femto_function(){
  U8 i;
  
  if(key_func==1 || key_func==2)  key_condition=1;  //inverse or filter
  
  else if(key_func==3){ //change inverse
    phase_data=!phase_data;
    phase_ess();
  }
  else if(key_func==4){ //change filter
    filter_flag++;
    
    if(mute_enable) {			//0 = mute, 	1 = unmute
      I2C_Write(0x90, 0x0a, 0xcf);    //mute
      I2C_Write(0x92, 0x0a, 0xcf);    //mute
    }
    
    ess_filter();
    volume_set();
    DelayTime_ms(5);  //5msec
    
    
    if(mute_enable) {			//0 = mute, 	1 = unmute
      I2C_Write(0x90, 0x0a, 0xce);    //unmute
      I2C_Write(0x92, 0x0a, 0xce);    //unmute
    }
    //rom_write_multi();
    //DelayTime_ms(10);  //10msec
  }
  
  for(i=0; i<16; i++) {
    if(i<8) dot_string[i]=filter_name[filter_flag][i];
    else {
      if(!phase_data) dot_string[i]=phase_name[i-8];  //phase on
      else dot_string[i]=normal_name[i-8];   //phase off
    }
  }
  
  key_func_tmr=0;
  dot_string_digit();
}



/*
void ex_eeprom_save(){
  //U16 i;
  if(rom_data_cnt==0xff){
    rom_data_cnt=0;
    
    if(rom_address==0x7fc){
      rom_address=0x10;
      
      if(rom_add_pt==0x0d){
        rom_cycle++;
        rom_add_pt=0x03;
        //馆汗冉荐客 矫累林家客 单捞磐林家 历厘
        rom_I2C_Write(0,rom_add_pt); //start address
        rom_I2C_Write(1,((rom_cycle>>8)&0x00ff)); 
        rom_I2C_Write(2,(rom_cycle&0x00ff)); //address cycle(2bytes)
        rom_I2C_Write(rom_add_pt,((rom_address>>8)&0x00ff));
        rom_I2C_Write(rom_add_pt+1,(rom_address&0x00ff)); //data address(2bytes)
      }
      else{
        rom_add_pt+=2;
        //矫累林家客 单捞磐林家 历厘
        rom_I2C_Write(0,rom_add_pt); //start address
        rom_I2C_Write(rom_add_pt,((rom_address>>8)&0x00ff));
        rom_I2C_Write(rom_add_pt+1,(rom_address&0x00ff)); //data address(2bytes)
      }
    }
    
    else{
      rom_address+=3;
      //单捞磐林家 历厘
      rom_I2C_Write(rom_add_pt,((rom_address>>8)&0x00ff));
      rom_I2C_Write(rom_add_pt+1,(rom_address&0x00ff)); //data address(2bytes)
    }
  }
  else{
    rom_data_cnt++;
  }
  //单捞磐客 墨款飘 历厘  
  rom_I2C_Write(rom_address,rom_data_cnt); //data counter
  rom_I2C_Write(rom_address+1,((rom_data>>8)&0x00ff));
  rom_I2C_Write(rom_address+2,(rom_data&0x00ff));//2bytes, vol(1byte)+ch(4bits)+filter(3bits)+inverse(1bit)
  
  //test check
  //for(i=0; i<0x20; i++)     test_eeprom[i]=rom_I2C_Read(i);
  //for(i=0; i<0x20; i++)     test_eeprom[i+0x20]=rom_I2C_Read(i+0x7e0);
}
*/




/*
void ess_filter_read(){
	S32 coeff;
	U8 i,c;
	
	I2C_Write(0x90, 37, 0x10); // enable stage 1 programming
	for ( i = 0; i < 64; i++) // for all 64 stage 1 coefficients
	{
		coeff = stage1[i]; // get the stage1 coefficient
		for (c = 0; c < 8; c++) // for all 8 channels
		{
			I2C_Write(0x90, 41, ((coeff >> 24) & 0xff));
			I2C_Write(0x90, 40, ((coeff >> 16) & 0xff));
			I2C_Write(0x90, 39, ((coeff >> 8) & 0xff));
			I2C_Write(0x90, 38, ((coeff) & 0xff));
		}
	}
	I2C_Write(0x90, 38, 0x00); // clear the stage 1 coefficient pipe
	I2C_Write(0x90, 37, 0x00); // disable stage 1 programming
}
*/
/*
void audio_level_down(){
	U8 data0,data1,data2,data3;
	if(display_num){
		display_num--;
		
		if(display_num>45)			audio_level-=0xF4240;
		else if(display_num>4)		audio_level-=0x989680;
		else if(display_num>0)		audio_level-=0x5F5E100;
		else 	if(display_num==0)	audio_level=0;
		
		data3 = (audio_level>>24) & 0xff;
		data2 = (audio_level>>16) & 0xff;
		data1 = (audio_level>>8) & 0xff;
		data0 = audio_level & 0xff;
		
		//ES9018
		I2C_Write(0x90,23,data3);	//Lch Master Trim
		I2C_Write(0x90,22,data2);	//Lch Master Trim
		I2C_Write(0x90,21,data1);	//Lch Master Trim
		I2C_Write(0x90,20,data0);	//Lch Master Trim
		I2C_Write(0x92,23,data3);	//Rch Master Trim
		I2C_Write(0x92,22,data2);	//Rch Master Trim
		I2C_Write(0x92,21,data1);	//Rch Master Trim
		I2C_Write(0x92,20,data0);	//Rch Master Trim
		
		//dot-matrix
		if(digit4_digit8_check) 		dot_matrix_char(display_num);
		else 									dot_matrix_char_digit8(display_num);
			
		if(ch_led_data==0)			ch0_vol=display_numl;
		else if(ch_led_data==1)	ch1_vol=display_numl;
		else if(ch_led_data==2)	ch2_vol=display_numl;
		else if(ch_led_data==3)	ch3_vol=display_numl;
		else if(ch_led_data==4)	ch4_vol=display_numl;
		else if(ch_led_data==5)	ch5_vol=display_numl;
		else if(ch_led_data==6)	ch6_vol=display_numl;
		else if(ch_led_data==7)	ch7_vol=display_numl;
	}
}

void audio_level_up(){
	U8 data0,data1,data2,data3;
	if(display_num<100){
		display_num++;
		
		if(display_num==1)  				audio_level=0x4C8067FF;
		else if(display_num<6)			audio_level+=0x5F5E100;
		else if(display_num<47)		audio_level+=0x989680;
		else if(display_num<=100)	audio_level+=0xF4240;
		
		data3 = (audio_level>>24) & 0xff;
		data2 = (audio_level>>16) & 0xff;
		data1 = (audio_level>>8) & 0xff;
		data0 = audio_level & 0xff;
		
		//ES9018
		I2C_Write(0x90,23,data3);	//Lch Master Trim
		I2C_Write(0x90,22,data2);	//Lch Master Trim
		I2C_Write(0x90,21,data1);	//Lch Master Trim
		I2C_Write(0x90,20,data0);	//Lch Master Trim
		I2C_Write(0x92,23,data3);	//Rch Master Trim
		I2C_Write(0x92,22,data2);	//Rch Master Trim
		I2C_Write(0x92,21,data1);	//Rch Master Trim
		I2C_Write(0x92,20,data0);	//Rch Master Trim
		
		//dot-matrix
		if(digit4_digit8_check) 		dot_matrix_char(display_num);
		else 									dot_matrix_char_digit8(display_num);
		
		if(ch_led_data==0)			ch0_vol=display_numl;
		else if(ch_led_data==1)	ch1_vol=display_numl;
		else if(ch_led_data==2)	ch2_vol=display_numl;
		else if(ch_led_data==3)	ch3_vol=display_numl;
		else if(ch_led_data==4)	ch4_vol=display_numl;
		else if(ch_led_data==5)	ch5_vol=display_numl;
		else if(ch_led_data==6)	ch6_vol=display_numl;
		else if(ch_led_data==7)	ch7_vol=display_numl;
	}
}


void audio_level_sync(U8 ch_display_num){
	U8 data3,data2,data1,data0, i;
	U32 temp;
	
	if(ch_display_num==0)	temp=0;
	else if(ch_display_num==100)	temp=0x7fffffff;
	else {
		for(i=1; i<=ch_display_num; i++){
			if(i==1)  			temp=0x4C8067FF;
			else if(i<6)		temp+=0x5F5E100;
			else if(i<47)		temp+=0x989680;
			else if(i<100)		temp+=0xF4240;
		}
	}
	
	if(audio_level!=temp){
		audio_level=temp;
	
		data3 = (audio_level>>24) & 0xff;
		data2 = (audio_level>>16) & 0xff;
		data1 = (audio_level>>8) & 0xff;
		data0 = audio_level & 0xff;
	
		//ES9018
		I2C_Write(0x90,23,data3);	//Lch Master Trim
		I2C_Write(0x90,22,data2);	//Lch Master Trim
		I2C_Write(0x90,21,data1);	//Lch Master Trim
		I2C_Write(0x90,20,data0);	//Lch Master Trim
		I2C_Write(0x92,23,data3);	//Rch Master Trim
		I2C_Write(0x92,22,data2);	//Rch Master Trim
		I2C_Write(0x92,21,data1);	//Rch Master Trim
		I2C_Write(0x92,20,data0);	//Rch Master Trim
	}
	//dot-matrix
	if(digit4_digit8_check) 		dot_matrix_char(display_num);
	else 									dot_matrix_char_digit8(display_num);
}


*/

