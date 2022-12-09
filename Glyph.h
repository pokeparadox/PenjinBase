/*
	Penjin is Copyright (c)2005, 2006, 2007, 2008, 2009, 2010 Kevin Winfield-Pantoja

	This file is part of Penjin.

	Penjin is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Penjin is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with Penjin.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef GLYPH_H
#define GLYPH_H

#include "Image.h"
#include <SDL/SDL_ttf.h>
#include "PenjinTypes.h"
#include "Colour.h"
#include <ctype.h>

namespace GlyphClass
{
    enum RENDER_MODES
    {
        QUICK,  //  fast with colourkey
        BOXED,  //  antialiased against bg box.
        NICE    //  anitaliased fully with alpha channel SLOW
    };
}
using namespace GlyphClass;
class Glyph
{
    public:
        Glyph();
        ~Glyph();

        void render();
        #ifdef PENJIN_SDL
            void render(SDL_Surface* screen);
        #endif
        void refresh(); //  Updates the glyph

        void setFont(TTF_Font* f){font = f;}            //  Set the pointer to the loaded Font object
        #ifdef PENJIN_3D
            void setPosition(Vector3df* p){position = p;}
        #else
            void setPosition(Vector2di* p){position = p;}
        #endif
        void setRenderMode(RENDER_MODES m){renderMode = m;}
        void setCharacter(CRchar c){character = c;}
        void setColour(const Colour& c){colour = c;}
        void setBgColour(const Colour& c){bgColour = c;}
        void setFontSize(CRuint s){fontSize = s;}

        Colour getColour()const{return colour;}
        Colour getBgColour()const{return bgColour;}
        uint getFontSize()const{return fontSize;}
        char getCharacter()const{return character;}
        uint getWidth()const{return glyph.getWidth();}      //  return character width
        uint getHeight()const{return glyph.getHeight();}    //  return chracter height.
        RENDER_MODES getRenderMode()const{return renderMode;}

        // updates the Glpyh with any changes occurred in the parent Text object
        // returns true if anything changed, false otherwise
        bool update(const Colour& col, const Colour& bg, const RENDER_MODES& mode);

    private:
        // Disabled to prevent double freed pointers.
        Glyph(const Glyph& noCopy);
        Glyph& operator=(const Glyph& noCopy);

        #ifdef PENJIN_SDL
            Image glyph;
        #else
            Texture glyph;
        #endif
        //  We can check the below params to see if glyph needs to be recreated.
        char character;
        uint fontSize;
        Colour colour;
        Colour  bgColour;
        TTF_Font* font;
        GlyphClass::RENDER_MODES renderMode;
        bool needsInit;
        //  This is just a position we move around
        #ifdef PENJIN_3D
            Vector3df* position;
        #else
            Vector2di* position;
        #endif
};

#endif // GLYPH_H
