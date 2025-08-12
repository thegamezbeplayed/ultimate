//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// rTexpacker v5.0 Atlas Descriptor Code exporter v5.0                          //
//                                                                              //
// more info and bugs-report:  github.com/raylibtech/rtools                     //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2019-2025 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define ATLAS_RAY_SHEET_IMAGE_PATH      "ray_sheet.png"
#define ATLAS_RAY_SHEET_SPRITE_COUNT    7

// Atlas sprite properties
typedef struct rtpAtlasSprite {
    const char *nameId;
    const char *tag;
    int originX, originY;
    int positionX, positionY;
    int sourceWidth, sourceHeight;
    int padding;
    bool trimmed;
    int trimRecX, trimRecY, trimRecWidth, trimRecHeight;
    int colliderType;
    int colliderPosX, colliderPosY, colliderSizeX, colliderSizeY;
} rtpAtlasSprite;

// Atlas sprites array
static rtpAtlasSprite rtpDescRaySheet[7] = {
    { "red_body_rhombus", "", 40, 40, 0, 0, 80, 80, 0, false, 0, 0, 80, 80, 0, 0, 0, 0, 0 },
    { "red_body_circle", "", 40, 40, 80, 0, 80, 80, 0, false, 0, 0, 80, 80, 0, 0, 0, 0, 0 },
    { "red_body_squircle", "", 40, 40, 160, 0, 80, 80, 0, false, 0, 0, 80, 80, 0, 0, 0, 0, 0 },
    { "red_body_square", "", 40, 40, 240, 0, 80, 80, 0, false, 0, 0, 80, 80, 0, 0, 0, 0, 0 },
    { "tile", "", 40, 40, 320, 0, 80, 80, 0, false, 0, 0, 80, 80, 0, 0, 0, 0, 0 },
    { "tile", "", 40, 40, 400, 0, 80, 80, 0, false, 0, 0, 80, 80, 0, 0, 0, 0, 0 },
    { "tile", "", 40, 40, 480, 0, 80, 80, 0, false, 0, 0, 80, 80, 0, 0, 0, 0, 0 },
};
