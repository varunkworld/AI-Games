//==================================================================================
// Name        : main.cpp
// Author      : Ayush Agrawal & Varun Kumar
// E-mail      : agrawal.1@iitj.ac.in, kumar.2@iitj.ac.in
// Description : A Checkers game that runs minimax algorithm with alpha beta pruning
//==================================================================================

#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <sstream>
#include <time.h>
#include "checkers_board.h"

using namespace std;

int turn = -1;		//Minimax algorithm will run for White
class iMove {		//Intermediate Move class for Minimax algorithm 
  public:
  	Board* move;
  	int eval;
  iMove(Board* m, int e) {
  	eval = e;
    move = m;
  }
};

iMove* H_MinMax(Board* board, int limit);
iMove* H_Max_Value(Board* board, int limit, int depth, int a, int b);
iMove* H_Min_Value(Board* board, int limit, int depth, int a, int b);

iMove* H_MinMax(Board* board, int limit) {
	return H_Max_Value(board, limit, 1, INT_MIN, INT_MAX);
}

iMove* H_Max_Value(Board* board, int limit, int depth, int a, int b) {
	vector<Board*> *moves;
	iMove* val = new iMove(NULL,INT_MIN);
	iMove* move;
	int alpha = a,beta = b;

  	// depth has reached the limit or opponent is looser
	if (depth >= limit || board->looser(!turn)) {
		return new iMove(board, board->eval(turn));
	} else {
      	// get all possible moves
		moves = board->possible_moves();
		for (vector<Board*>::iterator it = moves->begin(); it != moves->end(); it++) {
			move = H_Min_Value(*it, limit, depth+1, alpha, beta);
			if (move->eval > val->eval) {
				val->move = *it;
            	val->eval = move->eval;
			}
			if(val->eval >= beta){
				return val;
			}
          	if(val->eval > alpha) {
				alpha = val->eval;
            }
		}
		return val;
	}
}

iMove* H_Min_Value(Board* board, int limit, int depth, int a, int b) {
	vector<Board*> *moves;
	iMove* val = new iMove(NULL,INT_MAX);
	iMove* move;
	int alpha = a,beta = b;

  // depth has reached the limit or opponent is looser
  if (depth >= limit || board->looser(turn)) {
		return new iMove(board, board->eval(turn));
	} else {
    	// get all possible moves
		moves = board->possible_moves();
		for (vector<Board*>::iterator it = moves->begin(); it != moves->end(); it++) {
			move = H_Max_Value(*it, limit, depth+1, alpha, beta);
			if (move->eval < val->eval) {
				val->move = *it;
            	val->eval = move->eval;
			}
			if(val->eval <= alpha){
				return val;
			}
          	if (val->eval < beta) {
          		beta = val->eval;
         	}
		}
	}
  return val;
}

vector<string> split(const string &s, char delim) {		//to split a string input
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

int main() {
  	iMove *m;
  	char choice;
  	int depth;
	Board *test, *b;
  	cout << "\n***************************************\n";
  	cout << "    Welcome to the GAME - CHECKERS !!  \n";
  	cout << "***************************************\n";
  	cout << "					- by Ayush & Varun\n";
  	cout << "Do you want forced jump (y/n) ? ";
    cin >> choice;
  	cout << "How much depth do you want (1-9) ? ";
    cin >> depth;
  	cin.ignore();
	cout << "Board:" << endl;
  	b = new Board(choice == 'y');
	b->print();
	Board *prev = new Board(b);
  
  /*vector<vector<int> > ss = 
  {
    {1,0,1,0,1,0,1,0},
    {0,1,0,1,0,1,0,1},
    {1,0,1,0,1,0,1,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,-1,0,-1,0,-1,0,-1},
    {-1,0,-1,0,-1,0,-1,0},
    {0,-1,0,-1,0,-1,0,-1}
  };*/
    vector<vector<int> > ss2 = 
  { {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,2,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,-2,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  };
  Board* tt = new Board();
  tt->board = ss2;
  tt->whites = 0;
  tt->blacks = 0;
  tt->white_kings = 1;
  tt->black_kings = 1;
  tt->turn = false;
  tt->enable_forced_jump = false;
	 {
      	b = tt;
        b->print();
      	cout << endl;
		cout << "White: " << b->eval(0) << endl;
		cout << "Black: " << b->eval(1) << endl;

      if(b->looser(0)) {
      	cout << " Black Wins !!!\n";
        //break;
      }
		
      	timespec t;
		clock_gettime(CLOCK_REALTIME, &t);
      	double start_time = ((t.tv_sec * 1000) + (t.tv_nsec / 1000000)) + 0.5;
		m = H_MinMax(b,depth);
      	b = m->move;
      	clock_gettime(CLOCK_REALTIME, &t);
		if(b!=NULL){
			b->print();
          cout<< "\nPassed Time = "<< ((((t.tv_sec * 1000) + (t.tv_nsec / 1000000)) + 0.5) - start_time)/1000.0 << " secs " << endl << endl;
			cout << endl;
            cout << "White: " << b->eval(0) << endl;
			cout << "Black: " << b->eval(1) << endl;
          	
		}
		else{
			cout<< "Error!!"<<endl;
			//break;
		}
        if(b->looser(1)) {
      		cout << " \nWhite Wins !!!\n";
          //break;
        }
      	cout << endl;
	}
	return 0;
}