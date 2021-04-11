
#ifndef __AKC6955_H__
#define __AKC6955_H__


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>



#define AM  0
#define FM  1

enum band_value
{
  FM1 = 0x00,
  MW2 = 0x02,

  SW1 = 0x05,
  SW2 = 0x06,
  SW3 = 0x07,
  SW4 = 0x08,
  SW5 = 0x09,
  SW6 = 0x0a,
  SW7 = 0x0b,
  SW8 = 0x0c,
  SW9 = 0x0d,
  SW10 = 0x0e,
  SW11 = 0x0f,
  
  LW = 0x00,
  
  TV1 = 0x05,
  TV2 = 0x06,
  //MW1 = 0x01,
};

enum band
{
  _FM1 = 0,
  _MW2,

  _SW1,
  _SW2,
  _SW3,
  _SW4,
  _SW5,
  _SW6,
  _SW7,
  _SW8,
  _SW9,
  _SW10,
  _SW11,

  _LW,
  _TV1,
  _TV2,
  //_MW1,
};

union reg0
{
  unsigned char value;
  struct
  {
    unsigned char reserved:            2;
    unsigned char mute:             1;
    unsigned char seek_direction:       1; // 0 �� 1 ��
    unsigned char go_seek:            1; // 0->1 ��ʼ���� STC=1 ֹͣ����
    unsigned char tune:             1; // 0->1 ��ָ����Ƶ��
    unsigned char fm_mode:            1;
    unsigned char power_on:           1;
  };
};

union reg1
{
  unsigned char value;
  struct
  {
    unsigned char fm_band:            3; // ������Ϊ000 FM 187~108, ��̨�����space��
    unsigned char am_band:            5; // ������Ϊ00010 MW2��0.522~1.62, 9K��̨
  };
} ;

union reg2
{
  unsigned char value;
  struct
  {
    unsigned char chanel_freq_high:     5; // �ŵ��ŵĸ�5λ FM mode: Channel Freq.=25kHz*CHAN + 30MHz   AM mode, 5K�ŵ���ģʽʱ:Channel Freq.= (mode3k?3:5) kHz*CHAN
    unsigned char mode3k:             1; // MCU, MW2 ����ʱ��������ŵ���һ��Ҫ��֤�� 3 �ı����������̨���ҵ�
    unsigned char ref_32kmode:        1; // 1���ο�ʱ��Ϊ32.768K / 0 1���ο�ʱ��Ϊ12M
    unsigned char reserved:           1;
  };
};

union reg3
{
  unsigned char value;
  struct
  {
    unsigned char chanel_freq_low:      8;  // �ŵ��ŵĵ� 8 λ
  };
};

union reg4
{
  unsigned char value;
  struct
  {
    unsigned char user_chanel_start:    8; // �Զ����ŵ�����ʼ //chan=32*usr_chan_start
  };
};

union reg5
{
  unsigned char value;
  struct
  {
    unsigned char user_chanel_end:      8; // �Զ����ŵ�����ֹ chan=32*usr_chan_stop
  };
};

union reg6
{
  unsigned char value;
  struct
  {
    unsigned char phase_inv:          1; // 0 ͬ�� / 1����
    unsigned char line_in:            1; // 0 OFF / 1 ON
    unsigned char sound_volume:       6; // <24:mute��24~63��40���������ƣ�ÿ��1.5dB pd_adc_vol=1 ʱ�øüĴ�������
  };
};


union reg7
{
  unsigned char value;
  struct
  {
    unsigned char fm_band_width:        2; // 00 150K 01 200K 10 50K 11 100K
    unsigned char stereo:             2; // ��00���Զ�������,������Stereo_th���� ��10��ֻҪ�е�Ƶ��ǿ�������� ��x1��ǿ�Ƶ��������
    unsigned char bass_boost:         1;
    unsigned char de:               1; // ȥ����ģʽѡ�� 0�� 75 �� s (USA) 1�� 50 �� s (China / Europe)
    unsigned char reserved0:            1;
    unsigned char reserved1:            1;
  };
};


union reg8
{
  unsigned char value;
  struct
  {
    unsigned char stereo_threshold:       2; // FM��ʼ�����������CNR����ֵ
    unsigned char fd_threshold:         2; // оƬ����̨�͵���õ�Ƶƫ����
    unsigned char am_cnr_threshold:       2; // AMģʽʱ��оƬ��̨�͵�Ƶ����������
    unsigned char fm_cnr_threshold:       2; // FMģʽʱ��оƬ��̨�͵�Ƶ����������
  };
};

union reg9
{
  unsigned char value;
  struct
  {
    unsigned char lv_en:            1; //�ڵ�ѹ����ʱ���Ƿ�����ѹ����ģʽ
    unsigned char reserved0:          1;
    unsigned char oscillator_type:      1; //Oscillator source selection
    unsigned char i2c_volume:         1;
    unsigned char reserved1:          4;
  };
};

union reg11
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          4;
    unsigned char fm_seek_skip:       2; // FM ��̨���� 00��25kHz 01��50kHz 10��100kHz 11��200kHz, ��� TV1 �� TV2 �õ�Ϊ 100K
    unsigned char reserved1:          2;
  };
};

union reg12
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          5;
    unsigned char pd_rx:              1; // 1--ģ������Ƶͨ���ر�
    unsigned char reserved1:          1;
    unsigned char pd_adc:           1; // 0-- �ź�ͨ�� ADC ��
  };
};

union reg13
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          2;
    unsigned char predefined_volume:    2; // �������������ֵ��00�� 0dB 01�� 3.5dB 10�� 7dB 11�� 10.5dB
    unsigned char reserved1:          2;
    unsigned char st_led:           1; // 0��tund�ܽ�Ϊ��г�� 1��FM �ҷ�wtmodeʱ��tund�ܽ�Ϊ���������ָʾ�ƣ�����Ϊ��г��
    unsigned char reserved2:          1;
  };
};

// *************************************** ����Ϊֻ�� ********************************************************//
union reg20
{
  unsigned char value;
  struct
  {
    unsigned char read_chanel_high:     5;
    unsigned char tuned:              1;
    unsigned char finished:           1;
    unsigned char stereo:           1;
  };
};

union reg21
{
  unsigned char value;
  struct
  {
    unsigned char read_chanel_low:    8;
  };
};

union reg22
{
  unsigned char value;
  struct
  {
    unsigned char cnr_am:           7; // AM ��ʽʱ�źŵ�����ȣ���λ dB
    unsigned char mode3k:           1;
  };
};

union reg23
{
  unsigned char value;
  struct
  {
    unsigned char cnr_fm:               7; // FM ��ʽʱ�źŵ�����ȣ���λ dB
    unsigned char stereo_demode:        1; // ֻ�� FM ���������ʱ���������������� 30%������ʾ 1
  };
};

union reg24
{
  unsigned char value;
  struct
  {
    unsigned char lvmod:              1; // �͵�ѹ�����������ģʽָʾλ
    unsigned char reserved:           1;
    unsigned char pgalevel_if:        3; // ��Ƶ���ʿ��ƻ�����ȼ�, �ȼ�Խ������Լ��
    unsigned char pgalevel_rf:        3; // ��Ƶ���ʿ��ƻ�����ȼ�, �ȼ�Խ������Լ��
  };
};

union reg25
{
  unsigned char value;
  struct
  {
    unsigned char vbat:           6; // ��Դ��ѹָʾ��vcc_bat(V)=1.8+0.05*vbat
    unsigned char reserved:       2;
  };
};

union reg26
{
  unsigned char value;
  struct
  {
    unsigned char fd_num:           8; // Ƶƫָʾ�������ʽ������ 127 ʱ���� 256 ���ɱ�Ϊ������ֵ��ע��˴� FM �� 1KHz ��λ��AM ��100Hz Ϊ��λ
  };
};

union reg27
{
  unsigned char value;
  struct
  {
    unsigned char rssi:           7; // ������ rssi��pgalevel_rf��pgalevel_if �������߿��źŵ�ƽ FM/SW: Pin(dBuV) = 103 - rssi - 6*pgalevel_rf - 6*pgalevel_if MW/LW: Pin(dBuV) = 123 - rssi - 6*pgalevel_rf - 6*pgalevel_if
    unsigned char reserved:       1;
  };
};


void AKC6955_Init(void);

void AKC6955_SetVolume(uint32_t volume);

bool AKC6955_SetChannel(bool fm,uint32_t channel);

bool AKC6955_IsResetNeeded(void);


#endif

