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
#ifndef ACHIEVEMENTSYSTEM_H
#define ACHIEVEMENTSYSTEM_H

///------------------------------
/// AchievementSystem class
/// by foxblock 10/12/09
///
/// The backbone of the Achievement system
/// Contains all achievements, monitors and updates progress and is used for renderring
///------------------------------


#define DEFAULT_ACHIEVEMENT_HEIGHT 74
#define DEFAULT_ACHIEVEMENT_WIDTH 300

#define DEFAULT_FONT_COLOUR WHITE
#define DEFAULT_HEADLINE_COLOUR 6563880
#define DEFAULT_BACKGROUND_COLOUR DARK_GREY
#define DEFAULT_PROGRESS_COLOUR 10526880

#define DEFAULT_FADE_TIME 500
#define DEFAULT_SHOW_TIME 2000

#define DEFAULT_SECRET_HEADLINE "???"
#define DEFAULT_SECRET_DESCRIPTION "It's a secret."

#include "Achievement.h"
#include "AchievementBoolean.h"
#include "AchievementCount.h"
#include "AchievementTime.h"
#include "AchievementReset.h"
#include "AchievementList.h"
#include "Event.h"

#include "Text.h"
#include "Colour.h"
#include "Timer.h"
#include "Vector2di.h"
#include "TextDoc.h"
#include "Encryption.h"
#include "PenjinErrors.h"

using namespace PenjinErrors;
using namespace std;

enum PopupPos
{
    ppTOPLEFT,
    ppTOPCENTER,
    ppTOPRIGHT,
    ppBOTTOMLEFT,
    ppBOTTOMCENTER,
    ppBOTTOMRIGHT
};

struct Popup
{
    Achievement* a;
    int time;
};

class AchievementSystem
{
    private:
        AchievementSystem();

        static AchievementSystem* m_self;
    public:
        ~AchievementSystem();
        static AchievementSystem* GetSingleton();

        // adds an achievent to the vector, what else...
        void addAchievement(Achievement* a){achievements.push_back(a); a->setParent(this);};

        // returns a reference to a single achievement, which can be used to render a customized list-view (for example)
        Achievement* getAchievement(CRint index) const {return achievements.at(index);};

        // different wrapper functions for logging an event using different levels of information
        void logEvent(CRint id, CRint count=1) {logEventSpecial(StringUtility::intToString(id),NULL,count);}
        void logEvent(CRstring name, CRint count=1) {logEventSpecial(name,NULL,count);}
        void logEventSpecial(CRint id, vector<SpecialProperty>* special, CRint count=1) {logEventSpecial(StringUtility::intToString(id),special,count);}
        void logEventSpecial(CRstring name, vector<SpecialProperty>* special, CRint count=1);

        // display and layout functions
        void setOffset(CRint newX, CRint newY) {setOffset(Vector2di(newX,newY));}; // offset for list-view
        void setOffset(const Vector2di& newOffset) {offset = newOffset;};
        void setSpacing(int newS) {spacing = newS;}; // spacing between single achievements in list-view
        Vector2di getAchievementSize() const {return achievementSize;}; // size of a single achievement in list-view
        void setAchievementSize(CRint newW, CRint newH) {setAchievementSize(Vector2di(newW,newH));};
        void setAchievementSize(const Vector2di& newSize) {achievementSize = newSize;};
        void setPopupSize(CRint newW, CRint newH) {setPopupSize(Vector2di(newW,newH));}; // size set size of achievement popups
        void setPopupSize(const Vector2di& newSize) {popupSize = newSize;};
        void setPopupPosition(PopupPos pos); // set position of the popup (screen borders)
        void setPopupFadeTime(CRint newFade) {fadeTime = newFade;};
        void setPopupShowTime(CRint newShow) {showTime = newShow;};
        void disablePopups() {showPopups = false;};
        void enablePopups() {showPopups = true;};
        bool showingPopups() const {return showPopups;};

        // display customisation
        void setFontColour(const Colour &col) {colours[coFontColour] = col;};
        void setHeadlineColour(const Colour &col) {colours[coHeadlineColour] = col;}; // achievement name
        void setBackgroundColour(const Colour &col) {colours[coBackgroundColour] = col;};
        void setProgressColour(const Colour &col) {colours[coProgressColour] = col;}; // progress-bar colour
        Colour getFontColour() const {return colours[coFontColour];};
        Colour getHeadlineColour() const {return colours[coHeadlineColour];};
        Colour getBackgroundColour() const {return colours[coBackgroundColour];};
        Colour getProgressColour() const {return colours[coProgressColour];};
        // text shown when a secret achievement is still locked
        void setSecretHeadline(CRstring str) {secretText[0] = str;};
        void setSecretDescription(CRstring str) {secretText[1] = str;};
        string getSecretHeadline() const {return secretText[0];};
        string getSecretDescription() const {return secretText[1];};

        // general functions
        int achievementCount() const {return achievements.size();};
        int unlockedCount() const;
        float getListSize() const; // number of achievements displayed in list-view with current offset settings and screen size
        #ifdef PENJIN_SDL
        void render(SDL_Surface* screen);
        void renderList(SDL_Surface* screen, float numOffset);
        #else
        void render();
        void renderList(float numOffset);
        #endif
        void update();

        void setAchievementFile(CRstring newFile) {achievementFile = newFile;};
        string getAchievementFile() const {return achievementFile;};
        PENJIN_ERRORS load(CRstring file);
        PENJIN_ERRORS load() {return load(achievementFile);};
        PENJIN_ERRORS save(CRstring file);
        PENJIN_ERRORS save() {return save(achievementFile);};
        PENJIN_ERRORS eraseData();

    private:
        vector<Event*> log;
        vector<Achievement*> achievements;
        vector<Popup> popups;
        Encryption crypt;
        TextDoc doc;
        string achievementFile;

        Vector2di offset; // offset for the list view (in pixels)
        Vector2di achievementSize; // size of a single achievement (list view)
        Vector2di popupSize; // size of a single achievement (popup)
        int spacing; // spacing between achievements (list view)
        Vector2di popup; // offset for a popup (in pixels), does not get accessed directly
        int fadeTime; // time it takes for a popup to fade in/out (in milli seconds)
        int showTime; // tile a popup is fully shown (in milli seconds)
        bool showTop; // popup position on the top of the screen
        bool showPopups; // show popups with milestone or unlock information
        Timer popupTimer;

        enum ColourOptions {
            coFontColour=0,
            coHeadlineColour,
            coBackgroundColour,
            coProgressColour
        };
        Colour colours[4];
        string secretText[2];
};

#endif // ACHIEVEMENTSYSTEM_H
