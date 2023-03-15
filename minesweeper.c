#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

/*
ToDo:
	PROJECT COMPLETED
*/

//minesweeper is a singleplayer game, I refuse to make it 2 player because it wouldnt work. If someone wants to play 2 player they can alternate with the mouse and sort it out themselves.

FILE* save_file;

void gen();
void start_message();
void game_loop();
void discoverTiles(x, y);
void lose_cond(x, y);
void win_cond();
void updated_vis();
void saveGame();
void loadGame();


int user_in_x = -1, user_in_y = -1;
int width = 8, height = 8;
float start;
int total_bombs = 5;
int bombs = 0;
int minefield[8][8];
bool discov_tiles[8][8];
bool are_you_dead;
bool did_you_win;

void saveGame() 
{
	save_file = fopen("game_save.txt", "w");
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++) 
		{

			if (minefield[x][y] == -1)
			{
				fputc(9 + '0', save_file);
			}
			else 
			{
				fputc(minefield[x][y] + '0', save_file);
			}
		}
		fputc('\n', save_file);
	}
	
	fputc('\n', save_file);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++) 
		{

			if (discov_tiles[x][y])
			{
				fputc(1 + '0', save_file);
			}
			else 
			{
				fputc(0 + '0', save_file);
			}
		}
		fputc('\n', save_file);
	}
	fclose(save_file);
}

void loadGame()
{
	//it just works
	save_file = fopen("game_save.txt", "r");
	
	int rowIndex = 0;
	char buffer[255];
	while(fscanf(save_file, "%[^\n] ", buffer) != EOF)
	{
		if(rowIndex < height)
		{
			for (int y = 0; y < height; y++) 
			{
				if (buffer[y] - '0' == 9)
				{
					minefield[rowIndex][y] = -1;
				}
				else 
				{
					minefield[rowIndex][y] = buffer[y] - '0';
				}
			}
			rowIndex++;
		}
		else if(rowIndex - height < height)
		{
			for (int y = 0; y < height; y++) 
			{
				if (buffer[y] - '0' == 0)
				{
					discov_tiles[rowIndex - height][y] = false;
				}
				else 
				{
					discov_tiles[rowIndex - height][y] = true;
				}
			}
			rowIndex++;
		}
	}
	fclose(save_file);
}


//Intro main page to minesweeper game
int main() 
{
	srand(time(NULL));
	printf("\t\tWelcome to BombSquad!\n");
	start_message();
	return 0;
}

//This method allows the user to start the game or exit 
void start_message() 
{
	start = 0;
	while ((start != 1) && (start != -1) && (start != 2))
	{
		if(are_you_dead == true || did_you_win == true){
			printf("\n\n\n\t\tWould you like to play again? Enter 1 if yes and -1 if you would like to Exit: \n");
		}
		else {
			printf("\t\tPlease enter 1 to START a new game, 2 to LOAD your previous save or -1 to Exit:  \n");
		}
		scanf("%f", &start);

		if (start == 1)
		{
			are_you_dead = false;
			did_you_win = false;
			gen();
			game_loop();
		}
		else if(start == 2)
		{
			are_you_dead = false;
			did_you_win = false;
			loadGame();
			game_loop();
		}
		else if (start == -1) 
		{
			exit(1);
		}
	}
}


void gen() 
{
	//generate a 2d array that contains 30x30 items
	//setting all values back to 0
	//First array dimension
	for (int f = 0; f < sizeof(minefield) / sizeof(minefield[0]); f++) // if length is 5 then we will loop over it with the f being: 0, 1, 2, 3, 4
	{
		//Second array dimension
		for (int s = 0; s < sizeof(minefield[f]) / sizeof(minefield[f][0]); s++)
		{
			//Set each value back to 0
			minefield[f][s] = 0;
			discov_tiles[f][s] = false;
		}
	}

	//set 10 amount of bombs on random positions
	//Sets all the bombs on random tiles
	for (int b = 0; b < total_bombs; b++)
	{
		int x = rand() % width; //first dimension of the array
		int y = rand() % height; //second dimension of the array

		//Check if tile already has a bomb
		if (minefield[x][y] != -1) {
			bombs++;
		}

		//placing bombs(-1)
		minefield[x][y] = -1;
	}

	//set the number of bombs in the neighbouring tiles for each tile
	for (int f = 0; f < sizeof(minefield) / sizeof(minefield[0]); f++)
	{
		for (int s = 0; s < sizeof(minefield[f]) / sizeof(minefield[f][0]); s++)
		{
			//check if its not a bomb					(dont need to count the neighbours for bomb tiles)(he has plenty already)
			if (minefield[f][s] != -1)
			{
				int count = 0;
				//count neighbours that have the -1 value

				//  	-1	0	1
				//-1	x	x	x
				// 0	x	o	x
				// 1	x	x	x

				if (minefield[f][s] != -1)
				{
					minefield[f][s] = 0;
				}
				if (f - 1 >= 0 && s - 1 >= 0 && minefield[f - 1][s - 1] == -1) 
				{
					minefield[f][s]++;
				}
				if (f - 1 >= 0 && minefield[f - 1][s] == -1) 
				{
					minefield[f][s]++;
				}
				if (s - 1 >= 0 && minefield[f][s - 1] == -1) 
				{
					minefield[f][s]++;
				}
				if (f - 1 >= 0 && s + 1 < width && minefield[f - 1][s + 1] == -1) 
				{
					minefield[f][s]++;
				}
				if (f + 1 < height && s - 1 >= 0 && minefield[f + 1][s - 1] == -1) 
				{
					minefield[f][s]++;
				}
				if (f + 1 < height && minefield[f + 1][s] == -1) 
				{
					minefield[f][s]++;
				}
				if ( s + 1 < width && minefield[f][s + 1] == -1) 
				{
					minefield[f][s]++;
				}
				if (f + 1 < height && s + 1 < width && minefield[f + 1][s + 1] == -1) 
				{
					minefield[f][s]++;
				}

				//set the minefield value to count
				//minefield[f][s] = count;
			}
		}
	}
}

void game_loop()
{
	updated_vis();

	//request user input
	while ((user_in_x == -1) && (user_in_y == -1))
	{
		printf("\t\tPlease enter an X co-ordinate or type -2 to Save and Exit: \n");
		scanf("%d", &user_in_x);

		//saving and exiting mid game
		if (user_in_x == -2 || user_in_y == -2)
		{
			printf("\n\t\tYou have saved and exited the game!\n");
			saveGame();
			fclose(save_file);
			exit(1);
		}

		printf("\t\t please enter a Y co-ordinate: \n");
		scanf("%d", &user_in_y);

		//input for x and y co-ordinates
		if (user_in_x >= width || user_in_y >= height)
		{
			printf("\t\tPlease enter a valid co-ordinate! \n");
			user_in_x = -1;
			user_in_y = -1;
		}
		else if (user_in_x >= 0 && user_in_y >= 0 && discov_tiles[user_in_x][user_in_y] == true)
		{
			printf("\t\tPlease enter a co-ordinate that has not been discovered yet!\n");
			user_in_x = -1;
			user_in_y = -1;
		}
	}

	//calculate values based on user input
	discoverTiles(user_in_x, user_in_y);
	
	//resets values
	user_in_x = -1;
	user_in_y = -1;

	//show level
	//		|0| |1| |2|
	// 
	//|0|	|x|	|x|	|x|
	//|1|	|x|	| |	| |
	//|2|	|x|	| |	| |

	//start another game loop
	if (are_you_dead == false && did_you_win == false) 
	{
		game_loop();
	}
	else 
	{
		start_message();
	}
}

//method to discover all neighbouring tiles that have a value of '0'
void discoverTiles(int x, int y) {
	//If this tile has already been discovered. then dont discover it again... dumbass

	if (discov_tiles[x][y] == true)
	{
		return;
	}

	//set discovered to true at pos x,y
	discov_tiles[x][y] = true;
	lose_cond(x,y);
	win_cond();
	//if current tile is not a zero then dont continue :D
	if (minefield[x][y] != 0)
	{
		return;
	}

	//recall this method for each neighbouring tile
	if (x - 1 >= 0 && y - 1 >= 0 && minefield[x - 1][y - 1] == 0) 
	{
		discoverTiles(x - 1, y - 1);
	}
	if (x - 1 >= 0 && minefield[x - 1][y] == 0) 
	{
		discoverTiles(x - 1, y);
	}
	if (y - 1 >= 0 && minefield[x][y - 1] == 0) 
	{
		discoverTiles(x, y - 1);
	}
	if (x - 1 >= 0 && y + 1 < width && minefield[x - 1][y + 1] == 0) 
	{
		discoverTiles(x - 1, y + 1);
	}
	if (x + 1 < height && y - 1 >= 0 && minefield[x + 1][y - 1] == 0) 
	{
		discoverTiles(x + 1, y - 1);
	}
	if (x + 1 < height && minefield[x + 1][y] == 0) 
	{
		discoverTiles(x + 1, y);
	}
	if (y + 1 < width && minefield[x][y + 1] == 0) 
	{
		discoverTiles(x, y + 1);
	}
	if (x + 1 < height && y + 1 < width && minefield[x + 1][y + 1] == 0) 
	{
		discoverTiles(x + 1, y + 1);
	}

	return;
}

//lose condition method, this resets the game
void lose_cond(int x, int y) 
{
	if (minefield[x][y] == -1)
	{
		updated_vis();
		printf("\t\tBOOM!\n");
		printf("\t\tGAME OVER. YOU ARE DEAD!\n");
		are_you_dead = true;
	}
}

void win_cond()
{
	//check are_you_dead. if so then dont check for win condition
	if (are_you_dead == true) 
	{
		return;
	}

	int tiles_discovered = 0;

	//check if all tiles that are not bombs have been discovered
	for (int x = 0; x < sizeof(minefield) / sizeof(minefield[0]); x++)
	{
		for (int y = 0; y < sizeof(minefield[x]) / sizeof(minefield[x][0]); y++) {
			
			//check if tiles != -1
			if (minefield[x][y] != -1 && discov_tiles[x][y] == true)
			{
				//variable++ until 71 tiles discovered
				tiles_discovered++;
			}
		}
	}
	
	//win check here
	int totalMapSize = width * height;
	//check if variable == 71
	if (tiles_discovered == totalMapSize - bombs)
	{
		//game won
		//set variable back to 0
		updated_vis();
		printf("\n\t\tYOU'VE WON!\n");
		did_you_win = true;
		tiles_discovered = 0;
	}
}

//prints visuals
void updated_vis() 
{
	//show level
	//		|0| |1| |2|
	// 
	//|0|	|x|	|x|	|x|
	//|1|	|x|	|x|	|x|
	//|2|	|x|	|x|	|x|

	printf("\t\t");
	//prints x axis
	for (int i = 0; i < width; i++)
	{
		printf("|%d|\t", i);
	}
	printf("\n\n");

	//prints y axis
	for (int f = 0; f < height; f++)
	{
		printf("|%d|\t\t", f);

		for (int z = 0; z < sizeof(minefield[f]) / sizeof(minefield[f][0]); z++)
		{
			//checks if tile is already discovered or not
			if (discov_tiles[f][z] == true)
			{
				if (minefield[f][z] == 0)
				{
					printf("| |\t");
				}
				else {
					printf("|%d|\t", minefield[f][z]);
				}
			}
			else
			{
				printf("|x|\t");
			}
		}
		printf("\n");
	}
}


