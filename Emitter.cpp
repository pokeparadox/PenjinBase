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
#include "Emitter.h"
#include "CollisionRegion.h"

Emitter::Emitter()
{
    animSprite = NULL;
    sprite = NULL;
    colour.setColour(WHITE);
    repeat = false;
    stopNew = false;
    fade = 0;
    fadeValue = 0;
    #ifndef PENJIN_3D
        acceleration = Vector2df(0,0);
        gravity = position = acceleration;
        maxVelocity = Vector2df(3,3);
    #else
        acceleration = Vector3df(0,0,0);
        gravity = position = acceleration;
        maxVelocity = Vector3df(3,3,3);
    #endif
    deviance = 0;
    lifeTime = 1000;
    max = 1000;
    #ifdef PLATFORM_GP2X
        clipBoundary.w = 320;
        clipBoundary.h = 240;
    #elif  PLATFORM_PANDORA
        clipBoundary.w = 800;
        clipBoundary.h = 480;
    #else
        clipBoundary.w = 640;
        clipBoundary.h = 480;
    #endif
    clipBoundary.x = 0;
    clipBoundary.y = 0;
    finished = false;

    particles = NULL;
    invisible = 0;
    partCol = NULL;
    partCol = new CollisionRegion;
    partCol->setPosition(0,0);
    partCol->setWidth(1);
    partCol->setHeight(1);
    #ifdef PENJIN_SDL
        screen = GFX::getVideoSurface();
    #endif
    emitTimer.setMode(MILLI_SECONDS);
    emitTimer.start();
}

Emitter::~Emitter()
{
    //  Tidy up sprite objects if used...
    if(sprite)
    {
        delete sprite;
        sprite = NULL;
    }
    if(animSprite)
    {
        delete animSprite;
        animSprite = NULL;
    }
    if(particles)
    {
        for(int i = max-1; i >= 0; --i)
        {
            if(particles[i])
            {
                delete particles[i];
                particles[i] = NULL;
            }
        }
        delete [] particles;
        particles = NULL;
    }
    delete partCol;
}

PENJIN_ERRORS Emitter::loadSprite(CRstring filename)
{
    if(sprite)
    {
        delete sprite;
        sprite = NULL;
    }

    sprite = new Sprite;

    PENJIN_ERRORS result = sprite->loadSprite(filename);

    if(result == PENJIN_OK)
    {
        if(particles)
        {
            if(particles[0]->getParticleType() != SPRITE_PARTICLE)
                setParticlesType(SPRITE_PARTICLE);
        }
        else
            return PENJIN_ERROR;
        for (int i=max-1; i>=0; --i)
            ((SpriteParticle*)particles[i])->setSprite(sprite);
        partCol->loadImage(filename);
        partCol->generateHitRegion();
        partCol->freeImage();
    }
    return result;
}


PENJIN_ERRORS Emitter::loadAnimatedSprite(CRstring filename)
{
    if(animSprite)
    {
        delete animSprite;
        animSprite = NULL;
    }

    animSprite = new AnimatedSprite;
    animSprite->setFrameRate(FIFTEEN_FRAMES);
    animSprite->setLooping(true);

    PENJIN_ERRORS result = animSprite->loadFrame(filename);

    if(result == PENJIN_OK)
    {
        if(particles)
        {
            if(particles[0]->getParticleType() != ANIMATED_PARTICLE)
                setParticlesType(ANIMATED_PARTICLE);
        }
        else
            return PENJIN_ERROR;
        for (int i=max-1; i>=0; --i)
            ((AnimatedParticle*)particles[i])->setSprite(animSprite);
        partCol->loadImage(filename);
        partCol->generateHitRegion();
        partCol->freeImage();
    }
    return result;
}

PENJIN_ERRORS Emitter::loadAnimatedSprites(CRstring fileName,CRuint xTiles,CRuint yTiles)
{
    if(animSprite)
    {
        delete animSprite;
        animSprite = NULL;
    }

    animSprite = new AnimatedSprite;
    animSprite->setFrameRate(FIFTEEN_FRAMES);
    animSprite->setLooping(true);

    PENJIN_ERRORS result = animSprite->loadFrames(fileName, xTiles, yTiles);
    if(result == PENJIN_OK)
    {
        if(particles)
        {
            if(particles[0]->getParticleType() != ANIMATED_PARTICLE)
                setParticlesType(ANIMATED_PARTICLE);
        }
        else
            return PENJIN_ERROR;
        for (int i=max-1; i>=0; --i)
            ((AnimatedParticle*)particles[i])->setSprite(animSprite);
        partCol->loadImage(fileName,xTiles,yTiles);
        partCol->generateHitRegion();
        partCol->freeImage();
    }
    return result;
}

void Emitter::setParticlesType(const PARTICLE_TYPES& type)
{
    if(particles)
    {
        if(particles[0]->getParticleType() != type)
            clearParticles();
        else
            return; //  The particles are already the right type

        for(int i = max-1;i >= 0; --i)
        {
            particles[i] = NULL;
            switch(type)
            {
                case PARTICLE:
                    particles[i] = new Particle;
                break;
                case SPRITE_PARTICLE:
                    particles[i] = new SpriteParticle;
                break;
                case ANIMATED_PARTICLE:
                    particles[i] = new AnimatedParticle;
                break;
                case MODEL3D_PARTICLE:
                    cout << "unimplemented MODEL3D_PARTICLE" << endl;
                break;
            }
        }
    }
}

#ifdef PENJIN_SDL
    void Emitter::render(SDL_Surface* screen)
    {
        //  Run through all particles
        for(int i =max-1-invisible; i >= 0; --i)
        {
            particles[i]->render(screen);
            #ifdef _DEBUG
            //partCol->setPosition(particles[i]->getPosition());
            //partCol->render();
            #endif
        }
    }
#else
    void Emitter::render()
    {
        //  Run through all particles
        //glBegin(GL_POINTS);
        for(int i = max-1 - invisible; i >= 0; --i)
            particles[i]->render();

        //glEnd();
    }

#endif
void Emitter::setLifeTime(int ms)
{
    /// TODO Fix alpha fades somehow
    lifeTime = ms;

    if(ms == 0)
        ms = 255;

    fadeValue = ms/255.0f;
    emitTimer.start();
    for(int i = max-1; i >=0 ; --i)
    {
        reset(i);
        particles[i]->setLifeTime(Random::nextInt(0,lifeTime));
    }
}

void Emitter::advanceUpdate(CRuint n)
{
    for(int i = n; i>=0; --i)
    {
        /*if(invisible > 0)
        {
            --invisible;
            reset(max-1-invisible);
        }*/
        for(int i = max-1-invisible; i >= 0; --i)
        {
            particles[i]->advanceUpdate();

            if(fade)
            {
                if(particles[i]->getParticleType() != PARTICLE)
                {
                    //  Fade alpha instead of colour
                    #ifdef PENJIN_FIXED
                        particles[i]->changeAlpha(fadeValue.intValue<<16);
                    #else
                        particles[i]->changeAlpha(fadeValue);
                    #endif
                    uchar al= particles[i]->getAlpha();
                    if(al == SDL_ALPHA_TRANSPARENT)
                        reset(i);
                }
                else
                {
                    Colour col = particles[i]->getColour();
                    Colour tCol;
                    tCol.setColour((uchar)5,5,5);
                    if((col >= BLACK && col <= Colour(BLACK) + tCol) || (col <= WHITE && col >= Colour(WHITE) - tCol))
                        reset(i);
                    #ifdef PENJIN_FIXED
                        if(fade == 1)
                            particles[i]->changeColour(fadeValue);
                        else if (fade == 2)
                            particles[i]->changeAlpha(fixedpoint::fix2int(fadeValue));// should be alpha but that doesn't have a working routine yet.
                    #else
                        if(fade == 1)
                            particles[i]->changeColour(fadeValue);
                        else if (fade == 2)
                            particles[i]->changeAlpha(fadeValue);// should be alpha but that doesn't have a working routine yet.
                    #endif

                }
            }
            if(particles[i]->getLifeTime() >= lifeTime)
            {
                if(repeat)
                    reset(i);
                /*else
                {
                    if(invisible < max)
                        ++invisible;
                }*/
            }

            #ifdef PENJIN_3D
                Vector3df vel;
            #else
                Vector2df vel;
            #endif

            vel = particles[i]->getVelocity();
            if(vel.x <0)
                vel.x = -vel.x;
            if(vel.y <0)
                vel.y = -vel.y;

            if(particles[i]->getPosition().x < clipBoundary.x || particles[i]->getPosition().x > clipBoundary.w
            || particles[i]->getPosition().y < clipBoundary.y || particles[i]->getPosition().y > clipBoundary.h)
            {
                reset(i);
            }
        }
    }

}

void Emitter::update()
{
    if(emitTimer.getScaledTicks() > lifeTime)
    {
        finished = true;
        emitTimer.start();
        /*if(invisible > 0)
        {
            --invisible;
        }*/
    }

    for(int i = max-1/*-invisible*/; i >= 0; --i)
    {
        particles[i]->update();

        if(fade)
        {
            if(particles[i]->getParticleType() != PARTICLE)
            {
                //  Fade alpha instead of colour
                #ifdef PENJIN_FIXED
                    particles[i]->changeAlpha(fadeValue.intValue<<16);
                #else
                    particles[i]->changeAlpha(fadeValue);
                #endif
                uchar al= particles[i]->getAlpha();
                if(al == SDL_ALPHA_TRANSPARENT)
                    reset(i);
            }
            else
            {
                #ifdef PENJIN_FIXED
                    if(fade == 1)
                        particles[i]->changeColour(fadeValue);
                    else if (fade == 2)
                        particles[i]->changeAlpha(fadeValue.intValue<<16);// should be alpha but that doesn't have a working routine yet.
                #else
                    if(fade == 1)
                        particles[i]->changeColour(fadeValue);
                    else if (fade == 2)
                        particles[i]->changeAlpha(fadeValue);// should be alpha but that doesn't have a working routine yet.
                #endif
                Colour col = particles[i]->getColour();
                if((col == BLACK || col == WHITE))
                    reset(i);
            }
        }
        if(particles[i]->getLifeTime() >= lifeTime)
        {
            if(repeat)
                reset(i);
            /*else
            {
                if(invisible < max)
                    ++invisible;
            }*/
        }

        #ifdef PENJIN_3D
            Vector3df vel;
        #else
            Vector2df vel;
        #endif

        vel = particles[i]->getVelocity();
        if(vel.x <0)
            vel.x = -vel.x;
        if(vel.y <0)
            vel.y = -vel.y;

        if(particles[i]->getPosition().x - vel.x < clipBoundary.x || particles[i]->getPosition().x + vel.x > clipBoundary.w
        || particles[i]->getPosition().y - vel.y < clipBoundary.y || particles[i]->getPosition().y + vel.y > clipBoundary.h)
        {
                reset(i);
        }
    }
}

void Emitter::setMax(CRint max)
{
    this->max = max;
    clearParticles();
    for(int i = max-1; i >=0; --i)
    {
        particles[i] = NULL;
        particles[i] = new Particle();
    }

    invisible = 0;//(max-1)/6;
    partCol->setPosition(0,0);
    partCol->setSize(1,1);
}

void Emitter::clearParticles()
{
    if(particles)
    {
        for(int i = max-1; i >=0; --i)
        {
            if(particles[i])
            {
                delete particles[i];
                particles[i] = NULL;
            }
        }
        delete [] particles;
    }
    particles = NULL;
    particles = new Particle*[max];
}
