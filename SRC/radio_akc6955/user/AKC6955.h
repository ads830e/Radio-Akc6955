
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
    unsigned char seek_direction:       1; // 0 下 1 上
    unsigned char go_seek:            1; // 0->1 开始搜索 STC=1 停止搜索
    unsigned char tune:             1; // 0->1 到指定的频率
    unsigned char fm_mode:            1;
    unsigned char power_on:           1;
  };
};

union reg1
{
  unsigned char value;
  struct
  {
    unsigned char fm_band:            3; // 可以设为000 FM 187~108, 搜台间隔有space定
    unsigned char am_band:            5; // 可以设为00010 MW2，0.522~1.62, 9K搜台
  };
} ;

union reg2
{
  unsigned char value;
  struct
  {
    unsigned char chanel_freq_high:     5; // 信道号的高5位 FM mode: Channel Freq.=25kHz*CHAN + 30MHz   AM mode, 5K信道号模式时:Channel Freq.= (mode3k?3:5) kHz*CHAN
    unsigned char mode3k:             1; // MCU, MW2 工作时，送入的信道号一定要保证是 3 的倍数。否则电台会乱掉
    unsigned char ref_32kmode:        1; // 1—参考时钟为32.768K / 0 1—参考时钟为12M
    unsigned char reserved:           1;
  };
};

union reg3
{
  unsigned char value;
  struct
  {
    unsigned char chanel_freq_low:      8;  // 信道号的低 8 位
  };
};

union reg4
{
  unsigned char value;
  struct
  {
    unsigned char user_chanel_start:    8; // 自定义信道号起始 //chan=32*usr_chan_start
  };
};

union reg5
{
  unsigned char value;
  struct
  {
    unsigned char user_chanel_end:      8; // 自定义信道号终止 chan=32*usr_chan_stop
  };
};

union reg6
{
  unsigned char value;
  struct
  {
    unsigned char phase_inv:          1; // 0 同相 / 1反相
    unsigned char line_in:            1; // 0 OFF / 1 ON
    unsigned char sound_volume:       6; // <24:mute，24~63共40级音量控制，每级1.5dB pd_adc_vol=1 时用该寄存器音量
  };
};


union reg7
{
  unsigned char value;
  struct
  {
    unsigned char fm_band_width:        2; // 00 150K 01 200K 10 50K 11 100K
    unsigned char stereo:             2; // “00”自动立体声,门限有Stereo_th控制 “10”只要有导频就强制立体声 “x1”强制单声道解调
    unsigned char bass_boost:         1;
    unsigned char de:               1; // 去加重模式选择 0— 75 μ s (USA) 1— 50 μ s (China / Europe)
    unsigned char reserved0:            1;
    unsigned char reserved1:            1;
  };
};


union reg8
{
  unsigned char value;
  struct
  {
    unsigned char stereo_threshold:       2; // FM开始立体声解调的CNR门限值
    unsigned char fd_threshold:         2; // 芯片内判台和点灯用的频偏门限
    unsigned char am_cnr_threshold:       2; // AM模式时，芯片判台和点灯的载噪比门限
    unsigned char fm_cnr_threshold:       2; // FM模式时，芯片判台和点灯的载噪比门限
  };
};

union reg9
{
  unsigned char value;
  struct
  {
    unsigned char lv_en:            1; //在低压供电时，是否进入低压工作模式
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
    unsigned char fm_seek_skip:       2; // FM 搜台步进 00—25kHz 01—50kHz 10—100kHz 11—200kHz, 针对 TV1 和 TV2 该档为 100K
    unsigned char reserved1:          2;
  };
};

union reg12
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          5;
    unsigned char pd_rx:              1; // 1--模拟与射频通道关闭
    unsigned char reserved1:          1;
    unsigned char pd_adc:           1; // 0-- 信号通道 ADC 打开
  };
};

union reg13
{
  unsigned char value;
  struct
  {
    unsigned char reserved0:          2;
    unsigned char predefined_volume:    2; // 输出音量调整整值：00： 0dB 01： 3.5dB 10： 7dB 11： 10.5dB
    unsigned char reserved1:          2;
    unsigned char st_led:           1; // 0—tund管脚为调谐灯 1—FM 且非wtmode时，tund管脚为立体声解调指示灯，其余为调谐灯
    unsigned char reserved2:          1;
  };
};

// *************************************** 以下为只读 ********************************************************//
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
    unsigned char cnr_am:           7; // AM 制式时信号的载噪比，单位 dB
    unsigned char mode3k:           1;
  };
};

union reg23
{
  unsigned char value;
  struct
  {
    unsigned char cnr_fm:               7; // FM 制式时信号的载噪比，单位 dB
    unsigned char stereo_demode:        1; // 只有 FM 立体声解调时（立体声比例大于 30%）才显示 1
  };
};

union reg24
{
  unsigned char value;
  struct
  {
    unsigned char lvmod:              1; // 低电压最大音量限制模式指示位
    unsigned char reserved:           1;
    unsigned char pgalevel_if:        3; // 射频功率控制环增益等级, 等级越大，增益约高
    unsigned char pgalevel_rf:        3; // 射频功率控制环增益等级, 等级越大，增益约高
  };
};

union reg25
{
  unsigned char value;
  struct
  {
    unsigned char vbat:           6; // 电源电压指示：vcc_bat(V)=1.8+0.05*vbat
    unsigned char reserved:       2;
  };
};

union reg26
{
  unsigned char value;
  struct
  {
    unsigned char fd_num:           8; // 频偏指示，补码格式，大于 127 时，减 256 即可变为正常数值，注意此处 FM 以 1KHz 单位，AM 以100Hz 为单位
  };
};

union reg27
{
  unsigned char value;
  struct
  {
    unsigned char rssi:           7; // 可利用 rssi、pgalevel_rf、pgalevel_if 计算天线口信号电平 FM/SW: Pin(dBuV) = 103 - rssi - 6*pgalevel_rf - 6*pgalevel_if MW/LW: Pin(dBuV) = 123 - rssi - 6*pgalevel_rf - 6*pgalevel_if
    unsigned char reserved:       1;
  };
};


void AKC6955_Init(void);

void AKC6955_SetVolume(uint32_t volume);

bool AKC6955_SetChannel(bool fm,uint32_t channel);

bool AKC6955_IsResetNeeded(void);


#endif

