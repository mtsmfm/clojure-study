#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
  最上位から探索して０ビットが初めて出現する位置を返す。
 */
int findZeroBit(unsigned char table, int n){
  return (table >> (7 - n)) & 0x1;
}

/*
  ビットの左回転
 */
unsigned char lrotate(unsigned char table, int n){
  return table << n | table >> (8 - n);
}

/*
  ビットの右回転
 */
unsigned char rrotate(unsigned char table, int n){
  return table << (8 - n) | table >> n;
}

/*
　８桁の数字の文字列（つまり８バイト）から０位外は１に置き換えて、
　unsigned char（１バイト）へ情報を圧縮する。
　情報圧縮は副次的で、主目的はビットローテーションを使うために
　用意した。
 */
unsigned char makeBitTable(char* table){
  char* wk = table;
  int count = 0;
  unsigned char bit;
  unsigned char result = 0b00000000;
  while(*wk != '\0'){
    if(*wk++ == '0'){
      bit = 0b0;
    } else {
      bit = 0b1;
    }
    result |= (bit << (7 - count++));
  }
  return result;
}


/*
  破壊的副作用あり
　最後のテストの期待値に合わせるために用意した。
　８桁の数字の文字列（つまり８バイト）から０位外は１に置き換えた
　８桁の数字の文字列を返す。
 */
void makeBitTableString(char* table){
  char* wk = table;
  while(*wk != '\0'){ 
    switch(*wk){
    case '0':
      break;
    default:
      *wk = '1';
    }
    wk++;
  } 
}

/*
  破壊的副作用あり
　空席が０、調理待ちが１，食事中が１、一休みが２を意味する。
　状態が一段階進む処理。
 */
void nextTable(char* table)
{
  char* wk = table;
  while(*wk != '\0'){ 
    switch(*wk){
    case '1':
      *wk = '2';
      break;
    case '2':
      *wk = '3';
      break;
    case '3':
      *wk = '0';
      break;
    default:
      *wk = *wk;
    }
    wk++;
  } 
}

/*
  破壊的副作用あり
  席番号から人数分、着席（つまり調理待ち状態）になる。
　空席かどうかのチェックはしていない。
 */
void sitdown(char *table, int seatNo, int num){
  int i;
  if(seatNo + num < 8){
    for(i = seatNo; i < seatNo + num; i++){
      table[i] = '1';
    }
  } else {
    for(i = seatNo; i < 8; i++){
      table[i] = '1';
    }
    for(i = 0; i < seatNo + num - 8; i++){
      table[i] = '1';
    }
  }
}

/*
　グループの人数分が隣接して座れる座席の中で座席番号が
　若い番号を返す。着席できない場合は９を返す。
　できる場合は、席番号（０から７まで）を返す。

　ビットローテーションを、グループ人数分回転しつつ、
　毎回ORをとることにより、人数分空席が隣接しているかの判断条件に使っている。

　ここが、アルゴリズム的な肝の部分である。
 */
int findHeadSeatNo(char* table, int grpNum){
  unsigned char bitTable = makeBitTable(table);
  int i;
  for(i = 0; i < grpNum - 1; i++){
    bitTable |= lrotate(bitTable, 1);
    if(bitTable == 0xff) break;
  }
  for(i = 0; i < 8; i++){
    if(!findZeroBit(bitTable, i)) return i;
  }
  return 9;
}


/*
  グループ人数分が着席できるまで状況を進行する。
 */
void process(char* table, int grpNum){
  nextTable(table);
  int seatNo;
  while((seatNo = findHeadSeatNo(table, grpNum)) == 9) nextTable(table);
  sitdown(table, seatNo, grpNum);
}


/*
  テストを実行する
　合格ならOK、不合格ならNGを印字する。
 */
void test(char* waitline, char* expected){
  char* wk = waitline;
  char grpNumChar;
  int grpNum;
  char table[9] = "00000000";
  while((grpNumChar = *wk++) != '\0'){
    grpNum = grpNumChar - '0';
    process(table, grpNum);
    //    printf("[%d]%s\n", grpNum, table);    
  }
  makeBitTableString(table);
  char result[3];
  strcpy(result, !strcmp(table, expected) ? "OK" : "NG");
  printf("[%s], %12s, %s\n", result, waitline, expected);
}


int main(void){
  test("3316", "11111110");
  test("3342153", "11111111");
  test("8", "11111111");
  test("232624", "11110110");
  test("1", "10000000");
  test("224673432", "11111000");
  test("123464334", "11111110");
  test("44372332", "11111111");
  test("2344", "11111111");
  test("6448476233", "11111100");
  test("4345174644", "11111111");
  test("77743", "11111110");
  test("2136524281", "10000000");
  test("344373", "11100000");
  test("434226", "11111111");
  test("7433223", "11111110");
  test("2246534", "11110111");
  test("634", "11111110");
  test("2222", "11111100");
  test("2442343238", "11111111");
  test("7243344", "11111111");
  test("26147234", "10111111");
  test("34424", "10011111");
  test("6334", "11011111");
  test("3828342", "11011110");
  test("4431", "11110000");
  test("2843212125", "11111111");
  test("333336482", "11000000");
  test("374", "11110000");
  test("4382333", "11100111");
}

