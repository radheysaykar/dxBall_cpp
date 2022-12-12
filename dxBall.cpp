/*

zoom out 4 times on terminals before playing 

to play enter these lines:
$ g++ -std=c++11 -pthread dxBall.cpp -odxBall
$ ./dxBall
and hit enter

enter:  change direction of board
Ctrl + c: terminate
*/


#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;

condition_variable cv;
int size = 62, direction = +1,  board_l = 9, board_half_l = 4, ball_x = 34, ball_y = 56, delay = 30, board_mid_y = 57, board_mid_x = 34, direction_ball_x = +1, direction_ball_y = -1, p = 0, dx2 = (size-1), dx2_dy2 = 1-size, c, initial_board_position = 30, remianing_bricks = 0;
bool display[62][62];


void Display(){
    system("clear");
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<size; j++) cout<<((display[i][j])?'@':' ')<<' ';
		cout<<"\n";
	}
}


void pause_execution(double seconds){
	time_t start = time(0);
	while(difftime(time(0), start) < seconds) ;
}

void circle(int xcenter, int ycenter, int r, bool v){
	int x = 0, y = r, p = 1 - r;
	
	display[xcenter + x][ycenter + y] = v;	
	display[xcenter + x][ycenter - y] = v;
	display[xcenter - x][ycenter + y] = v;
	display[xcenter - x][ycenter - y] = v;
	display[xcenter + y][ycenter + x] = v;
	display[xcenter + y][ycenter - x] = v;
	display[xcenter - y][ycenter + x] = v;
	display[xcenter - y][ycenter - x] = v;
	
	while(x<y){
		x++;
		if( p < 0 ) p = p + 2 * x + 1;
		else{
			y--;
			p = p + 2 * (x - y) + 1;
		}
		display[xcenter + x][ycenter + y] = v;	
		display[xcenter + x][ycenter - y] = v;
		display[xcenter - x][ycenter + y] = v;
		display[xcenter - x][ycenter - y] = v;
		display[xcenter + y][ycenter + x] = v;
		display[xcenter + y][ycenter - x] = v;
		display[xcenter - y][ycenter + x] = v;
		display[xcenter - y][ycenter - x] = v;
	}
}


void play_waves(){
	int t=1;
	while(t--){
		pause_execution(1);
		circle(30,30,5,1);
		Display();
		pause_execution(1);
		circle(30,30,8,1);
		Display();
		pause_execution(1);
		circle(30,30,12,1);
		Display();
		pause_execution(1);
		circle(30,30,16,1);
		Display();
		pause_execution(1);
		circle(30,30,20,1);
		Display();
		pause_execution(1);
		circle(30,30,20,0);
		Display();
		pause_execution(1);
		circle(30,30,16,0);
		Display();
		pause_execution(1);
		circle(30,30,12,0);
		Display();
		pause_execution(1);
		circle(30,30,8,0);
		Display();
		pause_execution(1);
		circle(30,30,5,0);
		Display();
	}
}

void read_value(){
	c = cin.get();
	direction = (-1)*direction;
	cv.notify_one();
}

void draw_brick(int x, int y){
	display[x][y] = 1;
	display[x][y-1] = 1;
	display[x+1][y] = 1;
	display[x+1][y-1] = 1;
	display[x-1][y] = 1;
	
	display[x-1][y-1] = 1;
}
void first_display(){
	for(int i = 0; i < size-1; i++){ display[size-1][i] = 1; display[i][size-1] = 1; display[0][i] = 1; display[i][0] = 1;}          //creates frame
	for(int i =0 ; i<board_l; i++) display[i+initial_board_position][board_mid_y] = 1;
	display[ball_x][ball_y] = 1;
	for (int i = 2; i < 56; i+=4)
	{
		draw_brick(i,6);
		remianing_bricks++;
		draw_brick(i+1,12);
		remianing_bricks++;
	}
	Display();
}

int main()
{
	//ios_base::sync_with_studio(false);
	cin.tie(0);	
	first_display();
	
	while(true){
		thread th(read_value);
		mutex mtx;
		unique_lock<mutex> lck(mtx);
		while(cv.wait_for(lck, chrono::milliseconds(delay)) == cv_status::timeout){
			int board_end_x = board_mid_x+(direction)*board_half_l, board_start_x = board_mid_x-(direction)*board_half_l;
			if(board_end_x != 1   && board_end_x != size-2){
				display[board_end_x+direction][board_mid_y] = 1;
				display[board_start_x][board_mid_y] = 0;
				board_mid_x+= direction;
			}
			
			int ball_x_prev = ball_x, ball_y_prev = ball_y;
			
			if(p>=0) ball_x += direction_ball_x, p += dx2_dy2;
			else p += dx2;
			ball_y +=direction_ball_y;
			
			if(!display[ball_x][ball_y]) display[ball_x_prev][ball_y_prev] = 0, display[ball_x][ball_y] = 1;
			else{
			 
				if(ball_y == size-1){ play_waves(); break;}
				else if(ball_y == 0)  direction_ball_y *=(-1); 

				else if(ball_x == size-1 || ball_x == 0) direction_ball_x *=(-1); 
				else if(ball_y == board_mid_y){	
					int dist_from_mid = ball_x-board_mid_x, dy, dx;
					
					if(dist_from_mid == 0) dx = 0, dy = 1;
					else if(dist_from_mid < 3 && dist_from_mid > -3) dx = (size-1)/2, dy = size-1; 
					else dx = (size-1)/2, dy = (size-1)/2; 
					p = 2*dx - dy, dx2_dy2 = 2*dx - 2*dy, dx2 = 2*dx;
					
					if(dist_from_mid > 0) direction_ball_x = 1;
					else direction_ball_x = -1;
					direction_ball_y *=(-1); 
					
				}
				else{
					int brick_x = ball_x, brick_y = ball_y; 
					while(display[brick_x][brick_y-1]) brick_y--;
					while(display[brick_x-1][brick_y]) brick_x--;
					
					display[brick_x][brick_y] =display[brick_x+1][brick_y] = display[brick_x+2][brick_y] = display[brick_x][brick_y+1] =display[brick_x+1][brick_y+1] = display[brick_x+2][brick_y+1] = 0;
					
					if(ball_x_prev < brick_x || ball_x_prev > brick_x+2) direction_ball_x *= (-1);
					else direction_ball_y *= (-1); 

					 remianing_bricks--;
					 if(remianing_bricks == 0){
					 	play_waves;
					 	break;
					 }  
				}
				ball_x = ball_x_prev, ball_y = ball_y_prev;
			}
			Display();
		}
		
		cout<<"press ctrl+c to exit\n";
		th.join();
	}
	return 0;
}
