#include "Engine.h"
#include "Object.h"
#include <stdlib.h>
#include <memory.h>
#include <ctime>
#include <utility>
#include <vector>
#include <algorithm>
#include <memory>
#include <cmath>

//
//  You are free to modify this file
//

//  is_key_pressed(int button_vk_code) - check if a key is pressed,
//                                       use keycodes (VK_SPACE, VK_RIGHT, VK_LEFT, VK_UP, VK_DOWN, 'A', 'B')
//
//  get_cursor_x(), get_cursor_y() - get mouse cursor position
//  is_mouse_button_pressed(int button) - check if mouse button is pressed (0 - left button, 1 - right button)
//  clear_buffer() - set all pixels in buffer to 'black'
//  is_window_active() - returns true if window is active
//  schedule_quit_game() - quit game after act()

unsigned int obj_cnt = 0;
unsigned int ui_cnt = 0;
std::vector<std::shared_ptr<GameObject>> objectList = {};
std::vector<std::shared_ptr<GameObject>> uiList = {};
uint32_t background[SCREEN_HEIGHT][SCREEN_WIDTH] = {0xFF000000};
Rooms room = STARTUP;
int game_state = 0;
float globals[10] = {0};
const int pipe_diff = 300;

// initialize game data in this function
void initialize()
{
    std::srand(std::time(nullptr));
}

void RoomStartup(float dt) {
    if (is_key_pressed(VK_SPACE))
    {
        room = TITLE;
        memset(globals, 0, 10 * sizeof(float));
        instance_clear();
        return;
    }
    if (obj_cnt == 0)
    {
        background_set("NULL");
        instance_create_sprited({ SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 }, "logo.bmp", false, { 100, 50 });
        objectList[0]->SetAlpha(0.0);
    }
    globals[0] += dt; // timer
    if (globals[0] > 0 && globals[0] <= 1)
    {
        objectList[0]->SetAlpha(static_cast<double>(globals[0]));
    }
    if (globals[0] >= 4 && globals[0] < 5)
        objectList[0]->SetAlpha(5 - static_cast<double>(globals[0]));
    if (globals[0] > 5)
    {
        room = TITLE;
        memset(globals, 0, 10 * sizeof(float));
        instance_clear();
    }
}

void RoomTitle(float dt) {
    if (ui_cnt == 0 && obj_cnt == 0)
    {
        background_set("Background.bmp");
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 }, "Title.bmp", true, {128, 196});
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 3 }, "Start.bmp", true, {187, 10});
    }
    globals[0] += dt; // timer
    if (globals[0] > 1 && uiList[1]->Alpha() > 0.5)
    {
        uiList[1]->SetAlpha(0.0);
        globals[0] = 0;
    }
    if (globals[0] > 1 && uiList[1]->Alpha() < 0.5)
    {
        uiList[1]->SetAlpha(1.0);
        globals[0] = 0;
    }
    if (is_key_pressed(VK_SPACE))
    {
        room = GAME;
        memset(globals, 0, 10 * sizeof(float));
        instance_clear();
    }
}

void RoomGame(float dt) {
    if (is_key_pressed('A') && globals[8] != 0) // reset game
    {
        memset(globals, 0, 10 * sizeof(float));
        instance_clear();
    }
    if (!is_key_pressed('A'))
        globals[8] = 1; // game start flag
    if ((obj_cnt == 0))
    {
        background_set("PipeBackground.bmp");
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, "Bird.bmp", true, {48, 55}, CIRCLE, 15);
        uiList[0]->SetGravity(1000);
        uiList[0]->SetDirection(M_PI / 2);
        globals[1] = std::rand() % (SCREEN_HEIGHT - pipe_diff) + pipe_diff / 2; // pipe height
        instance_create_sprited({ SCREEN_WIDTH, globals[1] + pipe_diff / 2 }, "PipeUp.bmp", false, {0, 0}, RECTANGLE, 768, 254);
        instance_create_sprited({ SCREEN_WIDTH, globals[1] - pipe_diff / 2 }, "PipeDown.bmp", false, {0, 768}, RECTANGLE, 768, 254);
        objectList[obj_cnt - 1]->SetSpeed(-300);
        objectList[obj_cnt - 2]->SetSpeed(-300);
        uiList[0]->CollisionCheck(*objectList[0]);
    }
    if (globals[9] == 42 && ui_cnt < 2)
    {
        uiList[0]->SetSpeed(0.0);
        uiList[0]->SetGravity(0.0);
        uiList[0]->SetAlpha(0.5);
        for (auto& obj : objectList)
        {
            obj->SetSpeed(0.0);
            obj->SetAlpha(0.5);
        }
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 5 }, "GameOver.bmp", true, {182, 94});
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT * 4 / 5 }, "Restart.bmp", true, { 342, 10 });
    }
    if (globals[9] == 42)
        return;
    globals[0] += dt; // timer
    if (is_key_pressed(VK_SPACE) && globals[7] < 1)
    {
        globals[7] = 1; // jump held flag
        uiList[0]->SetSpeed(300);
    }
    if (!is_key_pressed(VK_SPACE))
        globals[7] = 0;
    if (globals[0] > 2.5)
    {
        if (obj_cnt >= 4)
        {
            objectList.erase(objectList.begin());
            objectList.erase(objectList.begin());
            obj_cnt -= 2;
        }
        globals[0] = 0;
        globals[2]++; //score
        globals[1] = std::rand() % (SCREEN_HEIGHT - pipe_diff) + pipe_diff / 2;
        instance_create_sprited({ SCREEN_WIDTH, globals[1] + pipe_diff / 2 }, "PipeUp.bmp", false, { 0, 0 }, RECTANGLE, 768, 254);
        instance_create_sprited({ SCREEN_WIDTH, globals[1] - pipe_diff / 2 }, "PipeDown.bmp", false, { 0, 768 }, RECTANGLE, 768, 254);
        objectList[obj_cnt - 1]->SetSpeed(-300);
        objectList[obj_cnt - 2]->SetSpeed(-300);
    }
    if (globals[2] >= 10)
    {
        room = VICTORY;
        memset(globals, 0, 10 * sizeof(float));
        instance_clear();
        return;
    }

    if (uiList[0]->Y() < 0 || uiList[0]->Y() > SCREEN_HEIGHT)
        globals[9] = 42;

    for (auto& obj : objectList)
    {
        if (uiList[0]->CollisionCheck(*obj))
             globals[9] = 42; // lose flag
    }

}

void RoomVictory(float dt) {
    if (obj_cnt == 0)
    {
        background_set("Background.bmp");
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT * 3 / 7 }, "Epic.bmp", false, { 264, 38 });
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 3}, "Win.bmp", false, { 107, 88});
        instance_create_sprited({ SCREEN_WIDTH / 2, SCREEN_HEIGHT * 6 / 7 }, "EpicLore.bmp", false, {400, 15});
    }
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
    
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    switch (room) {
    case STARTUP:
        RoomStartup(dt);
        break;
    case TITLE:
        RoomTitle(dt);
        break;
    case GAME:
        RoomGame(dt);
        break;
    case VICTORY:
        RoomVictory(dt);
        break;
    default:
        break;
    }

    for (auto& obj : objectList) {
        obj->MoveTo(obj->X() + dt * obj->Speed() * std::cos(obj->Direction()),
            obj->Y() - dt * obj->Speed() * std::sin(obj->Direction()));
        obj->SetSpeed(obj->Speed() - dt * obj->Gravity());
    }
    for (auto& obj : uiList) {
        obj->MoveTo(obj->X() + dt * obj->Speed() * std::cos(obj->Direction()),
            obj->Y() - dt * obj->Speed() * std::sin(obj->Direction()));
        obj->SetSpeed(obj->Speed() - dt * obj->Gravity());
    }

    //if (is_key_pressed(VK_SPACE) && obj_cnt == 0)
    //{   
    //    double a = get_cursor_x();
    //    double b = get_cursor_y();
    //    instance_create_sprited({a, b}, "test.bmp", {16, 16});
    //}
    //
    //if (obj_cnt != 0)
    //{
    //    if (is_key_pressed(VK_UP))
    //        objectList[0]->SetAlpha(std::clamp(objectList[0]->Alpha() + dt, 0.0, 1.0));
    //    if (is_key_pressed(VK_DOWN))
    //        objectList[0]->SetAlpha(std::clamp(objectList[0]->Alpha() - dt, 0.0, 1.0));
    //    if (is_key_pressed(VK_LEFT))
    //    {
    //        if (objectList[0]->SpriteName() == "test.bmp")
    //            objectList[0]->ChangeSprite("pipeUp.bmp");
    //    }
    //    //if (is_key_pressed(VK_RIGHT) && obj_cnt == 1)
    //    objectList[0]->MoveTo(get_cursor_x(), get_cursor_y());
    //}
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
    // clear backbuffer
    //memset(buffer, background, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
    memcpy(buffer, background, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    for (auto& obj : objectList) {
        obj->Draw();
    }

    for (auto& ui : uiList) {
        ui->Draw();
    }

}

// free game data in this function
void finalize()
{
}

