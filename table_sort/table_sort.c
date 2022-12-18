#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#define BUF_SIZE 100
typedef struct table_entry {
  int year;
  int month;
  int day;

  int ddd;

  int hour;
  int minute;
  int second;

  int cscore;
  
  float u[11]; // foF2, foF1, foE, foEs, h`Es,
               // hmF2  hmF1, hmE, B0, B1, D1

} table_entry;

FILE* fp;
char cur_char =' ';
char buf[BUF_SIZE];

int line_num= 0;
int col_num= 0;

// Reads the next character and updates the line_num and col_num variables
// appropriately
void advance()
{
  if(cur_char == EOF) {
    fprintf(stderr, "Attempted to read past end of file");
    exit(EXIT_FAILURE);
  }
    
  cur_char = fgetc(fp);
  if(cur_char == '\n') {
    line_num += 1;
    col_num = 1;
  }
  else
    col_num += 1;
}

void report_error(const char* msg) {
  fprintf(stderr, "Error at line %d column %d: %s\n", line_num, col_num, msg);
  exit(EXIT_FAILURE);
}

// Reads the floating point number at the position of cur_char
// Accounts for the possibility the inputis '---' indicating a
// missing entry
void read_float(float* out)
{
  int buf_idx = 0;
  while(isdigit(cur_char) || cur_char == '.' || cur_char=='-') {
    buf[buf_idx] = cur_char;
    advance(fp);
    buf_idx++;
    if(buf_idx >= BUF_SIZE) {
      report_error("Attempt to read float overflowed buffer");
    }
  }
  buf[buf_idx] = '\0';
  
  // Check if entry was missing
  if(strcmp(buf, "---") == 0) {
    *out = NAN;
    return;
  }
  
  char* ptr;
  double val = strtod(buf, &ptr); 

  if(*ptr != '\0') {
    report_error("Error converting string to float");
  }

  *out = (float) val;
}

// Reads the integer at the position of cur_char
void read_int(int* out)
{
  int buf_idx = 0;
  while(isdigit(cur_char)) {
    buf[buf_idx] = cur_char;
    advance(fp);
    buf_idx++;
    if(buf_idx >= BUF_SIZE) {
      report_error("Attempt to read integer overflowed buffer");
    }
  }
  buf[buf_idx] = '\0';
  char* ptr;
  long val = strtol(buf, &ptr, 10); 

  if(*ptr != '\0') {
    report_error("Error converting string to int");
  }

  *out = (int) val;
}

// Reads date at cur_char and stores it into entry
void read_date(table_entry* entry)
{
  read_int(&entry->year);
  if(cur_char != '.') {
    report_error("Expected '.' to follow year");
  }
  advance();
  read_int(&entry->month);
  if(cur_char != '.') {
    report_error("Expected '.' to follow month");
  }
  advance();
  read_int(&entry->day);
}

void read_ddd(table_entry* entry)
{
  if(cur_char != '(') {
    report_error ("Expected '('");
  }
  advance();
  read_int(&entry->ddd);
  if(cur_char != ')') {
    report_error("Expected ')'");
  }
  advance();
}
void read_time(table_entry* entry)
{
  read_int(&entry->hour);
  if(cur_char != ':') {
    report_error("Expected  after hour ':'");
  }
  advance();
  read_int(&entry->minute);
  if(cur_char != ':') {
    report_error("Expected after minute ':'");
  }
  advance();
  read_int(&entry->second);
}

void read_line(table_entry* entry) {
  read_date(entry);
  
  while(isspace(cur_char)) {
    advance(); 
  }

  read_ddd(entry);

  while(isspace(cur_char)) {
    advance();
  }

  read_time(entry);

  while(isspace(cur_char)) {
    advance();
  }

  read_int(&entry->cscore);

  for(int i = 0; i < 10; i++) {
    while(isspace(cur_char)) {
      advance();
    }
    read_float(entry->u+i);
  }

  // Advance until the start of the next line
  while(isspace(cur_char)) {
    advance();
  }
}

//void sortByField(int field, table_entry* entries, int lo, int hi);
//int partition(int field, table_entry* entries, int lo, int hi); 

// partition function for quicksort implementation of sortByField
int partition(int field, table_entry* entries, int lo, int hi)
{
  int i = lo+1;
  int j = hi;
  float v = entries[lo].u[field];
  table_entry tmp;
  while(1) {
    while (entries[i].u[field] < v) {
      i = i+1;
      if (i == hi)
	break;
    }
    while (v < entries[j].u[field]) {
      j = j - 1;
      if (j == lo)
	break;
    }

    if (i >= j)
      break;
    // Swap entries i and j
    tmp = entries[i];
    entries[i] = entries[j];
    entries[j] = tmp; 
  }

  // Swap entries lo and j
  tmp = entries[lo];
  entries[lo] = entries[j];
  entries[j] = tmp; 
}

// Sorts the entries according to a field in the u field of the
// table data
void sortByField(int field, table_entry* entries, int lo, int hi)
{
  if (hi <= lo)
    return;
  int j = partition(field, entries, lo, hi);
  sortByField(field, entries, lo,j-1);
  sortByField(field, entries, j+1,hi);
}

int main() {
  table_entry entries[500];
  fp = fopen("AU930_ROAM.TXT", "r");
  // Ignore first line containing column names
  while(cur_char != '\n')
    advance();
  
  // Skip whitespace until the table begins
  while(isspace(cur_char))
    advance();
  int i = 0;
  while(cur_char != EOF) {
    read_line(entries+i);
    i = i+1;
  }
}
  

