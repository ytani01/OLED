/*
  pbm ファイルを コンソール1に表示する
  コンソールと画像のサイズを取得
  ヘッダ=P4, 白黒2値, バイナリフォーマット
  カーソルを消す: # setterm -cursor off > /dev/tty1
  syslogd をコンソールに出力している場合、表示後スクロールアウトする場合がある
  コンソール1を表示するために CTRL+ALT+F1 または chvt1 が必要
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

// main
int main(int argc, char* argv[]) {
  int fbfd = 0;
  // struct fb_var_screeninfo orig_vinfo;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  long int screensize = 0;
  char *fbp = 0;
  char inverse=0;

  // ヘルプ
  if (argc>1 && (strcmp(argv[1], "-h")==0 || strcmp(argv[1], "-H")==0)) {
    fprintf(stderr, " %s [-h][-i(inverse)] FILENAME.pnm\n", argv[0]);
    exit(0);
  }

  if (argc>1 && (strcmp(argv[1], "-i")==0)) { inverse=1; }
  
  // フレームバッファのファイルオープン
  fbfd = open("/dev/fb0", O_RDWR);
  if (!fbfd) {
    syslog(LOG_ERR, "Error: cannot open framebuffer device.\n");
    exit(1);
  }
  // syslog(LOG_INFO, "The framebuffer device was opened successfully.\n");
    
  // 画面情報(vinfo)の取得: x解像度, y解像度, デプス
  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
    syslog(LOG_ERR,"Error reading variable information.\n");
  }
  //syslog(LOG_INFO,"Original X=%d, Y=%d, DEPTH=%dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
  
  // 画面情報(finfo)の取得: 1ライン分のメモリサイズ
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
    syslog(LOG_ERR,"Error reading fixed information.\n");
  }
  // syslog(LOG_INFO, "finfo.line_length=%d\n", finfo.line_length);
	
  // フレームバッファをメモリに mmap() で割り付け
  screensize = finfo.smem_len;
  fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  if ((int)fbp == -1) {
    syslog(LOG_ERR, "Failed to mmap.\n");
    exit(1);
  }

  unsigned char vram[128][64];
  int x,y,i;
  int imgxsize,imgysize;
  
  unsigned int d, flag=0;
  FILE *fp;
  char str[16];
  
  // pbm ファイルの読み込み
  if ((fp = fopen(argv[argc-1],"r")) == NULL) { exit(1); }
 startReadPbm:
  fgets(str, 16, fp);
  if (feof(fp)) exit(0);
  if (strncmp(str, "P4", 2)) { fprintf(stderr,"%s: Not pbm binary(P4) format.\n", argv[1]); exit(1); }
  fscanf(fp, "%d %d\n", &imgxsize, &imgysize);

  // フレームバッファ消去
  for (y=0; y<64; y++) {
    for (x=0; x<128; x++) {
      fbp[y*finfo.line_length + x] = 0x00;
    }
  }
  
  // フレームバッファに書き込み
  // 全画像ファイルのバイト値を読む
  for (y=0; y<imgysize; y++) {
    for (x=0; x<(((imgxsize-1) / 8)+1); x++) {
      d = fgetc(fp);
      // そのうち画面内だけ表示
      if (y<64 && x<16) {
	for (i=0;i<8;i++) {
	  if (inverse==0) {
	    // 通常
	    if (d & (1<<(7-i))) fbp[y*finfo.line_length + x*8+i] = 0x07;
	    else fbp[y*finfo.line_length + x*8+i] = 0x00;
	  } else {
	    // 反転
	    if (d & (1<<(7-i))) fbp[y*finfo.line_length + x*8+i] = 0x00;
	    else fbp[y*finfo.line_length + x*8+i] = 0x07;
	  }
	}
      } // (y<64 && x<16)
    } // x
  } // y

  goto startReadPbm;
  close(fbfd);
  
  return 0;
}


/*
// 縦方向、逆向きの VRAM 構造
// vram[128][8]
// 座標 (x,y) への書き込み: vram[127-x][7-y/8] の (7-(y % 8)) ビット目を立てる

// 書き込み: oled.c の drawDisplay() 関数を参照
// 128バイトを i2c の 1セッションとして連続書き込みを 8セッション行う
// 毎セッション x,y の書き込み開始位置を設定する
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
*/

