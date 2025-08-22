/*******************************************************************************************
*
*   MenuMain v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_MENU_MAIN_IMPLEMENTATION
*       #include "gui_menu_main.h"
*
*       INIT: GuiMenuMainState state = InitGuiMenuMain();
*       DRAW: GuiMenuMain(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"
#include "game_process.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_MENU_MAIN_H
#define GUI_MENU_MAIN_H

typedef struct {
    // Define controls variables

    // Define rectangles
    Rectangle layoutRecs[3];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiMenuMainState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiMenuMainState InitGuiMenuMain(void);
void GuiMenuMain(GuiMenuMainState *state);
static void Button001();                // Button: Button002 logic
static void Button002();                // Button: Button002 logic

#ifdef __cplusplus
}
#endif

#endif // GUI_MENU_MAIN_H

/***********************************************************************************
*
*   GUI_MENU_MAIN IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_MENU_MAIN_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiMenuMainState InitGuiMenuMain(void)
{
    GuiMenuMainState state = { 0 };

    // Initilize controls variables

    // Init controls rectangles
    state.layoutRecs[0] = (Rectangle){ 0, 0, 576, 584 };// GroupBox: GroupBox000
    state.layoutRecs[1] = (Rectangle){ 216, 256, 120, 24 };// Button: Button002
    state.layoutRecs[2] = (Rectangle){ 216, 160, 120, 24 };// Button: Button002

    // Custom variables initialization

    return state;
}
// Button: Button002 logic
static void Button001()
{
  GameTransitionScreen();
}
// Button: Button002 logic
static void Button002()
{
}


void GuiMenuMain(GuiMenuMainState *state)
{
    // Draw controls
    GuiGroupBox(state->layoutRecs[0], "Bouncing balls");
    if (GuiButton(state->layoutRecs[1], "PLAY")) Button001(); 
    if (GuiButton(state->layoutRecs[2], "EXIT")) Button002(); 
}

#endif // GUI_MENU_MAIN_IMPLEMENTATION
