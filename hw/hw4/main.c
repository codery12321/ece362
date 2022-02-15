
#if defined(STM32)
#include "stm32f0xx.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Q1 */ void hello(void);
/* Q2 */ void showsub2(int a, int b);
/* Q3 */ void showsub3(int a, int b, int c);
/* Q4 */ void listing(const char *school, int course, const char *verb,
                      int enrollment, const char *season, int year);
/* Q5 */ int  trivial(int);
/* Q6 */ int  depth(int x, const char *s);
/* Q7 */ int  collatz(int x);
/* Q8 */ int  permute(int a, int b, int c, int d, int e, int f);
/* Q9 */ int  bizarre(int base, int nth);
/* Q10 */ int easy(int a, int b, int shift, int mask, int skip);

static int c_trivial(unsigned int n)
{
    int tmp[100]; // a stack-allocated array of 100 words
    for(int x=0; x < sizeof tmp / sizeof tmp[0]; x += 1)
        tmp[x] = x+1;
    if (n >= sizeof tmp / sizeof tmp[0])
        n = sizeof tmp / sizeof tmp[0] - 1;
    return tmp[n];
}

void onoff(const char *s)
{
#if defined(STM32)
  s++;
  const char p = *s++;
  int n = 0;
  while(*s >= '0' && *s <= '9') n = n*10 + *s++ - '0';
  if (n > 15) return;
  if ((p == 'a' && (n == 13 || n == 14)) || (p == 'c' && n > 12) ||
      (p == 'b' && (n == 10 || n == 11)) || p == 'e') return;
  RCC->AHBENR /*easteregg*/ |= RCC_AHBENR_GPIOAEN<<(p-'a');
  typeof(GPIOA) pts[]={GPIOA,GPIOB,GPIOC,GPIOD,0,GPIOF};
  typeof(GPIOA) pt=pts[p-'a'];
  pt->MODER = (pt->MODER & ~(3<<(2*n))) | (1<<(2*n));
  pt->ODR ^= 1<<n;
#endif
}

void test_hello(void) {
    hello();
}
void test_showsub2(void) {
  for(int i=4; i<7; i++)
    for(int j=1; j<6; j++)
      showsub2(i,j);
}

void test_showsub3(void) {
  for(int i=4; i<7; i++)
    for(int j=2; j<4; j++)
      for(int k=2; k<4; k++)
        showsub3(i,j,k);
}

void test_listing(void) {
  listing("ECE", 27000, "had", 420, "Spring", 2020);
  listing("ECE", 36200, "had", 210, "Spring", 2020);
  listing("ECE", 27000, "had", 240, "Fall", 2020);
  listing("ECE", 36200, "had", 280, "Fall", 2020);
  listing("ECE", 27000, "had", 438, "Spring", 2021);
  listing("ECE", 36200, "had", 246, "Spring", 2021);
  listing("ECE", 27000, "had", 205, "Fall", 2021);
  listing("ECE", 36200, "had", 301, "Fall", 2021);
  listing("ECE", 27000, "has", 466, "Spring", 2022);
  listing("ECE", 36200, "has", 202, "Spring", 2022);
}

void test_trivial(void) {
    printf("  trivial(-1) = %d\n",   trivial(-1));
    printf("c_trivial(-1) = %d\n", c_trivial(-1));
    for(int i=1; i<20; i+=3) {
        printf("  trivial(%d) = %d\n", i,   trivial(i));
        printf("c_trivial(%d) = %d\n", i, c_trivial(i));
    }
}

void test_depth(void)
{
    const char *base_str = "This is a a test.";
    for(int i=0; i<5; i++) {
        const char *s = &base_str[i*2];
        printf("Calling depth(%d, \"%s\")\n", i, s);
        int val = depth(i, s);
        printf("Return value = %d\n", val);
    }
}

unsigned int c_collatz(unsigned int n) {
    if (n == 1)
        return 0;
    if ((n & 1) == 0)
        return 1 + c_collatz(n/2);
    return 1 + c_collatz(3*n+1);
}

void test_collatz(void)
{
    for(int i=4; i<30; i++) {
        printf("collatz(%d)\n", i);
        int val = collatz(i);
        printf("return value = %d\n", val);
        printf("c_collatz(%d) = %d\n", i, c_collatz(i));
    }
}

int c_permute(int a, int b, int c, int d, int e, int f) {
    if (a <= 0)
        return f + e + d + c + b + a;
    return c_permute(f-1,a,b,c,d,e) + 1;
}

void test_permute(void)
{
    const static int in[][6] = {
            { 6,5,4,3,2,1 },
            { 5,4,5,4,3,2 },
            { 5,4,5,4,4,2 },
            { 5,4,5,4,4,3 },
            { 7,6,4,4,5,4 },
    };
    for(int x=0; x<5; x++) {
        printf("permute(%d,%d,%d,%d,%d,%d)\n",
                in[x][0],in[x][1],in[x][2],in[x][3],in[x][4],in[x][5]);
        int val = permute(in[x][0],in[x][1],in[x][2],in[x][3],in[x][4],in[x][5]);
        printf("return value = %d\n", val);
        printf("c_permute(%d,%d,%d,%d,%d,%d) = %d\n",
                in[x][0],in[x][1],in[x][2],in[x][3],in[x][4],in[x][5],
                c_permute(in[x][0],in[x][1],in[x][2],in[x][3],in[x][4],in[x][5]));
    }
}

int c_compare(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}
int c_bizarre(int base, int nth) {
    int array[200];
    int x;
    for(x=0; x < 200; x++)
        array[x] = ((base+x+1) * 255) & 0xff;
    qsort(array, 200, 4, c_compare);
    return array[nth];
}

void test_bizarre(void)
{
    const static int in[][2] = {
            { 1,10 },
            { 2,30 },
            { 3,50 },
            { 4,70 },
            { 5,90 },
    };
    for(int x=0; x<5; x++) {
        printf("bizarre(%d,%d)\n", in[x][0], in[x][1]);
        int val = bizarre(in[x][0], in[x][1]);
        printf("return value = %d\n", val);
        printf("c_bizarre(%d,%d) = %d\n", in[x][0], in[x][1],
                c_bizarre(in[x][0], in[x][1]));
    }
}

int c_easy(int a, int b, int shift, int mask, int skip)
{
    int save[64];
    int result = 0;
    save[0] = 1;
    for(int x=1; x < sizeof save / sizeof save[0]; x += 1)
        save[x] = (save[x-1] ^ (x & mask)) << shift;
    for(int x=a; x!=b; x = (x * 5 + skip) & 63)
        result ^= save[x];
    return result;
}

void test_easy(void)
{
    const static int in[][5] = {
            { 1, 2, 1, 2, 1 },
            { 3, 4, 1, 2, 1 },
            { 3,10, 2, 5, 1 },
            { 6,35, 3, 7, 1 },
            { 5,42, 4, 9, 3 },
    };
    for(int x=0; x<5; x++) {
        printf("easy(%d,%d,%d,%d,%d)\n",
                in[x][0],in[x][1],in[x][2],in[x][3],in[x][4]);
        int val = easy(in[x][0],in[x][1],in[x][2],in[x][3],in[x][4]);
        printf("return value = %d\n", val);
        printf("c_easy(%d,%d,%d,%d,%d) = %d\n",
                        in[x][0],in[x][1],in[x][2],in[x][3],in[x][4],
                        c_easy(in[x][0],in[x][1],in[x][2],in[x][3],in[x][4]));
    }
}

void test_all(void) {
  test_hello(); test_showsub2(); test_showsub3(); test_listing();
  test_trivial(); test_depth(); test_collatz(); test_permute();
  test_bizarre(); test_easy();
}

void (*test[])(void) = {
  test_all, test_hello, test_showsub2, test_showsub3, test_listing,
  test_trivial, test_depth, test_collatz, test_permute, test_bizarre,
  test_easy,
  test_all
};

void serial_init();

int main()
{
#if defined(STM32)
  serial_init();
#endif

  for(;;) {
      char buffer[10];
      printf("\nSelect test: ");
      fgets(buffer, 10, stdin); buffer[9] = '\0';
      if (buffer[0] == 'p' && buffer[1] >= 'a' && buffer[1] <= 'f')
          onoff(buffer);
      else {
          int num = 0;
          if (sscanf(buffer, "%d", &num) != 1 || num < 1 || num > 11) {
              printf("Enter a number between 1 and 11.\n");
              continue;
          }
          test[num]();
      }
  }
/*
  test_hello();
  test_showsub2();
  test_showsub3();
  test_listing();
  test_trivial();
  test_depth();
  test_collatz();
  test_permute();
  test_bizarre();
  //test_easy();*/
  return 0;
}

