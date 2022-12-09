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
#ifndef INC_TEXTURE_H
#define INC_TEXTURE_H

#if defined (PENJIN_GL) || defined (PENJIN_SOFT) || defined (PENJIN_ES) || defined (PENJIN_ES2)
#ifdef PENJIN_GL
    #include <SDL/SDL_opengl.h>
#elif PENJIN_SOFT
    //  GLenem...
    enum GLenum
    {
        GL_RGBA,
        GL_BGRA,
        GL_RGB,
        GL_BGR
    };
#elif PENJIN_ES || defined(PENJIN_ES2)
    #include <SDL/SDL.h>
    #include <GLES/gl.h>
    #include <GLES/egl.h>
#endif
#include <SDL/SDL_image.h>
#include "NumberUtility.h"
#include "PenjinTypes.h"
#include "PenjinErrors.h"
#include "GFX.h"
using namespace PenjinErrors;

class Texture
{
	public :
        Texture();
        Texture(CRstring fileName);
        ~Texture();

        Colour getTransparentColour()const{return colourKey;}
        void setTransparentColour(const Colour& c){colourKey = c;}

        PENJIN_ERRORS loadSurface(SDL_Surface* surface);//{return loadSurface(surface,true);}            //  Load a texture from a surface
        //PENJIN_ERRORS loadSurfaceNoKey(SDL_Surface* surface){return loadSurface(surface,false);}
        PENJIN_ERRORS loadTexture(CRstring fileName);//{return loadTexture(fileName,true);}               //  Load a texture from an image file
        //PENJIN_ERRORS loadTextureNoKey(CRstring imageName){return loadTexture(imageName,false);}       // Load a texture with no colourkey

        bool isLoaded()const{return loaded;}                //  Check if the texture is still taking up mem by OpenGL

        void setTextureID(CRuint textureID);             //  If you have created a texture elsewhere you can pass in
                                                                //    the ID so that it will be cleaned up
        uint getTextureID()const{return textureID;}

        int getWidth()const{return dimensions.x;}
        int getHeight()const{return dimensions.y;}
        void setDimensions(const Vector2di& dims){dimensions = dims;}  // overide dimensions
        Vector2di getDimensions()const{return dimensions;}
        void setWidth(const int& w){dimensions.x = w;}
        void setHeight(const int& h){dimensions.y = h;}

        //  Get the RAW dimensions
        int getRawWidth()const{return rawDimensions.x;}
        int getRawHeight()const{return rawDimensions.y;}
        Vector2di getRawDimensions()const{return rawDimensions;}

        Point2df getTextureCoords()const{return txCoords;}

	private :
        void init();
        //PENJIN_ERRORS loadSurface(SDL_Surface* screen, CRbool keyed);
        //PENJIN_ERRORS loadTexture(CRstring file, CRbool keyed);
        GLenum getTextureFormat(SDL_Surface* surface);  // Get the proper OpenGL format of the data stored in an SDL surface
        /// Check if a value is a power of two
        bool isPoT(const int& x)const{return NumberUtility::isPowerOfTwo(x);}
		bool loaded;
		uint textureID;
        Vector2di rawDimensions;    // stores the raw width and height in pixels of the image.
        Vector2di dimensions;       // stroes the actual wanted image dimensions
        Point2df txCoords;         // stores the caclulated tx coordinates
        Colour colourKey;
};
#endif
#endif
