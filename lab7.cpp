#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>

using namespace std;


class GameSettings { // Singleton 
private:
    static GameSettings* instance;

    GameSettings() {
        size = 9;
        boxSize = 3;
        emptyCell = 0;
    }

public:
    int size;
    int boxSize;
    int emptyCell;

    static GameSettings* getInstance() {
        if (instance == nullptr) {
            instance = new GameSettings();
        }
        return instance;
    }

    static void destroy() {
        delete instance;
        instance = nullptr;
    }
};

GameSettings* GameSettings::instance = nullptr;

class SudokuBoard {
private:
    vector<vector<int>> board;
    vector<vector<int>> startBoard;

public:
    SudokuBoard() {
        GameSettings* settings = GameSettings::getInstance();
        board.assign(settings->size, vector<int>(settings->size, settings->emptyCell));
        startBoard = board;
    }

    SudokuBoard(const vector<vector<int>>& puzzle) {
        board = puzzle;
        startBoard = puzzle;
    }

    int getValue(int row, int col) const {
        return board[row][col];
    }

    void setValue(int row, int col, int value) {
        board[row][col] = value;
    }

    bool isStartCell(int row, int col) const {
        return startBoard[row][col] != GameSettings::getInstance()->emptyCell;
    }

    bool isEmpty(int row, int col) const {
        return board[row][col] == GameSettings::getInstance()->emptyCell;
    }

    vector<vector<int>> getState() const {
        return board;
    }

    void setState(const vector<vector<int>>& state) {
        board = state;
    }

    void restart() {
        board = startBoard;
    }

    bool isFull() const {
        GameSettings* settings = GameSettings::getInstance();

        for (int row = 0; row < settings->size; row++) {
            for (int col = 0; col < settings->size; col++) {
                if (board[row][col] == settings->emptyCell) {
                    return false;
                }
            }
        }

        return true;
    }

    void print() const {
        GameSettings* settings = GameSettings::getInstance();

        cout << endl;
        cout << "    1 2 3   4 5 6   7 8 9" << endl;
        cout << "  +-------+-------+-------+" << endl;

        for (int row = 0; row < settings->size; row++) {
            cout << row + 1 << " | ";

            for (int col = 0; col < settings->size; col++) {
                if (board[row][col] == settings->emptyCell) {
                    cout << ". ";
                }
                else {
                    cout << board[row][col] << " ";
                }

                if ((col + 1) % settings->boxSize == 0) {
                    cout << "| ";
                }
            }

            cout << endl;

            if ((row + 1) % settings->boxSize == 0) {
                cout << "  +-------+-------+-------+" << endl;
            }
        }

        cout << endl;
    }
};

class BoardMemento { // Memento 
private:
    vector<vector<int>> state;

public:
    BoardMemento(const vector<vector<int>>& boardState) {
        state = boardState;
    }

    vector<vector<int>> getState() const {
        return state;
    }
};

class ValidationStrategy { // Strategy  
public:
    virtual bool canPlace(const SudokuBoard& board, int row, int col, int value) = 0;
    virtual bool isSolved(const SudokuBoard& board) = 0;
    virtual ~ValidationStrategy() {}
};

class ClassicSudokuValidation : public ValidationStrategy {
private:
    bool checkRow(const SudokuBoard& board, int row, int value) {
        GameSettings* settings = GameSettings::getInstance();

        for (int col = 0; col < settings->size; col++) {
            if (board.getValue(row, col) == value) {
                return false;
            }
        }

        return true;
    }

    bool checkColumn(const SudokuBoard& board, int col, int value) {
        GameSettings* settings = GameSettings::getInstance();

        for (int row = 0; row < settings->size; row++) {
            if (board.getValue(row, col) == value) {
                return false;
            }
        }

        return true;
    }

    bool checkBox(const SudokuBoard& board, int row, int col, int value) {
        GameSettings* settings = GameSettings::getInstance();

        int startRow = row - row % settings->boxSize;
        int startCol = col - col % settings->boxSize;

        for (int r = 0; r < settings->boxSize; r++) {
            for (int c = 0; c < settings->boxSize; c++) {
                if (board.getValue(startRow + r, startCol + c) == value) {
                    return false;
                }
            }
        }

        return true;
    }

public:
    bool canPlace(const SudokuBoard& board, int row, int col, int value) override {
        if (value < 1 || value > 9) {
            return false;
        }

        if (board.isStartCell(row, col)) {
            return false;
        }

        int oldValue = board.getValue(row, col);

        SudokuBoard tempBoard = board;
        tempBoard.setValue(row, col, GameSettings::getInstance()->emptyCell);

        return checkRow(tempBoard, row, value)
            && checkColumn(tempBoard, col, value)
            && checkBox(tempBoard, row, col, value)
            && oldValue != value;
    }

    bool isSolved(const SudokuBoard& board) override {
        GameSettings* settings = GameSettings::getInstance();

        if (!board.isFull()) {
            return false;
        }

        for (int row = 0; row < settings->size; row++) {
            for (int col = 0; col < settings->size; col++) {
                int value = board.getValue(row, col);

                SudokuBoard tempBoard = board;
                tempBoard.setValue(row, col, settings->emptyCell);

                if (!checkRow(tempBoard, row, value)
                    || !checkColumn(tempBoard, col, value)
                    || !checkBox(tempBoard, row, col, value)) {
                    return false;
                }
            }
        }

        return true;
    }
};

class GameObserver { // Observer
public:
    virtual void update(const string& message) = 0;
    virtual ~GameObserver() {}
};

class ConsoleGameObserver : public GameObserver {
public:
    void update(const string& message) override {
        cout << "[Message] " << message << endl;
    }
};

class MoveCommand { // Command 
private:
    SudokuBoard* board;
    int row;
    int col;
    int value;
    int previousValue;

public:
    MoveCommand(SudokuBoard* board, int row, int col, int value) {
        this->board = board;
        this->row = row;
        this->col = col;
        this->value = value;
        this->previousValue = board->getValue(row, col);
    }

    void execute() {
        board->setValue(row, col, value);
    }

    void undo() {
        board->setValue(row, col, previousValue);
    }
};

class SudokuFactory { // Factory Method
public:
    virtual SudokuBoard createBoard() = 0;
    virtual ~SudokuFactory() {}
};

class EasySudokuFactory : public SudokuFactory { 
public:
    SudokuBoard createBoard() override {
        vector<vector<int>> puzzle = {
            {5, 3, 0, 0, 7, 0, 0, 0, 0},
            {6, 0, 0, 1, 9, 5, 0, 0, 0},
            {0, 9, 8, 0, 0, 0, 0, 6, 0},

            {8, 0, 0, 0, 6, 0, 0, 0, 3},
            {4, 0, 0, 8, 0, 3, 0, 0, 1},
            {7, 0, 0, 0, 2, 0, 0, 0, 6},

            {0, 6, 0, 0, 0, 0, 2, 8, 0},
            {0, 0, 0, 4, 1, 9, 0, 0, 5},
            {0, 0, 0, 0, 8, 0, 0, 7, 9}
        };

        return SudokuBoard(puzzle);
    }
};

class SudokuGame {
private:
    SudokuBoard board;
    unique_ptr<ValidationStrategy> validation;
    vector<GameObserver*> observers;
    vector<MoveCommand> history;
    vector<BoardMemento> saves;
    bool running;

    void notify(const string& message) {
        for (GameObserver* observer : observers) {
            observer->update(message);
        }
    }

    void printRules() {
        cout << "Commands:" << endl;
        cout << "  set r c v  - put number v into row r and column c" << endl;
        cout << "  clear r c  - clear a cell" << endl;
        cout << "  undo       - undo last move" << endl;
        cout << "  save       - save current state" << endl;
        cout << "  load       - load last saved state" << endl;
        cout << "  restart    - restart the game" << endl;
        cout << "  help       - show commands" << endl;
        cout << "  exit       - exit" << endl;
        cout << endl;
    }

    bool correctCoordinates(int row, int col) {
        return row >= 1 && row <= 9 && col >= 1 && col <= 9;
    }

public:
    SudokuGame(SudokuFactory* factory) {
        board = factory->createBoard();
        validation = make_unique<ClassicSudokuValidation>();
        running = true;
    }

    void addObserver(GameObserver* observer) {
        observers.push_back(observer);
    }

    void setNumber(int row, int col, int value) {
        if (!correctCoordinates(row, col)) {
            notify("Incorrect cell coordinates.");
            return;
        }

        row--;
        col--;

        if (board.isStartCell(row, col)) {
            notify("You cannot change a fixed cell.");
            return;
        }

        if (validation->canPlace(board, row, col, value)) {
            MoveCommand command(&board, row, col, value);
            command.execute();
            history.push_back(command);
            notify("Move completed.");

            if (validation->isSolved(board)) {
                board.print();
                notify("Congratulations! Sudoku is solved.");
                running = false;
            }
        }
        else {
            notify("This move violates Sudoku rules.");
        }
    }

    void clearCell(int row, int col) {
        if (!correctCoordinates(row, col)) {
            notify("Incorrect cell coordinates.");
            return;
        }

        row--;
        col--;

        if (board.isStartCell(row, col)) {
            notify("You cannot clear a fixed cell.");
            return;
        }

        MoveCommand command(&board, row, col, GameSettings::getInstance()->emptyCell);
        command.execute();
        history.push_back(command);
        notify("Cell cleared.");
    }

    void undo() {
        if (history.empty()) {
            notify("Move history is empty.");
            return;
        }

        history.back().undo();
        history.pop_back();
        notify("Last move was undone.");
    }

    void save() {
        saves.push_back(BoardMemento(board.getState()));
        notify("Game state saved.");
    }

    void load() {
        if (saves.empty()) {
            notify("There are no saved states.");
            return;
        }

        board.setState(saves.back().getState());
        notify("Last saved state loaded.");
    }

    void restart() {
        board.restart();
        history.clear();
        notify("Game restarted.");
    }

    void run() {
        printRules();

        while (running) {
            board.print();

            cout << "Enter command: ";

            string command;
            cin >> command;

            if (command == "set") {
                int row, col, value;
                cin >> row >> col >> value;
                setNumber(row, col, value);
            }
            else if (command == "clear") {
                int row, col;
                cin >> row >> col;
                clearCell(row, col);
            }
            else if (command == "undo") {
                undo();
            }
            else if (command == "save") {
                save();
            }
            else if (command == "load") {
                load();
            }
            else if (command == "restart") {
                restart();
            }
            else if (command == "help") {
                printRules();
            }
            else if (command == "exit") {
                running = false;
            }
            else {
                notify("Unknown command. Type help.");
            }
        }

        notify("Game finished.");
    }
};

int main() {
    EasySudokuFactory factory;
    SudokuGame game(&factory);

    ConsoleGameObserver observer;
    game.addObserver(&observer);

    game.run();

    GameSettings::destroy();

    return 0;
}
