#include "Engine.h"
#include "Object.h"
#include "Draw.h"
#include "Physics.h"
#include <stdlib.h>
#include <memory.h>
#include <utility>
#include <vector>
#include <algorithm>

//
//  You are free to modify this file
//

//class Test {
//private:
//    int a = 0;
//    double b = 0;
//    std::pair<int, int> c = {0, 0};
//public:
//    Test();
//    Test(int _a) : a(_a) {};
//    Test(const Test& t) {
//        a = t.a;
//        b = t.b;
//        c = t.c;
//    }
//};

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

unsigned int obj_cnt = 0;
std::vector<GameObject> objectList = {};
int room = 0;

// initialize game data in this function
void initialize()
{
    
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
    
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();
    if (is_key_pressed(VK_SPACE) && obj_cnt == 0)
    {   
        int a = get_cursor_x();
        int b = get_cursor_y();
        objectList.push_back(GameObject({0, 0}));
    }
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
    // clear backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    for (int i = SCREEN_WIDTH / 4; i < SCREEN_WIDTH * 3 / 4; i++)
        for (int j = SCREEN_HEIGHT / 4; j < SCREEN_HEIGHT * 3 / 4; j++)
            buffer[j][i] = 0x00FFFFFF;

    if (0 <= (get_cursor_x() + 50) && (get_cursor_x() + 50) <= SCREEN_WIDTH && 0 <= (get_cursor_y() + 50) && (get_cursor_y() + 50) < +SCREEN_HEIGHT)
        buffer[get_cursor_y() + 50][get_cursor_x() + 50] = 0xFFFF0000;
    if (0 <= (get_cursor_x() - 50) && (get_cursor_x() - 50) <= SCREEN_WIDTH && 0 <= (get_cursor_y() - 50) && (get_cursor_y() - 50) < +SCREEN_HEIGHT)
        buffer[get_cursor_y() - 50][get_cursor_x() - 50] = 0x80FF0000;

}

// free game data in this function
void finalize()
{
}

