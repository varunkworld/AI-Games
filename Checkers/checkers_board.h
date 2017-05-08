//==================================================================================
// Name        : checkers_board.h
// Author      : Ayush Agrawal & Varun Kumar
// E-mail      : agrawal.1@iitj.ac.in, kumar.2@iitj.ac.in
// Description : A Checkers game that runs minimax algorithm with alpha beta pruning
//==================================================================================

#include <cmath>
#include <vector>

using namespace std;

class Board {
  vector<vector<int> > board; //Board State
  int blacks; //Number of Black Men
  int whites; //Number of White Men
  int white_kings;  //Number of White Kings
  int black_kings;  //Number of Black kings
  bool turn;  //"true" denotes Black's turn and "false" denotes White's turn
  bool enable_forced_jump;  //Toggling forced jump option
public:
  
  Board(bool e = true) {  // default value for forced jump is true
      //allocate the vector
      board.resize(8);
      turn = true;  //Black's turn at the beginning
    
      // initialise the board onfiguration
      blacks = whites = 12;       // number of black & white men
      black_kings = white_kings = 0;  // number of white & black kings
    
      // all places are empty
      for (int i = 0; i < 8; ++i) {
          board[i].resize(8);
          for (int j = 0; j < 8; ++j) {
              board[i].push_back(0);
          }
      }
    
      // put black & white pieces on the board
      for(int i = 0; i < 4;i++) {
          board[0][2*i] = 1;
          board[1][2*i+1] = 1;
          board[2][2*i] = 1;
          board[5][2*i+1] = -1;
          board[6][2*i] = -1;
          board[7][2*i+1] = -1;

      }
    
      enable_forced_jump = e;
  }
  
  //copy constructor
  Board(Board *b) {
      board = b->board;
      blacks = b->blacks;
      whites = b->whites;
      white_kings = b->white_kings;
      black_kings = b->black_kings;
      turn = b->turn;
      enable_forced_jump = b->enable_forced_jump;
  }
  
  void print() {
      cout << "  A B C D E F G H \n";
      for(int i = 7;i >= 0; i--)
      {
          cout<<i+1<<"|";
          for(int j=0;j<8;j++)
          {
              switch(board[i][j])
              {
                  case 2:  cout<<"B|";break;  //Black King
                  case 1:  cout<<"b|";break;  //Black Man
                  case -1: cout<<"w|";break;  //White Man
                  case -2: cout<<"W|";break;  //White King
                default: ((i+j)%2) == 0 ? cout<<".|" : cout<<" |";
              }
          }
          cout<<endl;
      }
  }
  
  // move piece from src to dest after checking the validity
  Board* move_piece(int src_x, int src_y, int dest_x, int dest_y) {
    Board* temp(this);
    temp->board[dest_x][dest_y] = temp->board[src_x][src_y];  // put src piece on dest
    if(abs(dest_x-src_x) == 2 && abs(dest_y-src_y) == 2) {    // checking whether it's a jump move
      if(!turn) { // white turn
        temp->board[(src_x+dest_x)/2][(src_y+dest_y)/2] == 1 ? temp->blacks-- : temp->black_kings--;
      } else {    // black turn
        temp->board[(src_x+dest_x)/2][(src_y+dest_y)/2] == -1 ? temp->whites-- : temp->white_kings--;
      }
      temp->board[(src_x+dest_x)/2][(src_y+dest_y)/2] = 0;
    }
    temp->board[src_x][src_y] = 0;
    if(turn && dest_x == 7) { // black reached uppermost position of the board
      if(temp->board[dest_x][dest_y] == 1) {  // 
        temp->black_kings++;  
        temp->blacks--;
      }
      temp->board[dest_x][dest_y] = 2;  // change it to black king
    } else if(!turn && dest_x == 0) { // white reached lowermost position of the board
      if(temp->board[dest_x][dest_y] == -1) {
        temp->white_kings++;
        temp->whites--;
      }
      temp->board[dest_x][dest_y] = -2; // change it to white king
    }
    return temp;
  }
  
  // for single jump or diagonal move
  Board* single_move(int src_x, int src_y, int dest_x, int dest_y) {
    // out of range of the board
    if(src_x<0||src_x>7||src_y<0||src_y>7||dest_x<0||dest_x>7||dest_y<0||dest_y>7) {
      return NULL;
    }
    // source is empty
    if(board[src_x][src_y] == 0) {
      return NULL;
    }
    // source and destination are same
    if(src_x == dest_x && src_y == dest_y) {
      return NULL;
    }
    if(!turn && board[src_x][src_y] > 0) {      // if turn is white and source is white
      return NULL;
    } else if(turn && board[src_x][src_y] < 0) {  // if turn is black and source is black
      return NULL;
    }
    int sign = turn ? 1 : -1; // direction of the move according to the turn
    sign = abs(board[src_x][src_y]) == 2 ? 0 : sign; 
    if(sign*(dest_x-src_x) < 0) { // direction is opposite to the actual movement 
      return NULL;
    }
    if(board[dest_x][dest_y] != 0) {  // destination is non empty
      return NULL;
    }

    // checking jumping move and middle piece is of different color
    if(abs(dest_x-src_x) == 2 && abs(dest_y-src_y) == 2 && board[(src_x+dest_x)/2][(src_y+dest_y)/2]*board[src_x][src_y] < 0) {
      return move_piece(src_x,src_y,dest_x,dest_y);
    } else if(abs(dest_x-src_x) == 1 && abs(dest_y-src_y) == 1) { // ordinary move
      return move_piece(src_x,src_y,dest_x,dest_y);
    }
    return NULL;
  }
  
 // multiple moves by user; input is pair of vectors containing positions of the move
  Board* multiple_move_user(vector<int> x, vector<int> y) {   
    if(x.size() != y.size()) {
      return NULL;
    }
    bool forced_jump = false;
    if(enable_forced_jump) {
      forced_jump = check_forced_jump();  //checking if player has to "force jump"
    }
    if(x.size() == 2 && abs(x[1]-x[0]) == 1 && abs(y[1]-y[0]) == 1 && !forced_jump) { // move is non-jumping move & is single stepped
      Board* temp = single_move(x[0],y[0],x[1],y[1]);
      if(temp != NULL) {
        temp->turn = !(this->turn);
      }
      return temp;
    }
    
    Board* temp = new Board(this);
    int i = 0;
    for(; i < (x.size()-1); i++) {    //move is jumping move and may be multiple stepped
      if(abs(x[i]-x[i+1]) != 2 || abs(y[i]-y[i+1]) != 2) {
        return NULL;
      }
      if( ((board[x[i]][y[i]] == 1 && x[i+1] == 7) || (board[x[i]][y[i]] == -1 && x[i+1] == 0)) && i+1 != x.size()-1 ) { // black/white man becomes king
        return NULL;
      }
      temp = temp->single_move(x[i],y[i],x[i+1],y[i+1]);
      
      if(temp == NULL)
        return NULL;
    }
    if((board[x[0]][y[0]] == 1 && x[i] == 7) || (board[x[0]][y[0]] == -1 && x[i] == 0)) {
      return temp;
    }
    Board* temp2 = new Board(temp);
    //Checking if more jumps are possible from current location
    if(temp2->single_move(x[i],y[i],x[i]+2,y[i]+2) != NULL)
      return NULL;
    if(temp2->single_move(x[i],y[i],x[i]-2,y[i]+2) != NULL)
      return NULL;
    if(temp2->single_move(x[i],y[i],x[i]+2,y[i]-2) != NULL)
      return NULL;
    if(temp2->single_move(x[i],y[i],x[i]-2,y[i]-2) != NULL)
      return NULL;
    temp->turn = !(this->turn);
    return temp;
  }
  
  // dfs for checking the number of final jump moves that one can take
  void dfs(Board* b, int src_x, int src_y, vector<Board*> &m) {
    int flag = 0;

    // up-right
    Board* temp = new Board(b);
    temp = temp->single_move(src_x,src_y,src_x+2,src_y+2);
    if(temp == NULL) {
        flag++;
    } else if(b->board[src_x][src_y] == 1 && src_x+2 == 7) {  // black man becomes king
        temp->turn = !(this->turn);
        m.push_back(temp);
    } else {
        temp = new Board(temp);
        dfs(temp,src_x+2,src_y+2,m);
    }
  // down-right
    temp = new Board(b);
    temp = temp->single_move(src_x,src_y,src_x-2,src_y+2);
    if(temp == NULL) {
      flag++;
    } else if(b->board[src_x][src_y] == -1 && src_x-2 == 0) { // white man becomes king
        temp->turn = !(this->turn);
        m.push_back(temp);
    } else {
      temp = new Board(temp);
      dfs(temp,src_x-2,src_y+2,m);
    }
  // down-left
    temp = new Board(b);
    temp = temp->single_move(src_x,src_y,src_x-2,src_y-2);
    if(temp == NULL) {
        flag++;
    } else if(b->board[src_x][src_y] == -1 && src_x-2 == 0) { // white man becomes king
        temp->turn = !(this->turn);
        m.push_back(temp);
    } else {
      temp = new Board(temp);
        dfs(temp,src_x-2,src_y-2,m);
    }
  // up-left
    temp = new Board(b);
    temp = temp->single_move(src_x,src_y,src_x+2,src_y-2);
    if(temp == NULL) {
      flag++;
    } else if(b->board[src_x][src_y] == 1 && src_x+2 == 7) {  // black man becomes king
        temp->turn = !(this->turn);
        m.push_back(temp);
    } else {
      temp = new Board(temp);
      dfs(temp,src_x+2,src_y-2,m);
    }
    // goes in none of the above if loops
    if(flag == 4) {
      b->turn = !(this->turn);
      m.push_back(b);
    }
  }
  
  // moving computer's piece from src to dest and return list of the necessary possible moves from dest
  vector<Board*> multiple_move_comp(int src_x, int src_y, int dest_x, int dest_y, bool forced_jump) {
    vector<Board*> moves;
    bool flag = false;
    if(abs(board[src_x][src_y]) == 1) {
      flag = true;
    }
    Board* temp = single_move(src_x,src_y,dest_x,dest_y); //performing the move in the input
    if(temp == NULL) {
      return moves;
    }
    if(abs(dest_x-src_x) == 1 && abs(dest_y-src_y) == 1 && !forced_jump) {  // if move is non-jumping then return this move only
        temp = new Board(temp);
        temp->turn = !(this->turn);
        moves.push_back(temp);
    } else if(abs(dest_x-src_x) == 2 && abs(dest_y-src_y) == 2){  // if it is jumping move then explore all the moves that can be taken from dest
      temp = new Board(temp);
      if(flag && (dest_x == 0 || dest_x == 7)) {  // black/white man becomes king
        temp->turn = !(this->turn);
        moves.push_back(temp);
      } else {
        dfs(temp,dest_x,dest_y,moves);
      }
    }
    return moves;
  }
  
  // getting all moves that a computer can take
  vector<Board*>* possible_moves() {
    vector<Board*> *moves = new vector<Board*>();
    vector<Board*> temp;
    bool forced_jump = false;
    if(enable_forced_jump) {
      forced_jump = check_forced_jump();
    }
    //checking all moves for this board for computer
    for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        if((turn?1:-1)*board[i][j]>0) {
          for(int k = 0; k < 8; k++) {
            for(int l = 0; l < 8; l++) {
              // src === dest
              if(i == k && j == l) {
                continue;
              }
              Board* t = new Board(this);
              temp = t->multiple_move_comp(i,j,k,l,forced_jump);
              // checking and appending possible moves to moves vector from src
              if(temp.size() != 0) {
                moves->insert(moves->end(), temp.begin(), temp.end());
              }
            }
          }
        }
      }
    }
    return moves;
  }
  
  // checking whether that player t is a loser or not
  bool loser(int t) {
      for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        if((t?1:-1)*board[i][j]>0) {
          for(int k = 0; k < 8; k++) {
            for(int l = 0; l < 8; l++) {
              // src == dest
              if(i == k && j == l) {
                continue;
              }
              Board* t = new Board(this);
              // if there is move available then that player is not a loser
              if(t->single_move(i,j,k,l) != NULL ) {
                return false;
              }
            }
          }
        }
      }
    }
    return true;
  }
  
  // checking whether there is a jump available to that player's turn
  bool check_forced_jump()
  {
    for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        if((turn?1:-1)*board[i][j]>0) {
          for(int k = 0; k < 8; k++) {
            for(int l = 0; l < 8; l++) {
              if(abs(i-k) != 2 || abs(j-l) != 2) {
                continue;
              }
              Board* t = new Board(this);
              if(t->single_move(i,j,k,l) != NULL) {
                return true;
              }
            }
          }
        }
      }
    }
    return false;
  }
  
  // eval/cut-off value
  int eval(int color) {
    int black_eval = 0, white_eval = 0;
    black_eval += (1*blacks);
    black_eval += (2*black_kings);
    white_eval += (1*whites);
    white_eval += (2*white_kings);
    return color==1 ? (black_eval - white_eval) : (white_eval - black_eval);
  }
};