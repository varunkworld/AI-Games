#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <climits>
#include "chess_board.h"
Board* ABMinMax(Board* board, short int depth_limit);
Board* ABMaxMove(Board* board, short int depth_limit, short int depth, int a, int b);
Board* ABMinMove(Board* board, short int depth_limit, short int depth, int a, int b);


Board* ABMinMax(Board* board, short int depth_limit) {
	return ABMaxMove(board, depth_limit, 1, 0, 0);
}
Board* ABMaxMove(Board* board, short int depth_limit, short int depth, int a, int b) {
	vector<Board*> *moves;
	Board* best_move = NULL;
	Board* best_real_move = NULL;
	Board* move = NULL;
	int alpha = a,beta = b;

	if (depth >= depth_limit) {//if depth limit is reached
		return board;
	} else {
		moves = board->list_all_moves();
		for (vector<Board*>::iterator it = moves->begin(); it != moves->end(); it++) {
			move = ABMinMove(*it, depth_limit, depth+1, alpha, beta);
			if (best_move == NULL || move->evaluate_board(Black) > best_move->evaluate_board(Black)) {
				best_move = move;
				best_real_move = *it;
				alpha = move->evaluate_board(Black);
			}
			if(beta > alpha){
				return best_real_move;
			}
		}
		return best_real_move;
	}
}
Board* ABMinMove(Board* board, short int depth_limit, short int depth, int a, int b) {
	vector<Board*> *moves;
	Board* best_move = NULL;
	Board* best_real_move = NULL;
	Board* move = NULL;
	int alpha = a,beta = b;

	if (depth >= depth_limit) {//if depth limit is reached
		return board;
	} else {
		moves = board->list_all_moves();
		for (vector<Board*>::iterator it = moves->begin(); it != moves->end(); it++) {
			move = ABMaxMove(*it, depth_limit, depth+1, alpha, beta);
			if (best_move == NULL || move->evaluate_board(White) < best_move->evaluate_board(White)) {
				best_move = move;
				best_real_move = *it;
				beta = move->evaluate_board(White);
			}
			if(beta < alpha){
				return best_real_move;
			}
		}
		return best_real_move;
	}
}

int main() {
	Board *b = new Board();
	Board *test;
	b->initialize();
	cout << "Board:" << endl;
	b->print();
	//	int i = 1;

	char src_x = 0, src_y = 0, dest_x = 0, dest_y = 0;


	while (true) {
		cout << "Enter your move: ";
		cin >> src_y >> src_x >> dest_y >> dest_x;
		cout << endl;
		//integerize
		src_x -= '1';
		dest_x -= '1';

		src_y -= 'a';
		dest_y -= 'a';

		test = b->move(src_x, src_y, dest_x, dest_y);
		if (test == NULL) {
			cout << "Invalid move. Try again." << endl << endl;
			continue;
		} else {
			b = test;
			b->print();
		}
		//cout << "White: " << b->evaluate_board(White) << endl;
		//cout << "Black: " << b->evaluate_board(Black) << endl;

		//enemy turn

		time_t start = time(NULL);
		b = ABMinMax(b,6);
		cout<< "passed time = "<< time(NULL) - start <<endl;

		if(b!=NULL){
			b->print();
		}
		else{
			cout<< "null dondu exiting"<<endl;
			break;
		}
	}

	return 0;
}
