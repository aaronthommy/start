/**********************************************************************************************
*
* rtextures - Basic functions to deal with textures
*
* LICENSE: zlib/libpng
*
* Copyright (c) 2014-2024 Ramon Santamaria (@raysan5)
*
* This software is provided "as-is", without any express or implied warranty. In no event
* will the authors be held liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose, including commercial
* applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be mis-represented; you must not claim that you
* wrote the original software. If you use this software in a product, an acknowledgment
* in the product documentation would be appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* mis-represented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RTEXTURES_H
#define RTEXTURES_H

#include "raylib.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Draw a Texture tiled (till window limits)
void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint);

#ifdef __cplusplus
}
#endif


#if defined(RTEXTURES_IMPLEMENTATION)

#include "raymath.h"        // Required for: Vector2Add(), Vector2Scale()

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Draw a Texture tiled (till window limits)
void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint)
{
    if ((texture.id <= 0) || (scale <= 0.0f)) return;
    if ((source.width == 0) || (source.height == 0)) return;

    int tileWidth = (int)(source.width*scale), tileHeight = (int)(source.height*scale);
    if ((dest.width < tileWidth) && (dest.height < tileHeight))
    {
        // Case 1: Tiled texture is smaller than destination rectangle
        DrawTexturePro(texture, (Rectangle){ source.x, source.y, (dest.width/scale), (dest.height/scale) }, dest, origin, rotation, tint);
    }
    else if (dest.width <= tileWidth)
    {
        // Case 2: Tiled texture width is smaller than destination rectangle
        int dy = 0;
        for (dy = 0; dy+tileHeight < dest.height; dy += tileHeight)
        {
            DrawTexturePro(texture, (Rectangle){ source.x, source.y, dest.width/scale, source.height }, (Rectangle){ dest.x, dest.y + dy, dest.width, (float)tileHeight }, origin, rotation, tint);
        }

        // Draw remaining texture
        if (dy < dest.height)
        {
            DrawTexturePro(texture, (Rectangle){ source.x, source.y, dest.width/scale, (dest.height - dy)/scale }, (Rectangle){ dest.x, dest.y + dy, dest.width, dest.height - dy }, origin, rotation, tint);
        }
    }
    else if (dest.height <= tileHeight)
    {
        // Case 3: Tiled texture height is smaller than destination rectangle
        int dx = 0;
        for (dx = 0; dx+tileWidth < dest.width; dx += tileWidth)
        {
            DrawTexturePro(texture, (Rectangle){ source.x, source.y, source.width, dest.height/scale }, (Rectangle){ dest.x + dx, dest.y, (float)tileWidth, dest.height }, origin, rotation, tint);
        }

        // Draw remaining texture
        if (dx < dest.width)
        {
            DrawTexturePro(texture, (Rectangle){ source.x, source.y, (dest.width - dx)/scale, dest.height/scale }, (Rectangle){ dest.x + dx, dest.y, dest.width - dx, dest.height }, origin, rotation, tint);
        }
    }
    else
    {
        // Case 4: Tiled texture is bigger than destination rectangle
        int dx = 0, dy = 0;

        for (dy = 0; dy+tileHeight < dest.height; dy += tileHeight)
        {
            for (dx = 0; dx+tileWidth < dest.width; dx += tileWidth)
            {
                DrawTexturePro(texture, source, (Rectangle){ dest.x + dx, dest.y + dy, (float)tileWidth, (float)tileHeight }, origin, rotation, tint);
            }

            // Draw remaining texture tile (x)
            if (dx < dest.width)
            {
                DrawTexturePro(texture, (Rectangle){ source.x, source.y, (dest.width - dx)/scale, source.height }, (Rectangle){ dest.x + dx, dest.y + dy, dest.width - dx, (float)tileHeight }, origin, rotation, tint);
            }
        }

        // Draw remaining texture tile (y)
        if (dy < dest.height)
        {
            for (dx = 0; dx+tileWidth < dest.width; dx += tileWidth)
            {
                DrawTexturePro(texture, (Rectangle){ source.x, source.y, source.width, (dest.height - dy)/scale }, (Rectangle){ dest.x + dx, dest.y + dy, (float)tileWidth, dest.height - dy }, origin, rotation, tint);
            }

            // Draw remaining texture (xy)
            if (dx < dest.width)
            {
                DrawTexturePro(texture, (Rectangle){ source.x, source.y, (dest.width - dx)/scale, (dest.height - dy)/scale }, (Rectangle){ dest.x + dx, dest.y + dy, dest.width - dx, dest.height - dy }, origin, rotation, tint);
            }
        }
    }
}

#endif // RTEXTURES_IMPLEMENTATION

#endif // RTEXTURES_H