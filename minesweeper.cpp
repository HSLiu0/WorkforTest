#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include "minesweeper.h"

using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* pre-supplied function to load mine positions from a file */
void load_board(const char *filename, char board[9][9]) {

  cout << "Loading board from file '" << filename << "'... ";

  ifstream in(filename);
  if (!in)
    cout << "Failed!" << endl;
  assert(in);

  char buffer[512];

  int row = 0;
  in.getline(buffer,512);
  while (in && row < 9) {
    for (int n=0; n<9; n++) {
      assert(buffer[n] == '.' || buffer[n] == '*' || buffer[n] == ' ' || buffer[n] == '?' || isdigit(buffer[n]));
      board[row][n] = buffer[n];
    }
    row++;
    in.getline(buffer,512);
  }

  cout << ((row == 9) ? "Success!" : "Failed!") << endl;
  assert(row == 9);
}

/* internal helper function */
void print_row(const char *data, int row) {
  cout << (char) ('A' + row) << "|";
  for (int i=0; i<9; i++) 
    cout << ( (data[i]=='.') ? ' ' : data[i]);
  cout << "|" << endl;
}

/* pre-supplied function to display a minesweeper board */
void display_board(const char board[9][9]) {
  cout << "  ";
  for (int r=0; r<9; r++) 
    cout << (char) ('1'+r);
  cout <<  endl;
  cout << " +---------+" << endl;
  for (int r=0; r<9; r++) 
    print_row(board[r],r);
  cout << " +---------+" << endl;
}

/* pre-supplied function to initialise playing board */ 
void initialise_board(char board[9][9]) {
  for (int r=0; r<9; r++)
    for (int c=0; c<9; c++)
      board[r][c] = '?';
}

/* add your functions here */

bool is_complete(char mines[9][9], char revealed[9][9]){
  for(int i=0; i<9; ++i){
    for(int j=0; j<9; ++j){
      if(revealed[i][j]=='?')
	return false;
      if(mines[i][j]=='*'){
	if(revealed[i][j]!=mines[i][j])
	  return false;
      }
    }
  }
  return true;
}

int count_mines(const char* position, char mines[9][9]){
  int i=position[0]-'A';
  int j=position[1]-'1';
  int count=0;
  if(i>0 && i<8 && j>0 && j<8){
    for(int m=i-1; m<=i-1; ++m){
      for(int n=j-1; n<=j-1; ++n){
	if(mines[m][n]=='*')
	  count++;
      }
    }
  }
  if(i==0&&j<8){
    for(int m=i; m<=i+1; ++m){
      for(int n=j; n<=j+1; ++n){
	if(mines[m][n]=='*')
	  count++;
      }
    }
    if(mines[i][j-1]=='*')
      count++;
    if(mines[i+1][j-1]=='*')
      count++;
  }
  return count;
}

MoveResult make_move(const char* position, char mines[9][9], char revealed[9][9]){
  int x=position[0]-'A';
  int y=position[1]-'1';
  char z;
  if(position[2]!='\0'){
    z=position[2];
    if(x<0|| x>8 || y<0 || y>8 ||z!='*')
      return INVALID_MOVE;
    if(isdigit(revealed[x][y]))
      return REDUNDANT_MOVE;
  }else{
    if(x<0 || x>8 || y<0 || y>8)
      return INVALID_MOVE;
    if(mines[x][y]!='*'){
      if(revealed[x][y]!='?' || isdigit(revealed[x][y]))
	return REDUNDANT_MOVE;
    }else
      return BLOWN_UP;
  }
  if(is_complete(mines, revealed))
    return SOLVED_BOARD;
  return VALID_MOVE;
}

bool find_safe_move(char revealed[9][9], char* move){
  char temp[512]={};
  char temp2[512]={};
  char flag='8';
  char mines[9][9];
  load_board("mines.dat", mines);
  for(int i=0; i<9; ++i){
    for(int j=0; j<9; ++j){
      if(revealed[i][j]=='?'){
	char x='A'+i;
	char y='1'+j;
	char z='*';
	string s1;
	s1=s1+x+y;
	string s2;
	s2=s2+x+y+z;
	if(make_move(s1.c_str(), mines, revealed)==VALID_MOVE){
	  strcpy(move, s1.c_str());
	  return true;
	}else if(make_move(s2.c_str(), mines, revealed)==VALID_MOVE){
	  strcpy(move, s2.c_str());
	  return true;
	}else
	  return false;
      }
    }
  }
  return false;
}
