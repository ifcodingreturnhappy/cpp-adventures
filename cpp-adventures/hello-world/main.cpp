#include <iostream>
#include <string>
using namespace std;

#define LOG(x) std::cout << x << std::endl

enum class PlayerId {
	Player1 = 1,
	Player2 = -1
};

class TicTacBoardLogic {
private:
	// Positions:
	// 0 | 1 | 2
	// 3 | 4 | 5
	// 6 | 7 | 8
	static const int AMOUNT_OF_POSITIONS = 9;

	static const int SUMS_SIZE = 9;

	int positions[AMOUNT_OF_POSITIONS];
	PlayerId currentPlayerPlaying;


	void setRandomPlayer() {
		srand(time(NULL)); //Randomize seed initialization
		int randNum = rand() % 2 + 1; // Generate a random number between 1 and 2

		if (randNum == 1)
			currentPlayerPlaying = PlayerId::Player1;
		else
			currentPlayerPlaying = PlayerId::Player2;
	}

	void toggleCurrentPlayer() {
		if (currentPlayerPlaying == PlayerId::Player1)
			currentPlayerPlaying = PlayerId::Player2;
		else
			currentPlayerPlaying = PlayerId::Player1;
	}

public:
	TicTacBoardLogic()
	{
		for (int i = 0; i < AMOUNT_OF_POSITIONS; i++) {
			positions[i] = 0;
		}

		setRandomPlayer();
	}

	TicTacBoardLogic(PlayerId playerStartingPlaying) : TicTacBoardLogic()
	{
		currentPlayerPlaying = playerStartingPlaying; 
	}

	int getAmountOfPositions() {
		return AMOUNT_OF_POSITIONS;
	}

	PlayerId getCurrentPlayerPlaying() { return currentPlayerPlaying; }

	bool playAtPosition(int positionIndex) {
		try
		{
			if (positionIndex < 0 || positionIndex >= AMOUNT_OF_POSITIONS)
				return false;

			if (positions[positionIndex] != 0)
				return false;
			else
			{
				// Set the position as the currently playing player's
				positions[positionIndex] = (int)currentPlayerPlaying;

				// Change the player currently playing
				toggleCurrentPlayer();
			}
			return true;
		}
		catch (const char* ex)
		{
			return false;
		}
	}

	int getPositionRow(int positionIndex) {
		cout << floor(positionIndex / 3);
	}

	/// <summary>
	/// Outputs: [0,1,2,3] -> [Playing, P1-Win, P2-Win, Draw]
	/// </summary>
	int getGameState() {
		int sums[SUMS_SIZE] = { 0,0,0,0,0,0,0,0,0 }; // [R1,R2,R3, C1,C2,C3, D-primary,D-secondary, IsBoardFull]

		bool isBoardFull = true;

		for (int i = 0; i < AMOUNT_OF_POSITIONS; i++)
		{
			// Check rows
			int row = (int)floor(i / 3);
			sums[row] += positions[i];

			// Check columns 
			int col = i % 3;
			sums[col + 3] += positions[i];

			// Check diagonals
			// Principal diagonal
			if (row == col)
				sums[6] += positions[i];

			// Secondary diagonal
			if (row + col == 2)
				sums[7] += positions[i];

			// Set the is board full state to false since there is still an empty position
			if (positions[i] == 0)
				isBoardFull = false;
		}
		if (isBoardFull)
			sums[8] = 1;

		int incompleteLinesCount = 0;
		for (int i = 0; i < SUMS_SIZE; i++)
		{
			if (sums[i] == 3 * (int)PlayerId::Player1) // Player 1 won, early termination
				return 1;
			else if (sums[i] == 3 * (int)PlayerId::Player2) // Player 2 won, early termination
				return 2;
		}
		// If all the lines are complete and no player won, the game is a draw; 
		// else, the game is still playing
		if (sums[8] == 0)
			return 0; // Still playing
		else
			return 3; // Draw
	}

	int* getCurrentPositions() {
		return positions;
	}
};

class GameUI {
private:
	int LARGE_ROW_SIZE = 50;
	int _ammountOfPositions;

	char _player1Marker;
	char _player2Marker;

public:
	GameUI(char player1Marker, char player2Marker, int ammountOfPositions) {
		_player1Marker = player1Marker;
		_player2Marker = player2Marker;
		_ammountOfPositions = ammountOfPositions;
	}

	void moveToPreviousRow() {
		printf("\x1b[A");
	}

	void moveToStartOfCurrentRow() {
		printf("\r");
	}

	void clearCurrentRow() {
		cout << "\r" + string(LARGE_ROW_SIZE, ' ');
	}


	int readMoveFromPlayer(PlayerId playerToPlay) {
		char player = playerToPlay == PlayerId::Player1 ? _player1Marker : _player2Marker;
		cout << "\n\n" << player << " - Choose your play (from 1 to 9): \n\n";
		
		std::string line;
		std::getline(std::cin, line);

		try
		{
			int number = std::stoi(line);
			return number - 1;
		}
		catch (const std::exception&)
		{
			return -1;
		}
	}

	void printBoard(int* gamePositions) {
		int previousRow = -1;

		// Clean previous lines
		for (int i = 0; i < _ammountOfPositions; i++)
		{
			clearCurrentRow();
			moveToPreviousRow();
			moveToStartOfCurrentRow();
		}

		// Print the game state
		for (int i = 0; i < _ammountOfPositions; i++)
		{
			// Print to a new line if the row changes
			int currentRow = (int)floor(i / 3);
			if (currentRow != previousRow) {
				// Make sure row is clean from previous "frames"
				cout << endl;
				previousRow = currentRow;
			}
			
			// Print empty space if at the beggining of row
			int currentCol = i % 3;
			if (currentCol == 0) {
				cout << "        ";
			}

			// Print the player at the position
			cout << getPlayerMarker(gamePositions[i]) << " ";
		}
	}

	void printFinalState(int finalState) {
		cout << "\n\n";

		if (finalState == 3)
			cout << "DRAW!";
		else
			cout << "PLAYER " << finalState << " WON!";

		cout << "\n\n";
	}


	void printPositionList(int* currentPositions) {
		string output = "[";
		for (int i = 0; i < _ammountOfPositions; i++) {
			if (i > 0)
				output += ',';
			output += to_string(currentPositions[i]);
		}
		cout << output + "]\n";
	}

private:
	char getPlayerMarker(int player) {
		if (player == (int)PlayerId::Player1)
			return _player1Marker;
		else if (player == (int)PlayerId::Player2)
			return _player2Marker;
		else
			return '-';
	}
};

int main() {
	TicTacBoardLogic board;
	GameUI UI('x', 'o', board.getAmountOfPositions());

	PlayerId output = board.getCurrentPlayerPlaying();

	bool gameRunning = true;

	while (gameRunning && board.getGameState() == 0) {
		// Draw the game UI
		UI.printBoard(board.getCurrentPositions());

		// Read user input
		int move = UI.readMoveFromPlayer(board.getCurrentPlayerPlaying());

		// Use the player input to play
		if (board.playAtPosition(move) == false) {
			UI.clearCurrentRow();
			UI.moveToStartOfCurrentRow();
			UI.moveToPreviousRow();
			UI.clearCurrentRow();
			UI.moveToStartOfCurrentRow();
		}
	}
	// Draw the board for the last play
	UI.printBoard(board.getCurrentPositions());
	
	// Print the result of the game
	UI.printFinalState(board.getGameState());
}


