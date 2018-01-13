#include <iostream>
#include <ctime>
#include <vector>
#include <stdlib.h>
using namespace std;

typedef vector< vector<bool> > Board;
typedef vector< vector<char> > helpBoard;
struct Pair{
    int row, col;
};
typedef vector<Pair> Ship;
typedef vector<Ship> Collection;
struct Status{
    bool random;
    Pair future_guess;
    bool vertical;   // cannot both be true
    bool horizontal; // cannot both be true
};

//defaults: BOARD_SIZE = 10, NUM_SHIPS = 10, MAX_SHIPS1 = 4, MAX_SHIPS2 = 3, MAX_SHIPS3 = 2, MAX_SHIPS4 = 1
const int BOARD_SIZE = 10; // between 8 and 26
const int NUM_SHIPS = 10; // has to be equal to sum(MAX_SHIPSi) from i = 1 to i = 4
const int MAX_SHIPS1 = 4;
const int MAX_SHIPS2 = 3;
const int MAX_SHIPS3 = 2;
const int MAX_SHIPS4 = 1;

//turn on to allow debugging
bool debug = false;

//DONT TOUCH THIS ONE
int winner = 0; //0 means game going, 1 means player wins, 2 means pc wins

bool check_correct(Board& aux, string s, string si, string dir, string& errorMsg, int& row, int& col);
bool check_correct_computer(Board& aux, int row, int col, int size, int dir);
void computer_ships(Board& C, Collection& computer);
int get_random(int minimum, int maximum);
void input_ships(Board& B, Collection& user);
bool inside_limits(int n);
bool is_number(char c);
void mark_around(Board& aux, int row, int col);
void modify_board(Board& B, Board& aux, int row, int col, int size, char direction);
void print_board(const Board& B);
void print_helpboard(const helpBoard& B);
void play_turn_user(const Board& C, helpBoard& game, Collection& computer);
bool check_guess(const helpBoard& game, string guess, int& row, int& col);
void play_turn_computer(const Board& B, Collection& user, vector<Pair>& AI, Status& status, helpBoard& gamePC, Board& AI_board, vector<Pair>& current_touched);
void load_ship(Collection& c, int row, int col, int size, char direction);
void delete_pos(Collection& c, int row, int col, bool& sunk);
void rem(Ship& s, int j);
void clear_ships(Collection& c);
void fill_ai_board(Board& AI_board, Status& status, Pair guess, bool touched, bool sunk, vector<Pair> current_touched);
void delete_from_AI(vector<Pair>& AI, Board AI_board);
void choose_direction(Pair current, Status& status, Board AI_board);
void choose_right_left(const vector<Pair>& current_touched, Status& status, Board AI_board);
void choose_up_down(const vector<Pair>& current_touched, Status& status, Board AI_board);
int maxrow(const vector<Pair>& v);
int minrow(const vector<Pair>& v);
int maxcol(const vector<Pair>& v);
int mincol(const vector<Pair>& v);
void print_helpboard_and_board(const helpBoard& B, const Board& C);
void print_collection(const Collection& c);

int main(){
    srand(time(NULL));
    vector<Pair> AI(BOARD_SIZE*BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE*BOARD_SIZE; i++){
        AI[i].row = i/BOARD_SIZE;
        AI[i].col = i%BOARD_SIZE;
    }
    Status status;
    status.random = true;
    status.horizontal = false;
    status.vertical = false;
    Board AI_board(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
    vector<Pair> current_touched;
    
    cout << "Welcome to Battleship!" << endl;
    
    Collection computer;
    Board C(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
    computer_ships(C, computer);
    
    Board B(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
    cout << "This is the board:" << endl << endl;
    print_board(B);
    
    Collection user;
    input_ships(B, user); 
    
    int turn = 1;
    helpBoard game(BOARD_SIZE, vector<char>(BOARD_SIZE, '.'));
    helpBoard gamePC(BOARD_SIZE, vector<char>(BOARD_SIZE, '.'));
    
    while (winner == 0){ //game loop
        cout << endl << "----------------" << endl << endl;
        cout << "TURN " << turn << ":" << endl << endl;
        
        cout << "Your guesses:" << endl << endl;
        print_helpboard(game);
        
        play_turn_user(C, game, computer);
        
        if (winner == 0){
            cout << endl << endl << "Computer's guesses         ";
            for (int i = 0; i < (BOARD_SIZE-6); i++) cout << "  ";
            cout << " Your board:" << endl << endl;
            print_helpboard_and_board(gamePC, B);
            play_turn_computer(B, user, AI, status, gamePC, AI_board, current_touched);
            
            turn++;
        }
    }
    cout << endl << endl << "GAME OVER" << endl << endl << endl;
    if (winner == 1){
        cout << endl;
        print_helpboard_and_board(game, C);
        cout << endl << endl << endl << "You won!" << endl;
    }
    else{
        cout << endl;
        print_helpboard_and_board(gamePC, B);
        cout << endl << endl << endl << "You lost..." << endl;
    }
}

int get_random(int minimum, int maximum){
    int n = rand()%(maximum-minimum) + minimum;
    return n;
}

bool is_number(char c){
    return (c >= '0' and c <= '9');
}

bool inside_limits(int n){
    if (n >= 0 and n < BOARD_SIZE) return true;
    return false;
}

void print_board(const Board& B){
    cout << "  ";
    for (int i = 0; i < BOARD_SIZE; i++){
        if (i < 10) cout << " ";
        cout << i+1;
    }
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++){
        cout << " " << char('a' + i) << " ";
        for (int j = 0; j < BOARD_SIZE; j++){
            if (j != 0) cout << ' ';
            if (B[i][j]) cout << 'X';
            else cout << '.';
        }
        cout << endl;
    }
}

void print_helpboard(const helpBoard& B){
    cout << "  ";
    for (int i = 0; i < BOARD_SIZE; i++){
        if (i < 10) cout << " ";
        cout << i+1;
    }
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++){
        cout << " " << char('a' + i) << " ";
        for (int j = 0; j < BOARD_SIZE; j++){
            if (j != 0) cout << ' ';
            cout << B[i][j];
        }
        cout << endl;
    }
}

void mark_around(Board& aux, int row, int col){
    aux[row][col] = true;
    if (inside_limits(row+1) and inside_limits(col+1)) aux[row+1][col+1] = true;
    if (inside_limits(row) and inside_limits(col+1)) aux[row][col+1] = true;
    if (inside_limits(row-1) and inside_limits(col+1)) aux[row-1][col+1] = true;
    if (inside_limits(row+1) and inside_limits(col)) aux[row+1][col] = true;
    if (inside_limits(row-1) and inside_limits(col)) aux[row-1][col] = true;
    if (inside_limits(row+1) and inside_limits(col-1)) aux[row+1][col-1] = true;
    if (inside_limits(row) and inside_limits(col-1)) aux[row][col-1] = true;
    if (inside_limits(row-1) and inside_limits(col-1)) aux[row-1][col-1] = true;
}

bool check_correct(Board& aux, string s, string si, string dir, string& errorMsg, int& row, int& col){
    if (si.length() > 1){
        errorMsg = "size is not between 1 and 4";
        return false;
    }
    int size = si[0] - '0';
    if (size != 1 and size != 2 and size != 3 and size != 4){ 
        errorMsg = "size is not between 1 and 4";
        return false;
    }
    if (dir.length() > 1){
        errorMsg = "direction is not v or h";
        return false;
    }
    char direction = dir[0];
    if (direction != 'v' and direction != 'h'){
        errorMsg = "direction is not v or h";
        return false;
    }
    errorMsg = "incorrect position";
    if (s.length() != 2 and s.length() != 3){
        return false;
    }
    if (s[0] < 'a' or s[0] > 'a'+BOARD_SIZE){
        return false;
    }
    row = s[0] - 'a';
    if (s[1] < '1' or s[1] > '9'){
        return false;
    }
    if (s.length() == 2) col = s[1] - '0' - 1;
    else{
        if (is_number(s[1]) and is_number(s[2]) and (10*(s[1]-'0') + (s[2]-'0')) <= BOARD_SIZE) col = 10*(s[1]-'0') + (s[2]-'0') - 1;
        else{
            return false;
        }
    }
    
    for (int i = 0; i < size; i++){
        if (direction == 'h'){
            if (not inside_limits(row) or not inside_limits(col+i) or aux[row][col+i]){
                return false;
            }
        }
        else{
            if (not inside_limits(row+i) or not inside_limits(col) or aux[row+i][col]){
                return false;
            }
        }
    }
    return true;
}

void modify_board(Board& B, Board& aux, int row, int col, int size, char direction){
    for (int i = 0; i < size; i++){
        if (direction == 'h'){
            B[row][col+i] = true;
            mark_around(aux, row, col+i);
        }
        else{
            B[row+i][col] = true;
            mark_around(aux, row+i, col);
        }
    }
}

void input_ships(Board& B, Collection& user){
    cout << endl;
    cout << "Input " << NUM_SHIPS << " ships in the following format: Leftmost/Uppermost position, size of the ship, 'h' for horizontal or 'v' for vertical. " << endl; 
    cout << "You have " << MAX_SHIPS1 << " ship(s) of size 1, " << MAX_SHIPS2 << " ship(s) of size 2, " << MAX_SHIPS3 << " ship(s) of size 3 and " << MAX_SHIPS4 << " ship(s) of size 4." << endl;
    cout << "If size is 1, input either h or v, it doesn't matter." << endl;
    cout << "Example: e5 3 h" << endl << endl;

    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    int count4 = 0;
    
    Board aux(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
    
    for (int i = 0; i < NUM_SHIPS; i++){
        string s;
        string si;
        string dir;
        bool correct = false;
        while (not correct){
            cout << "SHIP #" << i+1 << ": ";
            cin >> s >> si >> dir;
            string errorMsg;
            int row, col;
            correct = (check_correct(aux, s, si, dir, errorMsg, row, col));
            if (correct){
                int size = si[0]-'0';
                char direction = dir[0];
                if (size == 1){
                    if (count1 < MAX_SHIPS1) count1++;
                    else{
                        errorMsg = "too many ships of size 1";
                        correct = false;
                    }
                }
                if (size == 2){
                    if (count2 < MAX_SHIPS2) count2++;
                    else{
                        errorMsg = "too many ships of size 2";
                        correct = false;
                    }
                }
                else if (size == 3){
                    if (count3 < MAX_SHIPS3) count3++;
                    else{
                        errorMsg = "too many ships of size 3";
                        correct = false;
                    }
                }
                else if (size == 4){
                    if (count4 < MAX_SHIPS4) count4++;
                    else{
                        errorMsg = "too many ships of size 4";
                        correct = false;
                    }
                }
                if (correct){
                    load_ship(user, row, col, size, direction);
                    modify_board(B, aux, row, col, size, direction);
                    cout << endl;
                    print_board(B);
                    cout << endl;
                }
            }
            if (not correct) cout << errorMsg << endl;
        }
    }    
}

bool check_correct_computer(Board& aux, int row, int col, int size, int dir){
    for (int i = 0; i < size; i++){
        if (dir == 0){
            if (not inside_limits(row) or not inside_limits(col+i) or aux[row][col+i]){
                return false;
            }
        }
        else{
            if (not inside_limits(row+i) or not inside_limits(col+i) or aux[row+i][col]){
                return false;
            }
        }
    }
    return true;
}

void computer_ships(Board& C, Collection& computer){
    Board aux(BOARD_SIZE, vector<bool>(BOARD_SIZE, false));
    for (int i = 0; i < MAX_SHIPS4; i++){
        bool correct = false;
        while (not correct){
            int dir = get_random(0, 2);
            
            char d;
            if (dir == 0) d = 'h';
            else d = 'v';
            
            int row = get_random(0, BOARD_SIZE);
            int col = get_random(0, BOARD_SIZE);
            correct = (check_correct_computer(aux, row, col, 4, dir));
            if (correct){
                modify_board(C, aux, row, col, 4, d);
                load_ship(computer, row, col, 4, d);
            }
        }
    }
    for (int i = 0; i < MAX_SHIPS3; i++){
        bool correct = false;
        while (not correct){
            int dir = get_random(0, 2);
            
            char d;
            if (dir == 0) d = 'h';
            else d = 'v';
            
            int row = get_random(0, BOARD_SIZE);
            int col = get_random(0, BOARD_SIZE);
            correct = (check_correct_computer(aux, row, col, 3, dir));
            if (correct){
                modify_board(C, aux, row, col, 3, d);
                load_ship(computer, row, col, 3, d);
            }
        }
    }
    for (int i = 0; i < MAX_SHIPS2; i++){
        bool correct = false;
        while (not correct){
            int dir = get_random(0, 2);
            
            char d;
            if (dir == 0) d = 'h';
            else d = 'v';
            
            int row = get_random(0, BOARD_SIZE);
            int col = get_random(0, BOARD_SIZE);
            correct = (check_correct_computer(aux, row, col, 2, dir));
            if (correct){
                modify_board(C, aux, row, col, 2, d);
                load_ship(computer, row, col, 2, d);
            }
        }
    }
    for (int i = 0; i < MAX_SHIPS1; i++){
        bool correct = false;
        while (not correct){
            int dir = get_random(0, 2);
            
            char d;
            if (dir == 0) d = 'h';
            else d = 'v';
            
            int row = get_random(0, BOARD_SIZE);
            int col = get_random(0, BOARD_SIZE);
            correct = (check_correct_computer(aux, row, col, 1, dir));
            if (correct){
                modify_board(C, aux, row, col, 1, d);
                load_ship(computer, row, col, 1, d);
            }
        }
    }
}

void play_turn_user(const Board& C, helpBoard& game, Collection& computer){
    int row, col;
    bool cont = true;
    while (cont){
        bool correct = false;
        while (not correct){
            string pos;
            cout << endl << endl << "Input position (Format example: e4): ";
            cin >> pos;
            correct = check_guess(game, pos, row, col);
            if (not correct) cout << "incorrect position or already guessed" << endl;
        }
        cout << endl;
        if (C[row][col]){
            cout << "Touched!" << endl;
            game[row][col] = 'X';
            bool sunk = false;
            delete_pos(computer, row, col, sunk);
            if (sunk){
                cout << "Sunk!!" << endl;
                clear_ships(computer);
                if (computer.size() == 0){
                    winner = 1;
                    return;
                }
            }
            cout << endl << "Your guesses:" << endl << endl;
            print_helpboard(game);
            //cout << endl << "COLLECTION: " << endl;
            //print_collection(computer);

        }
        else{
            cout << "Water" << endl;
            game[row][col] = 'O';
            cont = false;
        }
    }
}

bool check_guess(const helpBoard& game, string guess, int& row, int& col){
    if (guess.length() > 3 or guess.length() < 2) return false;
    if (guess[0] < 'a' or guess[0] > 'a'+BOARD_SIZE) return false;
    row = guess[0]-'a';
    if (guess.length() == 2){
        if (not is_number(guess[1]) or guess[1]-'0' > BOARD_SIZE) return false;
        col = guess[1] - '0' -1;
    }
    if (guess.length() == 3){
        if (not is_number(guess[1]) or not is_number(guess[2])) return false;
        if (10*(guess[1]-'0') + guess[2]-'0' > BOARD_SIZE) return false;
        col = 10*(guess[1]-'0') + guess[2]-'0' -1;
    }
    if (game[row][col] != '.'){
        return false;
    }
    return true;
}

void load_ship(Collection& c, int row, int col, int size, char direction){
    Ship s;
    for(int i = 0; i < size; i++){
        Pair a;
        if(direction == 'h'){
            a.row = row;
            a.col = col+i;
        }
        else{
            a.row = row+i;
            a.col = col; 
        }
        s.push_back(a);
    }
    c.push_back(s);
}

void delete_pos(Collection& c, int row, int col, bool& sunk){
    int size = c.size();
    for (int i = 0; i < size; i++){
        int size2 = c[i].size();
        for (int j = 0; j < size2; j++){
            if (c[i][j].row == row and c[i][j].col == col){
                rem(c[i], j);
                if (c[i].size() == 0) sunk = true;
            }
        }
    }
}

void rem(Ship& s, int j){
    int size = s.size();
    s[j] = s[size-1];
    s.pop_back();
}

void clear_ships(Collection& c){
    int size = c.size();
    for(int i = 0; i < size; i++){
        if (c[i].size() == 0){
            c[i] = c[size-1];
            c.pop_back();
            return;
        }
    }
}

//TOFIX
void play_turn_computer(const Board& B, Collection& user, vector<Pair>& AI, Status& status, helpBoard& gamePC, Board& AI_board, vector<Pair>& current_touched){
    int row, col;
    int guess, max;
    bool cont = true;
    while (cont){
        if (status.random){
            max = AI.size();
            guess = get_random(0, max);
            status.future_guess.row = AI[guess].row;
            status.future_guess.col = AI[guess].col;
        }
        else{
            if (current_touched.size() == 1){
                choose_direction(current_touched[0], status, AI_board);
            }
            else if (status.horizontal){
                choose_right_left(current_touched, status, AI_board);
            }
            else{
                choose_up_down(current_touched, status, AI_board);
            }
        }
        
        row = status.future_guess.row;
        col = status.future_guess.col;
        
        
        
        
        
        //TESTING FROM HERE
        
        if (debug){
        
            cout << endl << endl;
            
            cout << "my currently touched positions are:";
            int size = current_touched.size();
            for (int i = 0; i < size; i++) cout << " " << char(current_touched[i].row + 'a') << current_touched[i].col + 1;
            cout << endl;
            cout << "my status is: ";
            if (status.vertical and status.horizontal) cout << "both, ERROR";
            else if (status.horizontal) cout << "horizontal";
            else if (status.vertical) cout << "vertical";
            else cout << "none";
            cout << ", ";
            if (status.random) cout << "random";
            else cout << "not random";
            cout << endl;
            cout << "my helping board is:" << endl;
            print_board(AI_board);
        
        }
        
        //TESTING TO HERE
        
        
        
        
        cout << endl << endl;
        cout << "The computer has guessed the position " << char('a'+row) << col+1 << "." << endl << endl;
        bool touched = B[row][col];
        bool sunk = false;
        if (touched){
            status.random = false;
            cout << "Touched!" << endl;
            gamePC[row][col] = 'X';
            delete_pos(user, row, col, sunk);
            
            Pair touch;
            touch.row = row;
            touch.col = col;
            current_touched.push_back(touch); //updating currently touched positions
            
            if (sunk){
                cout << "Sunk!!" << endl;
                clear_ships(user);
                if (user.size() == 0){
                    winner = 2;
                    return;
                }
                status.random = true; 
            }
        }
        else{
            cout << "Water" << endl;
            gamePC[row][col] = 'O';
            cont = false;
        }

        Pair guessed;
        guessed.row = row;
        guessed.col = col;
        
        //UPDATING THE BOARD
        fill_ai_board(AI_board, status, guessed, touched, sunk, current_touched);
        
        //FOR ALL POSITIONS IN AI_board THAT ARE TRUE, DELETE THEM FROM AI
        delete_from_AI(AI, AI_board);
        
        //UPDATING STUFF
        int s = current_touched.size(); 
        if (sunk){ //emptying currently touched positions
            for (int i = 0; i < s; i++) current_touched.pop_back(); 
        }
        if (current_touched.size() == 1) choose_direction(current_touched[0], status, AI_board);
    }
}

void fill_ai_board(Board& AI_board, Status& status, Pair guess, bool touched, bool sunk, vector<Pair> current_touched){
    AI_board[guess.row][guess.col] = true;
    if (sunk){
        int size = current_touched.size();
        for (int i = 0; i < size; i++){
            mark_around(AI_board, current_touched[i].row, current_touched[i].col);
        }
        status.vertical = false;
        status.horizontal = false;
    }
    else if (current_touched.size() > 1){
        if (current_touched[0].row == current_touched[1].row) status.horizontal = true;
        else status.vertical = true;
    }

}

void delete_from_AI(vector<Pair>& AI, Board AI_board){
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j = 0; j < BOARD_SIZE; j++){
            if (AI_board[i][j]){
                bool found = false;
                int size = AI.size();
                for (int k = 0; k < size and not found; k++){
                    if (AI[k].row == i and AI[k].col == j){
                        AI[k] = AI[size-1]; //Remove the position from AI
                        AI.pop_back();
                        found = true;
                    }
                }
            }
        }
    }
}

void choose_direction(Pair current, Status& status, Board AI_board){ //WHEN YOU ONLY HAVE ONE TOUCHED POS
    bool correct = false;
    while (not correct){
        int dir = get_random(0, 4);
        if (dir == 0){ //RIGHT
            if (inside_limits(current.col + 1) and not AI_board[current.row][current.col + 1]){
                status.future_guess.row = current.row;
                status.future_guess.col = current.col + 1;
                correct = true;
            }
        }
        if (dir == 1){ //UP
            if (inside_limits(current.row - 1) and not AI_board[current.row - 1][current.col]){
                status.future_guess.row = current.row - 1;
                status.future_guess.col = current.col;
                correct = true;
            }
        }
        if (dir == 2){ //LEFT
            if (inside_limits(current.col - 1) and not AI_board[current.row][current.col - 1]){
                status.future_guess.row = current.row;
                status.future_guess.col = current.col - 1;
                correct = true;
            }
        }
        if (dir == 3){ //DOWN
            if (inside_limits(current.row + 1) and not AI_board[current.row + 1][current.col]){
                status.future_guess.row = current.row + 1;
                status.future_guess.col = current.col;
                correct = true;
            }
        }
    }    
}

void choose_right_left(const vector<Pair>& current_touched, Status& status, Board AI_board){ //when horizontal - 0 left, 1 right
    int row = current_touched[0].row;
    int col;
    int leftmost = mincol(current_touched);
    int rightmost = maxcol(current_touched);
    int dir = get_random(0, 1);
    if (dir == 0){
        if (inside_limits(leftmost-1) and not AI_board[row][leftmost-1]){
            col = leftmost-1;
        }
        else{
            col = rightmost+1;
        }
    }
    else if (dir == 1){
        if(inside_limits(rightmost+1) and not AI_board[row][rightmost+1]){
            col = rightmost+1;
        }
        else{
            col = leftmost-1;
        }
    }
    status.future_guess.row = row;
    status.future_guess.col = col;
}

void choose_up_down(const vector<Pair>& current_touched, Status& status, Board AI_board){ //when vertical - 0 down, 1 up
    int row;
    int col = current_touched[0].col;
    int upmost = minrow(current_touched);
    int downmost = maxrow(current_touched);
    int dir = get_random(0, 1);
    if (dir == 0){
        if (inside_limits(upmost-1) and not AI_board[upmost-1][col]){
            row = upmost-1;
        }
        else{
            row = downmost+1;
        }
    }
    else if (dir == 1){
        if(inside_limits(downmost+1) and not AI_board[downmost+1][col]){
            row = downmost+1;
        }
        else{
            row = upmost-1;
        }
    }
    status.future_guess.row = row;
    status.future_guess.col = col;
}

int mincol(const vector<Pair>& v){
    int min = BOARD_SIZE + 1;
    int size = v.size();
    for (int i = 0; i < size; i++){
        if (v[i].col < min) min = v[i].col;
    }
    return min;
}

int maxcol(const vector<Pair>& v){
    int max = -1;
    int size = v.size();
    for (int i = 0; i < size; i++){
        if (v[i].col > max) max = v[i].col;
    }
    return max;
}

int minrow(const vector<Pair>& v){
    int min = BOARD_SIZE + 1;
    int size = v.size();
    for (int i = 0; i < size; i++){
        if (v[i].row < min) min = v[i].row;
    }
    return min;
}

int maxrow(const vector<Pair>& v){
    int max = -1;
    int size = v.size();
    for (int i = 0; i < size; i++){
        if (v[i].row > max) max = v[i].row;
    }
    return max;
}

void print_collection(const Collection& c){
    int size1 = c.size();
    for (int i = 0; i < size1; i++){
        int size2 = c[i].size();
        for (int j = 0; j < size2; j++){
            if (j != 0) cout << " ";
            cout << char(c[i][j].row + 'a') << c[i][j].col + 1; 
        }
        cout << endl;
    }
}

void print_helpboard_and_board(const helpBoard& B, const Board& C){
    cout << "  ";
    for (int i = 0; i < BOARD_SIZE; i++){
        if (i < 10) cout << " ";
        cout << i+1;
    }
    cout << "               ";
    for (int i = 0; i < BOARD_SIZE; i++){
        if (i < 10) cout << " ";
        cout << i+1;
    }
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++){
        cout << " " << char('a' + i) << " ";
        for (int j = 0; j < BOARD_SIZE; j++){
            if (j != 0) cout << ' ';
            cout << B[i][j];
        }
        cout << "             ";
        cout << " " << char('a' + i) << " ";
        for (int j = 0; j < BOARD_SIZE; j++){
            if (j != 0) cout << ' ';
            if (C[i][j]) cout << 'X';
            else cout << '.'; 
        }
        cout << endl;
    }
}
