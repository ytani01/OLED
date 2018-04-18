/*
  oled: 128*64 OLED にフレームバッファを連続転送してモニタとして利用する
  2017/9/19 Soft I2C 対応, Brightness オプション追加
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <syslog.h>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define OLED_ADDR (0x3C)   // (0x78>>1) = 0x3c
#define byte unsigned char
#define CMD (0x80)
#define DATA (0x40)

char vram[128][64];
byte *buf = NULL;
int bufsize=1024+1;
int pt;
int lcd;
char i2cError = 0;
char initial=1;
unsigned char brightness=0x80;

// i2c セッション開始
void i2c_write_session_begin(byte type) {
  pt=0;
  buf[pt]=type;
  pt++;
}

// i2c データ投入
void i2c_write(byte value) {
  buf[pt]=value;
  pt++;
  if (pt>=bufsize) {
    buf= realloc(buf, bufsize*2);
    bufsize = bufsize*2;
  }
}

// i2c セッション終了
void i2c_write_session_end(void) {
  if (write(lcd, buf, pt) != pt){
    if (i2cError==0) {
      syslog(LOG_INFO, "Error write() to i2c slave\n");
      usleep(1000*1000*60*5);
      exit(1);
    }
    i2cError = 1;
  } else {
    i2cError = 0;
  }
}

// OLED の初期化
void setup_i2c() {
  byte i,j,k;
  i2c_write_session_begin(CMD);
  //i2c_write(0xAE); // AF:ON, AE:スリープモード
  i2c_write(0xAE); // AF:ON, AE:スリープモード

  i2c_write(0xA4); // VRAM の内容を表示

  // 表示開始アドレス
  i2c_write(0x40);
  i2c_write(0xD3);
  i2c_write(0x00);

  // メモリアドレッシングモードの変更
  i2c_write(0x20); // メモリアドレッシングモードの変更
  i2c_write(0x02); // 00H:Hモード, 01H:Vモード, 02H:ページアクセスモード, 03H:無効

  // スクロールしない
  i2c_write(0x2E);

  // Y 開始位置(H/V アドレスモード)
  i2c_write(0x22);
  i2c_write(0x00);
  i2c_write(0x07);

  // X 開始位置(H/V アドレスモード)
  i2c_write(0x21);
  i2c_write(0x00);
  i2c_write(0x7f);

  // コントラスト設定
  i2c_write(0x81); // コントラスト設定
  i2c_write(brightness); // コントラスト

  // 描画方向
  i2c_write(0xA0);
  i2c_write(0xC0);
  
  // A6H: ノーマル, A7H: 白黒反転
  i2c_write(0xA6); 

  // チャージポンプ
  i2c_write(0x8d); // チャージポンプの設定
  i2c_write(0x14); // 14:ON, 10:OFF
  i2c_write(0xAF); // AF:ON, AE:スリープモード
  i2c_write_session_end();
}

// 描画実行
void drawDisplay() {
  static char prev[128][64];
  int x,y;
  char flag;
  for(y=0; y<8; y++){
    // 前回値と同じかチェック
    flag=0;
    for (x=0;x<128;x++) {
      if (vram[x][y] != prev[x][y]) { flag=1; }
      prev[x][y] = vram[x][y];
    }
    // 1ブロック転送
    if (flag || initial) {
      i2c_write_session_begin(CMD);
      // Y 開始アドレスの指定(ページアクセスモード用)
      i2c_write(0xB0 | y);
      // X 開始アドレスの指定(ページアクセスモード用)
      i2c_write(0x00 | 0x00); // X 開始アドレス低ニブル
      i2c_write(0x10 | 0x00); // X 開始アドレス高ニブル
      i2c_write_session_end();
      // 8バイト毎に水平に連続で16個表示
      i2c_write_session_begin(DATA);
      for(x=0; x<128; x++){
	i2c_write(vram[x][y]);
	//i2c_write(0xff);
      }// X ループ  
      i2c_write_session_end();
    } // if (flag || initial) 
  } // Yループ
  initial=0;
}

// main
int main(int argc, char* argv[]) {
  int fbfd = 0;
  // struct fb_var_screeninfo orig_vinfo;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  long int screensize = 0;
  char *fbp = 0;

  // ヘルプ
  if (argc>1 && (strcmp(argv[1], "-h")==0 || strcmp(argv[1], "-H")==0)) {
    fprintf(stderr, "OLED Display Driver\n");
    fprintf(stderr, "%s [waitTime(100)] [i2cAddress(0x3c)] [i2cDeviceFile(/dev/i2c-0)] [framebufferDeviceFile(/dev/fb0)][brightness(0-255)]\n", argv[0]);
    fprintf(stderr, "Example: %s 100 0x3c /dev/i2c-0 /dev/fb0 128\n", argv[0]);
    exit(0);
  }
  
  // I2C 通信用バッファ
  buf = realloc(buf, bufsize);
  
  // I2C デバイスのオープン
  if (argc>3) {
    if ((lcd = open(argv[3], O_RDWR)) >= 0) goto i2copen;
  } else {
    if ((lcd = open("/dev/i2c-1", O_RDWR)) >= 0) goto i2copen;
    if ((lcd = open("/dev/i2c-0", O_RDWR)) >= 0) goto i2copen;
  }
  syslog(LOG_INFO,"Faild to open() i2c port\n");
  exit(1);
 i2copen:
    
  // 通信先アドレスの設定
  {
    unsigned long oled_address = OLED_ADDR;
    int res;
    if (argc>2) {
      oled_address = strtol(argv[2], NULL, 0);
    }
    if (res = ioctl(lcd, I2C_SLAVE, oled_address) < 0){
      syslog(LOG_INFO,"Unable to get bus access to talk to slave\n");
      exit(1);
    }
    syslog(LOG_INFO,"To get bus access to talk to slave successfully.\n");
    //syslog(LOG_INFO,"code=%d\n", res);
  }

  // 明度
  if (argc>5) {
    sscanf(argv[5], "%d", &brightness);
  }
  
  
  // OLED setup
  setup_i2c();
  
  // フレームバッファのファイルオープン
  if (argc>4) {
    if (fbfd = open(argv[4], O_RDWR)) goto fbopen;
  } else {
    if (fbfd = open("/dev/fb0", O_RDWR)) goto fbopen;
  }
  syslog(LOG_INFO, "Error: cannot open() framebuffer device.\n");
  exit(1);
 fbopen:
  syslog(LOG_INFO, "The framebuffer device was opened successfully.\n");
  
  // 画面情報(vinfo)の取得
  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
    syslog(LOG_INFO,"Error reading variable information.\n");
    exit(1);
  }
  syslog(LOG_INFO,"vinfo.xres=%d, vinfo.yres=%d, vinfo.bits_per_pixel=%d\n",
	 vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
  
  // 画面情報(finfo)の取得
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
    syslog(LOG_INFO,"Error reading fixed information.\n");
    exit(1);
  }
  syslog(LOG_INFO, "finfo.line_length=%d\n", finfo.line_length);
	
  // フレームバッファをメモリに mmap() で割り付け
  screensize = finfo.smem_len;
  fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  if ((int)fbp == -1) {
    syslog(LOG_INFO, "Failed to mmap().\n");
    exit(1);
  }

  int x, y;
  unsigned int pix_offset;
  unsigned int value;

  while(1) {
    // フレームバッファを VRAM に転送
    for (y = 0; y < 64; y++) {
      for (x = 0; x < 128; x++) {
	// ピクセルのメモリ先頭からのオフセットを算出
        pix_offset = x + y * finfo.line_length;
        value = *((char*)(fbp + pix_offset));
	if (value) { vram[127-x][7-y/8] |= 1<<(7-y % 8); }
	else       { vram[127-x][7-y/8] &= ~(1<<(7-y % 8)); }
      } // x
    } // y

    // OLED 表示
    drawDisplay();

    // wait
    {
      useconds_t waittime = 100*1000;
      if (argc>1) {
	waittime = strtol(argv[1], NULL, 0) * 1000;
      }
      usleep(waittime);
    }
    
  } // while

  close(fbfd);
  close(lcd);
  return 0;
}
