#include "screens.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define GUI_MENU_MAIN_IMPLEMENTATION

#include "menu_main.h"

GuiMenuMainState menu_state;
// Title Screen Initialization logic
void InitTitleScreen(void)
{
  menu_state = InitGuiMenuMain();
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    // TODO: Update TITLE screen variables here!
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
  BeginDrawing();
  ClearBackground(RAYWHITE);

  GuiMenuMain(&menu_state);
  DrawFPS(10, 10);

  EndDrawing();
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
}
