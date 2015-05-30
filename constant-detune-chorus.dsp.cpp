//-----------------------------------------------------
// name: "Chorus"
// version: "0.1"
// author: "Bart Brouns"
// license: "GNU 3.0"
// copyright: "(c) Bart Brouns 2014"
//
// Code generated with Faust 0.9.70 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with : "" */
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline float faustpower(float x)          { return powf(x,N); } 
template <int N> inline double faustpower(double x)        { return pow(x,N); }
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
#endif
/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections : 
	the ARCHITECTURE section (in two parts) and the USER section. Each section 
	is governed by its own copyright and license. Please check individually 
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 Thomas Charbonnel and GRAME
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it 
    and/or modify it under the terms of the GNU General Public License 
	as published by the Free Software Foundation; either version 3 of 
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License 
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work 
	that contains this FAUST architecture section and distribute  
	that work under terms of your choice, so long as this FAUST 
	architecture section is not modified. 


 ************************************************************************
 ************************************************************************/

#include <libgen.h>
#include <stdlib.h>
#include <iostream>
#include <list>

#ifndef FAUST_FUI_H
#define FAUST_FUI_H

#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust User Interface
 * This abstract class contains only the method that the faust compiler can
 * generate to describe a DSP interface.
 ******************************************************************************/

class UI
{

 public:

	UI() {}

	virtual ~UI() {}

    // -- widget's layouts

    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

    // -- passive widgets

    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;

	// -- metadata declarations

    virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};

#endif

#include <string>
#include <map>
#include <set>
#include <vector>
#include <stack>

#include <iostream>
#include <fstream>

//using namespace std;

#if 1

/*******************************************************************************
 * FUI : used to save and recall the state of the user interface
 * This class provides essentially two new methods saveState() and recallState()
 * used to save on file and recall from file the state of the user interface.
 * The file is human readble and editable
 ******************************************************************************/

class FUI : public UI
{
    
    std::stack<std::string>             fGroupStack;
	std::vector<std::string>            fNameList;
	std::map<std::string, FAUSTFLOAT*>	fName2Zone;

 protected:

 	// labels are normalized by replacing white spaces by underscores and by
 	// removing parenthesis
	std::string normalizeLabel(const char* label)
	{
		std::string 	s;
		char 	c;

		while ((c=*label++)) {
			if (isspace(c)) 				{ s += '_'; }
			//else if ((c == '(') | (c == ')') ) 	{ }
			else 							{ s += c; }
		}
		return s;
	}

	// add an element by relating its full name and memory zone
	virtual void addElement(const char* label, FAUSTFLOAT* zone)
	{
		std::string fullname (fGroupStack.top() + '/' + normalizeLabel(label));
		fNameList.push_back(fullname);
		fName2Zone[fullname] = zone;
	}

	// keep track of full group names in a stack
	virtual void pushGroupLabel(const char* label)
	{
		if (fGroupStack.empty()) {
			fGroupStack.push(normalizeLabel(label));
		} else {
			fGroupStack.push(fGroupStack.top() + '/' + normalizeLabel(label));
		}
	}

	virtual void popGroupLabel()
	{
		fGroupStack.pop();
	};

 public:

	FUI() 			{}
	virtual ~FUI() 	{}

	// -- Save and recall methods

	// save the zones values and full names
	virtual void saveState(const char* filename)
	{
		std::ofstream f(filename);

		for (unsigned int i=0; i<fNameList.size(); i++) {
			std::string	n = fNameList[i];
			FAUSTFLOAT*	z = fName2Zone[n];
			f << *z << ' ' << n.c_str() << std::endl;
		}

		f << std::endl;
		f.close();
	}

	// recall the zones values and full names
	virtual void recallState(const char* filename)
	{
		std::ifstream f(filename);
		FAUSTFLOAT	v;
		std::string	n;

		while (f.good()) {
			f >> v >> n;
			if (fName2Zone.count(n)>0) {
				*(fName2Zone[n]) = v;
			} else {
				std::cerr << "recallState : parameter not found : " << n.c_str() << " with value : " << v << std::endl;
			}
		}
		f.close();
	}


    // -- widget's layouts (just keep track of group labels)

    virtual void openTabBox(const char* label) 			{ pushGroupLabel(label); }
    virtual void openHorizontalBox(const char* label) 	{ pushGroupLabel(label); }
    virtual void openVerticalBox(const char* label)  	{ pushGroupLabel(label); }
    virtual void closeBox() 							{ popGroupLabel(); };

    // -- active widgets (just add an element)

    virtual void addButton(const char* label, FAUSTFLOAT* zone) 		{ addElement(label, zone); }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) 	{ addElement(label, zone); }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT)
    																{ addElement(label, zone); }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT)
    																{ addElement(label, zone); }
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT)
    																{ addElement(label, zone); }

    // -- passive widgets (are ignored)

    virtual void addHorizontalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT) {};
    virtual void addVerticalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT) {};

	// -- metadata are not used

    virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};
#endif

#endif

#ifndef FAUST_PUI_H
#define FAUST_PUI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#ifndef FAUST_PathUI_H
#define FAUST_PathUI_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathUI : Faust User Interface
 * Helper class to build complete path for items.
 ******************************************************************************/

class PathUI : public UI
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
};

#endif
#include <vector>
#include <string>

/*******************************************************************************
 * PrintUI : Faust User Interface
 * This class print arguments given to calls to UI methods and build complete path for labels.
 ******************************************************************************/

class PrintUI : public PathUI
{

    public:

        PrintUI() {}

        virtual ~PrintUI() {}

        // -- widget's layouts

        virtual void openTabBox(const char* label)
        {
            fControlsLevel.push_back(label);
            std::cout << "openTabBox label : " << label << std::endl;
        }
        virtual void openHorizontalBox(const char* label)
        {
            fControlsLevel.push_back(label);
            std::cout << "openHorizontalBox label : " << label << std::endl;
        }
        virtual void openVerticalBox(const char* label)
        {
            fControlsLevel.push_back(label);
            std::cout << "openVerticalBox label : " << label << std::endl;
        }
        virtual void closeBox()
        {
            fControlsLevel.pop_back();
            std::cout << "closeBox" << std::endl;
        }

        // -- active widgets

        virtual void addButton(const char* label, FAUSTFLOAT* zone)
        {
            std::cout << "addButton label : " << buildPath(label) << std::endl;
        }
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            std::cout << "addCheckButton label : " << buildPath(label) << std::endl;
        }
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            std::cout << "addVerticalSlider label : " << buildPath(label) << " init : " << init << " min : " << min << " max : " << max << " step : " << step << std::endl;
        }
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            std::cout << "addHorizontalSlider label : " << buildPath(label) << " init : " << init << " min : " << min << " max : " << max << " step : " << step << std::endl;
        }
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            std::cout << "addNumEntry label : " << buildPath(label) << " init : " << init << " min : " << min << " max : " << max << " step : " << step << std::endl;
        }

        // -- passive widgets

        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
        {
            std::cout << "addHorizontalBargraph label : " << buildPath(label) << " min : " << min << " max : " << max << std::endl;
        }
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
        {
            std::cout << "addVerticalBargraph label : " << buildPath(label) << " min : " << min << " max : " << max << std::endl;
        }

        // -- metadata declarations

        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {
            std::cout << "declare key : " << key << " val : " << val << std::endl;
        }
    
};

#endif
/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <string.h>
#include <stdlib.h>

/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __meta__
#define __meta__

struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
};

#endif


using std::max;
using std::min;

struct XXXX_Meta : std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

struct MY_Meta : Meta, std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

inline int	lsr(int x, int n)	{ return int(((unsigned int)x) >> n); }
inline int 	int2pow2(int x)		{ int r=0; while ((1<<r)<x) r++; return r; }

long lopt(char *argv[], const char *name, long def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return atoi(argv[i+1]);
	return def;
}

bool isopt(char *argv[], const char *name)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
	return false;
}

const char* lopts(char *argv[], const char *name, const char* def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return argv[i+1];
	return def;
}
#endif

#ifndef FAUST_GTKUI_H
#define FAUST_GTKUI_H

#ifndef FAUST_GUI_H
#define FAUST_GUI_H

#include <list>
#include <map>

/*******************************************************************************
 * GUI : Abstract Graphic User Interface
 * Provides additional macchanismes to synchronize widgets and zones. Widgets
 * should both reflect the value of a zone and allow to change this value.
 ******************************************************************************/

class uiItem;
typedef void (*uiCallback)(FAUSTFLOAT val, void* data);

class clist : public std::list<uiItem*>
{
    public:
    
        virtual ~clist();
        
};

class GUI : public UI
{
    
	typedef std::map<FAUSTFLOAT*, clist*> zmap;
	
 private:
 	static std::list<GUI*>	fGuiList;
	zmap                    fZoneMap;
	bool                    fStopped;
	
 public:
		
    GUI() : fStopped(false) 
    {	
		fGuiList.push_back(this);
	}
	
    virtual ~GUI() 
    {   
        // delete all 
        zmap::iterator g;
        for (g = fZoneMap.begin(); g != fZoneMap.end(); g++) {
            delete (*g).second;
        }
        // suppress 'this' in static fGuiList
        fGuiList.remove(this);
    }

	// -- registerZone(z,c) : zone management
	
	void registerZone(FAUSTFLOAT* z, uiItem* c)
	{
  		if (fZoneMap.find(z) == fZoneMap.end()) fZoneMap[z] = new clist();
		fZoneMap[z]->push_back(c);
	} 	

	void updateAllZones();
	
	void updateZone(FAUSTFLOAT* z);
	
	static void updateAllGuis()
	{
		std::list<GUI*>::iterator g;
		for (g = fGuiList.begin(); g != fGuiList.end(); g++) {
			(*g)->updateAllZones();
		}
	}
    void addCallback(FAUSTFLOAT* zone, uiCallback foo, void* data);
    virtual void show() {};	
    virtual void run() {};
	
	virtual void stop()		{ fStopped = true; }
	bool stopped() 	{ return fStopped; }

    virtual void declare(FAUSTFLOAT* , const char* , const char*) {}
};

/**
 * User Interface Item: abstract definition
 */

class uiItem
{
  protected :
		  
	GUI*            fGUI;
	FAUSTFLOAT*		fZone;
	FAUSTFLOAT		fCache;
	
	uiItem (GUI* ui, FAUSTFLOAT* zone) : fGUI(ui), fZone(zone), fCache(-123456.654321) 
	{ 
 		ui->registerZone(zone, this); 
 	}
	
  public :
  
	virtual ~uiItem() 
    {}
	
	void modifyZone(FAUSTFLOAT v) 	
	{ 
		fCache = v;
		if (*fZone != v) {
			*fZone = v;
			fGUI->updateZone(fZone);
		}
	}
		  	
	FAUSTFLOAT		cache()			{ return fCache; }
	virtual void 	reflectZone() 	= 0;	
};

/**
 * Callback Item
 */

struct uiCallbackItem : public uiItem
{
	uiCallback	fCallback;
	void*		fData;
	
	uiCallbackItem(GUI* ui, FAUSTFLOAT* zone, uiCallback foo, void* data) 
			: uiItem(ui, zone), fCallback(foo), fData(data) {}
	
	virtual void 	reflectZone() {		
		FAUSTFLOAT 	v = *fZone;
		fCache = v; 
		fCallback(v, fData);	
	}
};

// en cours d'installation de call back. a finir!!!!!

/**
 * Update all user items reflecting zone z
 */

inline void GUI::updateZone(FAUSTFLOAT* z)
{
	FAUSTFLOAT 	v = *z;
	clist* 	l = fZoneMap[z];
	for (clist::iterator c = l->begin(); c != l->end(); c++) {
		if ((*c)->cache() != v) (*c)->reflectZone();
	}
}

/**
 * Update all user items not up to date
 */

inline void GUI::updateAllZones()
{
	for (zmap::iterator m = fZoneMap.begin(); m != fZoneMap.end(); m++) {
		FAUSTFLOAT* z = m->first;
		clist*	l = m->second;
        if (z) {
            FAUSTFLOAT	v = *z;
            for (clist::iterator c = l->begin(); c != l->end(); c++) {
                if ((*c)->cache() != v) (*c)->reflectZone();
            }
        }
	}
}

inline void GUI::addCallback(FAUSTFLOAT* zone, uiCallback foo, void* data) 
{ 
	new uiCallbackItem(this, zone, foo, data); 
};

inline clist::~clist() 
{
    std::list<uiItem*>::iterator it;
    for (it = begin(); it != end(); it++) {
        delete (*it);
    }
}

#endif

/******************************************************************************
*******************************************************************************

                                GRAPHIC USER INTERFACE
                                  gtk interface

*******************************************************************************
*******************************************************************************/
#include <string>
#include <set>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <assert.h>

#define stackSize 256

// Insertion modes

#define kSingleMode 0
#define kBoxMode 1
#define kTabMode 2

static inline bool startWith(const std::string& str, const std::string& prefix)
{
    return (str.substr(0, prefix.size()) == prefix);
}

//------------ calculate needed precision
static int precision(double n)
{
	if (n < 0.009999) return 3;
	else if (n < 0.099999) return 2;
	else if (n < 0.999999) return 1;
	else return 0;
}

namespace gtk_knob
{

class GtkKnob
{
private:
	double start_x, start_y, max_value;
public:
	GtkRange parent;
	int last_quadrant;
	GtkKnob();
	~GtkKnob();
	GtkWidget* gtk_knob_new_with_adjustment(GtkAdjustment *_adjustment);
	
};

#define GTK_TYPE_KNOB          (gtk_knob_get_type())
#define GTK_KNOB(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_KNOB, GtkKnob))
#define GTK_IS_KNOB(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_KNOB))
#define GTK_KNOB_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass),  GTK_TYPE_KNOB, GtkKnobClass))
#define GTK_IS_KNOB_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GTK_TYPE_KNOB))

GtkKnob::GtkKnob()
// GtkKnob constructor
{}

GtkKnob::~GtkKnob()
{
	// Nothing specific to do...
}

struct GtkKnobClass {
	GtkRangeClass parent_class;
	int knob_x;
	int knob_y;
	int knob_step;
	int button_is;

};

//------forward declaration
GType gtk_knob_get_type ();

/****************************************************************
 ** calculate the knop pointer with dead zone
 */

const double scale_zero = 20 * (M_PI/180); // defines "dead zone" for knobs

static void knob_expose(GtkWidget* widget, int knob_x, int knob_y, GdkEventExpose *event, int arc_offset)
{
	/** check resize **/
	int grow;
	if(widget->allocation.width > widget->allocation.height) {
		grow = widget->allocation.height;
	} else {
		grow =  widget->allocation.width;
	}
	knob_x = grow-4;
	knob_y = grow-4;
	/** get values for the knob **/
	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));
	int knobx = (widget->allocation.x+2 + (widget->allocation.width-4 - knob_x) * 0.5);
	int knoby = (widget->allocation.y+2 + (widget->allocation.height-4 - knob_y) * 0.5);
	int knobx1 = (widget->allocation.x+2 + (widget->allocation.width-4)* 0.5);
	int knoby1 = (widget->allocation.y+2 + (widget->allocation.height-4) * 0.5);
	double knobstate = (adj->value - adj->lower) / (adj->upper - adj->lower);
	double angle = scale_zero + knobstate * 2 * (M_PI - scale_zero);
	double knobstate1 = (0. - adj->lower) / (adj->upper - adj->lower);
	double pointer_off = knob_x/6;
	double radius = std::min(knob_x-pointer_off, knob_y-pointer_off) / 2;
	double lengh_x = (knobx+radius+pointer_off/2) - radius * sin(angle);
	double lengh_y = (knoby+radius+pointer_off/2) + radius * cos(angle);
	double radius1 = std::min(knob_x, knob_y) / 2 ;

	/** get widget forground color convert to cairo **/
	GtkStyle *style = gtk_widget_get_style (widget);
	double r = std::min(0.6,style->fg[gtk_widget_get_state(widget)].red/65535.0),
		   g = std::min(0.6,style->fg[gtk_widget_get_state(widget)].green/65535.0),
		   b = std::min(0.6,style->fg[gtk_widget_get_state(widget)].blue/65535.0);

	/** paint focus **/
	if (GTK_WIDGET_HAS_FOCUS(widget)== TRUE) {
		gtk_paint_focus(widget->style, widget->window, GTK_STATE_NORMAL, NULL, widget, NULL,
		                knobx-2, knoby-2, knob_x+4, knob_y+4);
	}
	/** create clowing knobs with cairo **/
	cairo_t *cr = gdk_cairo_create(GDK_DRAWABLE(widget->window));
	GdkRegion *region;
	region = gdk_region_rectangle (&widget->allocation);
	gdk_region_intersect (region, event->region);
	gdk_cairo_region (cr, region);
	cairo_clip (cr);
	
	cairo_arc(cr,knobx1+arc_offset, knoby1+arc_offset, knob_x/2.1, 0, 2 * M_PI );
	cairo_pattern_t*pat =
		cairo_pattern_create_radial (knobx1+arc_offset-knob_x/6,knoby1+arc_offset-knob_x/6, 1,knobx1+arc_offset,knoby1+arc_offset,knob_x/2.1 );
	if(adj->lower<0 && adj->value>0.) {
		cairo_pattern_add_color_stop_rgb (pat, 0, r+0.4, g+0.4 + knobstate-knobstate1, b+0.4);
		cairo_pattern_add_color_stop_rgb (pat, 0.7, r+0.15, g+0.15 + (knobstate-knobstate1)*0.5, b+0.15);
		cairo_pattern_add_color_stop_rgb (pat, 1, r, g, b);
	} else if(adj->lower<0 && adj->value<=0.) {
		cairo_pattern_add_color_stop_rgb (pat, 0, r+0.4 +knobstate1- knobstate, g+0.4, b+0.4);
		cairo_pattern_add_color_stop_rgb (pat, 0.7, r+0.15 +(knobstate1- knobstate)*0.5, g+0.15, b+0.15);
		cairo_pattern_add_color_stop_rgb (pat, 1, r, g, b);
	} else {
		cairo_pattern_add_color_stop_rgb (pat, 0, r+0.4, g+0.4 +knobstate, b+0.4);
		cairo_pattern_add_color_stop_rgb (pat, 0.7, r+0.15, g+0.15 + knobstate*0.5, b+0.15);
		cairo_pattern_add_color_stop_rgb (pat, 1, r, g, b);
	}
	cairo_set_source (cr, pat);
	cairo_fill_preserve (cr);
	gdk_cairo_set_source_color(cr, gtk_widget_get_style (widget)->fg);
	cairo_set_line_width(cr, 2.0);
	cairo_stroke(cr);

	/** create a rotating pointer on the kob**/
	cairo_set_source_rgb(cr,  0.1, 0.1, 0.1);
	cairo_set_line_width(cr,std::max(3, std::min(7, knob_x/15)));
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND); 
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
	cairo_move_to(cr, knobx+radius1, knoby+radius1);
	cairo_line_to(cr,lengh_x,lengh_y);
	cairo_stroke(cr);
	cairo_set_source_rgb(cr,  0.9, 0.9, 0.9);
	cairo_set_line_width(cr,std::min(5, std::max(1,knob_x/30)));
	cairo_move_to(cr, knobx+radius1, knoby+radius1);
	cairo_line_to(cr,lengh_x,lengh_y);
	cairo_stroke(cr);
	cairo_pattern_destroy (pat);
	gdk_region_destroy (region);
	cairo_destroy(cr);
}

/****************************************************************
 ** general expose events for all "knob" controllers
 */

//----------- draw the Knob when moved
static gboolean gtk_knob_expose (GtkWidget* widget, GdkEventExpose *event)
{
	g_assert(GTK_IS_KNOB(widget));
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	knob_expose(widget, klass->knob_x, klass->knob_y, event, 0);
	return TRUE;
}

/****************************************************************
 ** set initial size for GdkDrawable per type
 */

static void gtk_knob_size_request (GtkWidget* widget, GtkRequisition *requisition)
{
	g_assert(GTK_IS_KNOB(widget));
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	requisition->width = klass->knob_x;
	requisition->height = klass->knob_y;
}

/****************************************************************
 ** set value from key bindings
 */

static void gtk_knob_set_value (GtkWidget* widget, int dir_down)
{
	g_assert(GTK_IS_KNOB(widget));

	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));

	int oldstep = (int)(0.5f + (adj->value - adj->lower) / adj->step_increment);
	int step;
	int nsteps = (int)(0.5f + (adj->upper - adj->lower) / adj->step_increment);
	if (dir_down)
		step = oldstep - 1;
	else
		step = oldstep + 1;
	FAUSTFLOAT value = adj->lower + step * double(adj->upper - adj->lower) / nsteps;
	gtk_widget_grab_focus(widget);
	gtk_range_set_value(GTK_RANGE(widget), value);
}

/****************************************************************
 ** keyboard bindings
 */

static gboolean gtk_knob_key_press (GtkWidget* widget, GdkEventKey *event)
{
	g_assert(GTK_IS_KNOB(widget));

	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));
	switch (event->keyval) {
	case GDK_Home:
		gtk_range_set_value(GTK_RANGE(widget), adj->lower);
		return TRUE;
	case GDK_End:
		gtk_range_set_value(GTK_RANGE(widget), adj->upper);
		return TRUE;
	case GDK_Up:
		gtk_knob_set_value(widget, 0);
		return TRUE;
	case GDK_Right:
		gtk_knob_set_value(widget, 0);
		return TRUE;
	case GDK_Down:
		gtk_knob_set_value(widget, 1);
		return TRUE;
	case GDK_Left:
		gtk_knob_set_value(widget, 1);
		return TRUE;
	}

	return FALSE;
}

/****************************************************************
 ** alternative (radial) knob motion mode (ctrl + mouse pressed)
 */

static void knob_pointer_event(GtkWidget* widget, gdouble x, gdouble y, int knob_x, int knob_y,
                               bool drag, int state)
{
	static double last_y = 2e20;
	GtkKnob *knob = GTK_KNOB(widget);
	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));
	double radius =  std::min(knob_x, knob_y) / 2;
	int  knobx = (widget->allocation.width - knob_x) / 2;
	int  knoby = (widget->allocation.height - knob_y) / 2;
	double posx = (knobx + radius) - x; // x axis right -> left
	double posy = (knoby + radius) - y; // y axis top -> bottom
	double value;
	if (!drag) {
		if (state & GDK_CONTROL_MASK) {
			last_y = 2e20;
			return;
		} else {
			last_y = posy;
		}
	}
	if (last_y < 1e20) { // in drag started with Control Key
		const double scaling = 0.005;
		double scal = (state & GDK_SHIFT_MASK ? scaling*0.1 : scaling);
		value = (last_y - posy) * scal;
		last_y = posy;
		gtk_range_set_value(GTK_RANGE(widget), adj->value - value * (adj->upper - adj->lower));
		return;
	}

	double angle = atan2(-posx, posy) + M_PI; // clockwise, zero at 6 o'clock, 0 .. 2*M_PI
	if (drag) {
		// block "forbidden zone" and direct moves between quadrant 1 and 4
		int quadrant = 1 + int(angle/M_PI_2);
		if (knob->last_quadrant == 1 && (quadrant == 3 || quadrant == 4)) {
			angle = scale_zero;
		} else if (knob->last_quadrant == 4 && (quadrant == 1 || quadrant == 2)) {
			angle = 2*M_PI - scale_zero;
		} else {
			if (angle < scale_zero) {
				angle = scale_zero;
			} else if (angle > 2*M_PI - scale_zero) {
				angle = 2*M_PI - scale_zero;
			}
			knob->last_quadrant = quadrant;
		}
	} else {
		if (angle < scale_zero) {
			angle = scale_zero;
		} else if (angle > 2*M_PI - scale_zero) {
			angle = 2*M_PI - scale_zero;
		}
		knob->last_quadrant = 0;
	}
	angle = (angle - scale_zero) / (2 * (M_PI-scale_zero)); // normalize to 0..1
	gtk_range_set_value(GTK_RANGE(widget), adj->lower + angle * (adj->upper - adj->lower));
}

/****************************************************************
 ** mouse button pressed set value
 */

static gboolean gtk_knob_button_press (GtkWidget* widget, GdkEventButton *event)
{
	g_assert(GTK_IS_KNOB(widget));
	
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	

	switch (event->button) {
	case 1:  // left button
		gtk_widget_grab_focus(widget);
		gtk_widget_grab_default (widget);
		gtk_grab_add(widget);
		klass->button_is = 1;
		knob_pointer_event(widget, event->x, event->y, klass->knob_x, klass->knob_y,
						   false, event->state);
		break;
	case 2: //wheel
		klass->button_is = 2;
		break;
	case 3:  // right button 
		klass->button_is = 3;
		break;
	default: // do nothing
		break;
	}
	return TRUE;
}

/****************************************************************
 ** mouse button release
 */

static gboolean gtk_knob_button_release (GtkWidget* widget, GdkEventButton *event)
{
	g_assert(GTK_IS_KNOB(widget));
	GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget))->button_is = 0;
	if (GTK_WIDGET_HAS_GRAB(widget))
		gtk_grab_remove(widget);
	return FALSE;
}

/****************************************************************
 ** set the value from mouse movement
 */

static gboolean gtk_knob_pointer_motion (GtkWidget* widget, GdkEventMotion *event)
{
	g_assert(GTK_IS_KNOB(widget));
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	
	gdk_event_request_motions (event);
	
	if (GTK_WIDGET_HAS_GRAB(widget)) {
		knob_pointer_event(widget, event->x, event->y, klass->knob_x, klass->knob_y,
						   true, event->state);
	}
	return FALSE;
}

/****************************************************************
 ** set value from mouseweel
 */

static gboolean gtk_knob_scroll (GtkWidget* widget, GdkEventScroll *event)
{
	usleep(5000);
	gtk_knob_set_value(widget, event->direction);
	return FALSE;
}

/****************************************************************
 ** init the GtkKnobClass
 */

static void gtk_knob_class_init (GtkKnobClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	/** set here the sizes and steps for the used knob **/
//--------- small knob size and steps
	
	klass->knob_x = 30;
	klass->knob_y = 30;
	klass->knob_step = 86;

//--------- event button
	klass->button_is = 0;

//--------- connect the events with funktions
	widget_class->expose_event = gtk_knob_expose;
	widget_class->size_request = gtk_knob_size_request;
	widget_class->button_press_event = gtk_knob_button_press;
	widget_class->button_release_event = gtk_knob_button_release;
	widget_class->motion_notify_event = gtk_knob_pointer_motion;
	widget_class->key_press_event = gtk_knob_key_press;
	widget_class->scroll_event = gtk_knob_scroll;
}

/****************************************************************
 ** init the Knob type/size
 */

static void gtk_knob_init (GtkKnob *knob)
{
	GtkWidget* widget = GTK_WIDGET(knob);
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));

	GTK_WIDGET_SET_FLAGS (GTK_WIDGET(knob), GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS (GTK_WIDGET(knob), GTK_CAN_DEFAULT);

	widget->requisition.width = klass->knob_x;
	widget->requisition.height = klass->knob_y;
}

/****************************************************************
 ** redraw when value changed
 */

static gboolean gtk_knob_value_changed(gpointer obj)
{
	GtkWidget* widget = (GtkWidget* )obj;
	gtk_widget_queue_draw(widget);
	return FALSE;
}

/****************************************************************
 ** create small knob
 */

GtkWidget* GtkKnob::gtk_knob_new_with_adjustment(GtkAdjustment *_adjustment)
{
	GtkWidget* widget = GTK_WIDGET( g_object_new (GTK_TYPE_KNOB, NULL ));
	GtkKnob *knob = GTK_KNOB(widget);
	knob->last_quadrant = 0;
	if (widget) {
		gtk_range_set_adjustment(GTK_RANGE(widget), _adjustment);
		g_signal_connect(GTK_OBJECT(widget), "value-changed",
		                 G_CALLBACK(gtk_knob_value_changed), widget);
	}
	return widget;
}

/****************************************************************
 ** get the Knob type
 */

GType gtk_knob_get_type (void)
{
	static GType kn_type = 0;
	if (!kn_type) {
		static const GTypeInfo kn_info = {
			sizeof(GtkKnobClass), NULL,  NULL, (GClassInitFunc)gtk_knob_class_init, NULL, NULL, sizeof(GtkKnob), 0, (GInstanceInitFunc)gtk_knob_init, NULL
		};
		kn_type = g_type_register_static(GTK_TYPE_RANGE,  "GtkKnob", &kn_info, (GTypeFlags)0);
	}
	return kn_type;
}
}/* end of gtk_knob namespace */

gtk_knob::GtkKnob myGtkKnob;

/**
 * rmWhiteSpaces(): Remove the leading and trailing white spaces of a string
 * (but not those in the middle of the string)
 */
static std::string rmWhiteSpaces(const std::string& s)
{
    size_t i = s.find_first_not_of(" \t");
    size_t j = s.find_last_not_of(" \t");

    if (i != std::string::npos & j != std::string::npos) {
        return s.substr(i, 1+j-i);
    } else {
        return "";
    }
}

/**
 * Extracts metdata from a label : 'vol [unit: dB]' -> 'vol' + metadata
 */
static void extractMetadata(const std::string& fulllabel, std::string& label, std::map<std::string, std::string>& metadata)
{
    enum {kLabel, kEscape1, kEscape2, kEscape3, kKey, kValue};
    int state = kLabel; int deep = 0;
    std::string key, value;

    for (unsigned int i=0; i < fulllabel.size(); i++) {
        char c = fulllabel[i];
        switch (state) {
            case kLabel :
                assert (deep == 0);
                switch (c) {
                    case '\\' : state = kEscape1; break;
                    case '[' : state = kKey; deep++; break;
                    default : label += c;
                }
                break;

            case kEscape1 :
                label += c;
                state = kLabel;
                break;

            case kEscape2 :
                key += c;
                state = kKey;
                break;

            case kEscape3 :
                value += c;
                state = kValue;
                break;

            case kKey :
                assert (deep > 0);
                switch (c) {
                    case '\\' :  state = kEscape2;
                                break;

                    case '[' :  deep++;
                                key += c;
                                break;

                    case ':' :  if (deep == 1) {
                                    state = kValue;
                                } else {
                                    key += c;
                                }
                                break;
                    case ']' :  deep--;
                                if (deep < 1) {
                                    metadata[rmWhiteSpaces(key)] = "";
                                    state = kLabel;
                                    key="";
                                    value="";
                                } else {
                                    key += c;
                                }
                                break;
                    default :   key += c;
                }
                break;

            case kValue :
                assert (deep > 0);
                switch (c) {
                    case '\\' : state = kEscape3;
                                break;

                    case '[' :  deep++;
                                value += c;
                                break;

                    case ']' :  deep--;
                                if (deep < 1) {
                                    metadata[rmWhiteSpaces(key)]=rmWhiteSpaces(value);
                                    state = kLabel;
                                    key="";
                                    value="";
                                } else {
                                    value += c;
                                }
                                break;
                    default :   value += c;
                }
                break;

            default :
                std::cerr << "ERROR unrecognized state " << state << std::endl;
        }
    }
    label = rmWhiteSpaces(label);
}

class GTKUI : public GUI
{
 private :
    static bool                         		fInitialized;
    static std::map<FAUSTFLOAT*, FAUSTFLOAT> 	fGuiSize;       // map widget zone with widget size coef
    static std::map<FAUSTFLOAT*, std::string>   fTooltip;       // map widget zone with tooltip strings
    static std::set<FAUSTFLOAT*>             	fKnobSet;       // set of widget zone to be knobs
	std::string									gGroupTooltip;
    
    bool isKnob(FAUSTFLOAT* zone) {return fKnobSet.count(zone) > 0;}
    
 protected :
    GtkWidget*  fWindow;
    int         fTop;
    GtkWidget*  fBox[stackSize];
    int         fMode[stackSize];
    bool        fStopped;

    GtkWidget* addWidget(const char* label, GtkWidget* w);
    virtual void pushBox(int mode, GtkWidget* w);

        
 public :
    static const gboolean expand = TRUE;
    static const gboolean fill = TRUE;
    static const gboolean homogene = FALSE;
         
    GTKUI(char * name, int* pargc, char*** pargv);

    // -- Labels and metadata

    virtual void declare(FAUSTFLOAT* zone, const char* key, const char* value);
    virtual int  checkLabelOptions(GtkWidget* widget, const std::string& fullLabel, std::string& simplifiedLabel);
    virtual void checkForTooltip(FAUSTFLOAT* zone, GtkWidget* widget);
    
    // -- layout groups
    
    virtual void openTabBox(const char* label = "");
    virtual void openHorizontalBox(const char* label = "");
    virtual void openVerticalBox(const char* label = "");
    virtual void closeBox();

    // -- active widgets
    
    virtual void addButton(const char* label, FAUSTFLOAT* zone);
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone);
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);   
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step); 
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);

    // -- passive display widgets
    
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max);
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max);
    
    // -- layout groups - internal
    
    virtual void openFrameBox(const char* label);   
   
    // -- extra widget's layouts

    virtual void openDialogBox(const char* label, FAUSTFLOAT* zone);
    virtual void openEventBox(const char* label = "");
    virtual void openHandleBox(const char* label = "");
    virtual void openExpanderBox(const char* label, FAUSTFLOAT* zone);
    
    virtual void adjustStack(int n);
    
    // -- active widgets - internal
    virtual void addToggleButton(const char* label, FAUSTFLOAT* zone);
    virtual void addKnob(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);
    
    // -- passive display widgets - internal
    
    virtual void addNumDisplay(const char* label, FAUSTFLOAT* zone, int precision);
    virtual void addTextDisplay(const char* label, FAUSTFLOAT* zone, const char* names[], FAUSTFLOAT min, FAUSTFLOAT max);
   
    virtual void show();
    virtual void run();
    
};

/******************************************************************************
*******************************************************************************

                                GRAPHIC USER INTERFACE (v2)
                                  gtk implementation

*******************************************************************************
*******************************************************************************/

// global static fields

bool                             GTKUI::fInitialized = false;
std::map<FAUSTFLOAT*, FAUSTFLOAT>     GTKUI::fGuiSize;
std::map<FAUSTFLOAT*, std::string>    GTKUI::fTooltip;
std::set<FAUSTFLOAT*>                 GTKUI::fKnobSet;       // set of widget zone to be knobs

/**
* Format tooltip string by replacing some white spaces by 
* return characters so that line width doesn't exceed n.
* Limitation : long words exceeding n are not cut 
*/
static std::string formatTooltip(unsigned int n, const std::string& tt)
{
	std::string  ss = tt;	// ss string we are going to format
	unsigned int lws = 0;	// last white space encountered
	unsigned int lri = 0;	// last return inserted
	for (unsigned int i = 0; i < tt.size(); i++) {
		if (tt[i] == ' ') lws = i;
		if (((i-lri) >= n) && (lws > lri)) {
			// insert return here
			ss[lws] = '\n';
			lri = lws;
		}
	}
	//std::cout << ss;
	return ss;
}

static gint delete_event(GtkWidget* widget, GdkEvent *event, gpointer data)
{
    return FALSE; 
}

static void destroy_event(GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}

GTKUI::GTKUI(char * name, int* pargc, char*** pargv) 
{
    if (!fInitialized) {
        gtk_init(pargc, pargv);
        fInitialized = true;
    }
    
    fWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    //gtk_container_set_border_width (GTK_CONTAINER (fWindow), 10);
    gtk_window_set_title (GTK_WINDOW (fWindow), name);
    gtk_signal_connect (GTK_OBJECT (fWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
    gtk_signal_connect (GTK_OBJECT (fWindow), "destroy", GTK_SIGNAL_FUNC (destroy_event), NULL);

    fTop = 0;
    fBox[fTop] = gtk_vbox_new (homogene, 4);
    fMode[fTop] = kBoxMode;
    gtk_container_add (GTK_CONTAINER (fWindow), fBox[fTop]);
    fStopped = false;
}

// empilement des boites

void GTKUI::pushBox(int mode, GtkWidget* w)
{
    ++fTop;
    assert(fTop < stackSize);
    fMode[fTop] = mode;
    fBox[fTop] = w;
}

/**
 * Remove n levels from the stack S before the top level
 * adjustStack(n): S -> S' with S' = S(0),S(n+1),S(n+2),...
 */
void GTKUI::adjustStack(int n)
{
    if (n > 0) {
        assert(fTop >= n);
        fTop -= n; 
        fMode[fTop] = fMode[fTop+n];
        fBox[fTop] = fBox[fTop+n];
    }
}

void GTKUI::closeBox()
{
    --fTop;
    assert(fTop >= 0);
}

/**
 * Analyses the widget zone metadata declarations and takes
 * appropriate actions 
 */
void GTKUI::declare(FAUSTFLOAT* zone, const char* key, const char* value)
{
	if (zone == 0) {
		// special zone 0 means group metadata
		if (strcmp(key,"tooltip") == 0) {
			// only group tooltip are currently implemented
			gGroupTooltip = formatTooltip(30, value);
		}
	} else {
		if (strcmp(key,"size")==0) {
			fGuiSize[zone] = atof(value);
		}
		else if (strcmp(key,"tooltip") == 0) {
			fTooltip[zone] = formatTooltip(30,value) ;
		}
		else if (strcmp(key,"style") == 0) {
			if (strcmp(value,"knob") == 0) {
				fKnobSet.insert(zone);
			}
		}
	}
}
    
/**
 * Analyses a full label and activates the relevant options. returns a simplified
 * label (without options) and an amount of stack adjustement (in case additional
 * containers were pushed on the stack). 
 */

int GTKUI::checkLabelOptions(GtkWidget* widget, const std::string& fullLabel, std::string& simplifiedLabel)
{   
    std::map<std::string, std::string> metadata;
    extractMetadata(fullLabel, simplifiedLabel, metadata);

    if (metadata.count("tooltip")) {
        gtk_tooltips_set_tip (gtk_tooltips_new (), widget, metadata["tooltip"].c_str(), NULL);
    }
    if (metadata["option"] == "detachable") {
        openHandleBox(simplifiedLabel.c_str());
        return 1;
    }

	//---------------------
	if (gGroupTooltip != std::string()) {
		gtk_tooltips_set_tip (gtk_tooltips_new (), widget, gGroupTooltip.c_str(), NULL);
		gGroupTooltip = std::string();
	}
	
	//----------------------
    // no adjustement of the stack needed
    return 0;
}

/**
 * Check if a tooltip is associated to a zone and add it to the corresponding widget
 */
void GTKUI::checkForTooltip(FAUSTFLOAT* zone, GtkWidget* widget)
{
    if (fTooltip.count(zone)) {
        gtk_tooltips_set_tip (gtk_tooltips_new (), widget, fTooltip[zone].c_str(), NULL);
    }
}

// The different boxes

void GTKUI::openFrameBox(const char* label)
{
    GtkWidget* box = gtk_frame_new (label);
    //gtk_container_set_border_width (GTK_CONTAINER (box), 10);
            
    pushBox(kSingleMode, addWidget(label, box));
}

void GTKUI::openTabBox(const char* fullLabel)
{
    std::string label;
    GtkWidget* widget = gtk_notebook_new();

    int adjust = checkLabelOptions(widget, fullLabel, label);
    
    pushBox(kTabMode, addWidget(label.c_str(), widget));

    // adjust stack because otherwise Handlebox will remain open
    adjustStack(adjust);
}

void GTKUI::openHorizontalBox(const char* fullLabel)
{   
    std::string label;
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    int adjust = checkLabelOptions(box, fullLabel, label);

    gtk_container_set_border_width (GTK_CONTAINER (box), 10);
    label = startWith(label, "0x") ? "" : label;
            
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label.c_str(), gtk_frame_new (label.c_str()));
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label.c_str(), box));
    }

    // adjust stack because otherwise Handlebox will remain open
    adjustStack(adjust);
}

void GTKUI::openVerticalBox(const char* fullLabel)
{
    std::string label;
    GtkWidget* box = gtk_vbox_new (homogene, 4);
    int adjust = checkLabelOptions(box, fullLabel, label);

    gtk_container_set_border_width (GTK_CONTAINER (box), 10);
    label = startWith(label, "0x") ? "" : label;
            
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label.c_str(), gtk_frame_new (label.c_str()));
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label.c_str(), box));
    }

    // adjust stack because otherwise Handlebox will remain open
    adjustStack(adjust);
}

void GTKUI::openHandleBox(const char* label)
{
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);
    label = startWith(label, "0x") ? "" : label;
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label, gtk_handle_box_new ());
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label, box));
    }
}

void GTKUI::openEventBox(const char* label)
{
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);
    label = startWith(label, "0x") ? "" : label;
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label, gtk_event_box_new ());
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label, box));
    }
}

struct uiExpanderBox : public uiItem
{
    GtkExpander* fButton;
    uiExpanderBox(GUI* ui, FAUSTFLOAT* zone, GtkExpander* b) : uiItem(ui, zone), fButton(b) {}
    static void expanded (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = gtk_expander_get_expanded  (GTK_EXPANDER(widget));
        if (v == 1.000000) {
            v = 0;
        } else {
            v = 1;
        }
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()
    {
        FAUSTFLOAT v = *fZone;
        fCache = v;
        gtk_expander_set_expanded(GTK_EXPANDER(fButton), v);
    }
};

void GTKUI::openExpanderBox(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);
    label = startWith(label, "0x") ? "" : label;
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label, gtk_expander_new (label));
        gtk_container_add (GTK_CONTAINER(frame), box);
        uiExpanderBox* c = new uiExpanderBox(this, zone, GTK_EXPANDER(frame));
        gtk_signal_connect (GTK_OBJECT (frame), "activate", GTK_SIGNAL_FUNC (uiExpanderBox::expanded), (gpointer)c);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label, box));
    }
}

GtkWidget* GTKUI::addWidget(const char* label, GtkWidget* w)
{ 
    switch (fMode[fTop]) {
        case kSingleMode    : gtk_container_add (GTK_CONTAINER(fBox[fTop]), w);                             break;
        case kBoxMode       : gtk_box_pack_start (GTK_BOX(fBox[fTop]), w, expand, fill, 0);                 break;
        case kTabMode       : gtk_notebook_append_page (GTK_NOTEBOOK(fBox[fTop]), w, gtk_label_new(label)); break;
    }
    gtk_widget_show (w);
    return w;
}

// --------------------------- Press button ---------------------------

struct uiButton : public uiItem
{
    GtkButton*  fButton;
    
    uiButton(GUI* ui, FAUSTFLOAT* zone, GtkButton* b) : uiItem(ui, zone), fButton(b) {}
    
    static void pressed(GtkWidget* widget, gpointer data)
    {
        uiItem* c = (uiItem*) data;
        c->modifyZone(1.0);
    }

    static void released(GtkWidget* widget, gpointer data)
    {
        uiItem* c = (uiItem*) data;
        c->modifyZone(0.0);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        if (v > 0.0) gtk_button_pressed(fButton); else gtk_button_released(fButton);
    }
};

void GTKUI::addButton(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* button = gtk_button_new_with_label (label);
    addWidget(label, button);
    
    uiButton* c = new uiButton(this, zone, GTK_BUTTON(button));
    
    gtk_signal_connect (GTK_OBJECT (button), "pressed", GTK_SIGNAL_FUNC (uiButton::pressed), (gpointer) c);
    gtk_signal_connect (GTK_OBJECT (button), "released", GTK_SIGNAL_FUNC (uiButton::released), (gpointer) c);

    checkForTooltip(zone, button);
}

// ---------------------------  Toggle Buttons ---------------------------

struct uiToggleButton : public uiItem
{
    GtkToggleButton* fButton;
    
    uiToggleButton(GUI* ui, FAUSTFLOAT* zone, GtkToggleButton* b) : uiItem(ui, zone), fButton(b) {}
    
    static void toggled (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = (GTK_TOGGLE_BUTTON (widget)->active) ? 1.0 : 0.0; 
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_toggle_button_set_active(fButton, v > 0.0); 
    }
};

void GTKUI::addToggleButton(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* button = gtk_toggle_button_new_with_label (label);
    addWidget(label, button);
    
    uiToggleButton* c = new uiToggleButton(this, zone, GTK_TOGGLE_BUTTON(button));
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC (uiToggleButton::toggled), (gpointer) c);

    checkForTooltip(zone, button);
}

void show_dialog(GtkWidget* widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget)) == TRUE)
    {
        gtk_widget_show(GTK_WIDGET(data));
        gint root_x, root_y;
        gtk_window_get_position (GTK_WINDOW(data), &root_x, &root_y);
        root_y -= 120;
        gtk_window_move(GTK_WINDOW(data), root_x, root_y);
    }
    else gtk_widget_hide(GTK_WIDGET(data));
}

static gboolean deleteevent( GtkWidget* widget, gpointer   data )
{
    return TRUE;
} 

void GTKUI::openDialogBox(const char* label, FAUSTFLOAT* zone)
{
    // create toplevel window and set properties
    GtkWidget* dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_deletable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_gravity(GTK_WINDOW(dialog), GDK_GRAVITY_SOUTH);
    gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(fWindow));
    gtk_window_set_position (GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
    gtk_window_set_keep_below (GTK_WINDOW(dialog), FALSE);
    gtk_window_set_title (GTK_WINDOW (dialog), label);
    g_signal_connect (G_OBJECT (dialog), "delete_event", G_CALLBACK (deleteevent), NULL); 
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    GtkWidget* box = gtk_hbox_new (homogene, 4);
 
    *zone = 0.0;
    GtkWidget* button = gtk_toggle_button_new ();
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC (show_dialog), (gpointer) dialog);
 
    gtk_container_add (GTK_CONTAINER(fBox[fTop]), button);
    gtk_container_add (GTK_CONTAINER(dialog), box);
    gtk_widget_show (button);
    gtk_widget_show(box);
    pushBox(kBoxMode, box);
}

// ---------------------------  Check Button ---------------------------

struct uiCheckButton : public uiItem
{
    GtkToggleButton* fButton;
    
    uiCheckButton(GUI* ui, FAUSTFLOAT* zone, GtkToggleButton* b) : uiItem(ui, zone), fButton(b) {}
    
    static void toggled (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = (GTK_TOGGLE_BUTTON (widget)->active) ? 1.0 : 0.0; 
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_toggle_button_set_active(fButton, v > 0.0); 
    }
};

void GTKUI::addCheckButton(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* button = gtk_check_button_new_with_label (label);
    addWidget(label, button);
    
    uiCheckButton* c = new uiCheckButton(this, zone, GTK_TOGGLE_BUTTON(button));
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC(uiCheckButton::toggled), (gpointer) c);

    checkForTooltip(zone, button);
}

// ---------------------------  Adjustmenty based widgets ---------------------------

struct uiAdjustment : public uiItem
{
    GtkAdjustment* fAdj;
    
    uiAdjustment(GUI* ui, FAUSTFLOAT* zone, GtkAdjustment* adj) : uiItem(ui, zone), fAdj(adj) {}
    
    static void changed (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = GTK_ADJUSTMENT (widget)->value; 
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_adjustment_set_value(fAdj, v);  
    }
};

// --------------------------- format knob value display ---------------------------

struct uiValueDisplay : public uiItem
{
	GtkLabel* fLabel;
	int	fPrecision ;

	uiValueDisplay(GUI* ui, FAUSTFLOAT* zone, GtkLabel* label, int precision)
		: uiItem(ui, zone), fLabel(label), fPrecision(precision) {}

	virtual void reflectZone()
		{
			FAUSTFLOAT v = *fZone;
			fCache = v;
			char s[64];
			if (fPrecision <= 0)
				snprintf(s, 63, "%d", int(v));

			else if (fPrecision > 3)
				snprintf(s, 63, "%f", v);

			else if (fPrecision == 1)
			{
				const char* format[] = {"%.1f", "%.2f", "%.3f"};
				snprintf(s, 63, format[1-1], v);
			}
			else if (fPrecision == 2)
			{
				const char* format[] = {"%.1f", "%.2f", "%.3f"};
				snprintf(s, 63, format[2-1], v);
			}
			else
			{
				const char* format[] = {"%.1f", "%.2f", "%.3f"};
				snprintf(s, 63, format[3-1], v);
			}
			gtk_label_set_text(fLabel, s);
		}
};

// ------------------------------- Knob -----------------------------------------

void GTKUI::addKnob(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	*zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, 0);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));

    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    
	GtkWidget* slider = gtk_vbox_new (FALSE, 0);
	GtkWidget* fil = gtk_vbox_new (FALSE, 0);
	GtkWidget* rei = gtk_vbox_new (FALSE, 0);
	GtkWidget* re = myGtkKnob.gtk_knob_new_with_adjustment(GTK_ADJUSTMENT(adj));
	GtkWidget* lw = gtk_label_new("");
	new uiValueDisplay(this, zone, GTK_LABEL(lw),precision(step));
	gtk_container_add (GTK_CONTAINER(rei), re);
	if(fGuiSize[zone]) {
		FAUSTFLOAT size = 30 * fGuiSize[zone];
		gtk_widget_set_size_request(rei, size, size );
		gtk_box_pack_start (GTK_BOX(slider), fil, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX(slider), rei, FALSE, FALSE, 0);
	} else {
		gtk_container_add (GTK_CONTAINER(slider), fil);
		gtk_container_add (GTK_CONTAINER(slider), rei);
	}
	gtk_container_add (GTK_CONTAINER(slider), lw);
	gtk_widget_show_all(slider);
	
    label = startWith(label, "0x") ? "" : label;
	if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, slider);
        closeBox();
    } else {
        addWidget(label, slider);
    }

    checkForTooltip(zone, slider);
}

// -------------------------- Vertical Slider -----------------------------------

void GTKUI::addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	if (isKnob(zone)) { 
		addKnob(label, zone, init, min, max, step);
		return;
	} 
    *zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, 0);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));

    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    
	GtkWidget* slider = gtk_vscale_new (GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(slider), precision(step));
	FAUSTFLOAT size = 160;
	if(fGuiSize[zone]) {
		size = 160 * fGuiSize[zone];
	}
	gtk_widget_set_size_request(slider, -1, size);
	
    gtk_range_set_inverted (GTK_RANGE(slider), TRUE);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, slider);
        closeBox();
    } else {
        addWidget(label, slider);
    }

    checkForTooltip(zone, slider);
}

// -------------------------- Horizontal Slider -----------------------------------

void GTKUI::addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	if (isKnob(zone)) { 
		addKnob(label, zone, init, min, max, step);
		return;
	} 
    *zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, 0);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));

    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    
    GtkWidget* slider = gtk_hscale_new (GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(slider), precision(step));
	FAUSTFLOAT size = 160;
	if(fGuiSize[zone]) {
		size = 160 * fGuiSize[zone];
	}
	gtk_widget_set_size_request(slider, size, -1);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, slider);
        closeBox();
    } else {
        addWidget(label, slider);
    }             

    checkForTooltip(zone, slider);
}

// ------------------------------ Num Entry -----------------------------------

void GTKUI::addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	if (isKnob(zone)) { 
		addKnob(label, zone, init, min, max, step);
		return;
	} 
    *zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, step);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));
    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    GtkWidget* spinner = gtk_spin_button_new (GTK_ADJUSTMENT(adj), 0.005, precision(step));
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, spinner);
        closeBox();
    } else {
        addWidget(label, spinner);
    }

    checkForTooltip(zone, spinner);
}

// ==========================   passive widgets ===============================

// ------------------------------ Progress Bar -----------------------------------

struct uiBargraph : public uiItem
{
    GtkProgressBar*     fProgressBar;
    FAUSTFLOAT          fMin;
    FAUSTFLOAT          fMax;
    
    uiBargraph(GUI* ui, FAUSTFLOAT* zone, GtkProgressBar* pbar, FAUSTFLOAT lo, FAUSTFLOAT hi) 
            : uiItem(ui, zone), fProgressBar(pbar), fMin(lo), fMax(hi) {}

    FAUSTFLOAT scale(FAUSTFLOAT v)        { return (v-fMin)/(fMax-fMin); }
    
    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_progress_bar_set_fraction(fProgressBar, scale(v));  
    }
};

void GTKUI::addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi)
{
    GtkWidget* pb = gtk_progress_bar_new();
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(pb), GTK_PROGRESS_BOTTOM_TO_TOP);
    gtk_widget_set_size_request(pb, 8, -1);
    new uiBargraph(this, zone, GTK_PROGRESS_BAR(pb), lo, hi);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, pb);
        closeBox();
    } else {
        addWidget(label, pb);
    }

    checkForTooltip(zone, pb);
}
    
void GTKUI::addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi)
{
    GtkWidget* pb = gtk_progress_bar_new();
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(pb), GTK_PROGRESS_LEFT_TO_RIGHT);
    gtk_widget_set_size_request(pb, -1, 8);
    new uiBargraph(this, zone, GTK_PROGRESS_BAR(pb), lo, hi);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, pb);
        closeBox();
    } else {
        addWidget(label, pb);
    }

    checkForTooltip(zone, pb);
}

// ------------------------------ Num Display -----------------------------------

struct uiNumDisplay : public uiItem
{
    GtkLabel* fLabel;
    int fPrecision;
    
    uiNumDisplay(GUI* ui, FAUSTFLOAT* zone, GtkLabel* label, int precision) 
            : uiItem(ui, zone), fLabel(label), fPrecision(precision) {}

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v;
        char s[64]; 
        if (fPrecision <= 0) { 
            snprintf(s, 63, "%d", int(v)); 
        } else if (fPrecision>3) {
            snprintf(s, 63, "%f", v);
        } else {
            const char* format[] = {"%.1f", "%.2f", "%.3f"};
            snprintf(s, 63, format[fPrecision-1], v);
        }
        gtk_label_set_text(fLabel, s);
    }
};

void GTKUI::addNumDisplay(const char* label, FAUSTFLOAT* zone, int precision)
{
    GtkWidget* lw = gtk_label_new("");
    new uiNumDisplay(this, zone, GTK_LABEL(lw), precision);
    openFrameBox(label);
    addWidget(label, lw);
    closeBox();

    checkForTooltip(zone, lw);
}

// ------------------------------ Text Display -----------------------------------

struct uiTextDisplay : public uiItem
{
    GtkLabel*       fLabel;
    const char**    fNames;
    FAUSTFLOAT      fMin;
    FAUSTFLOAT      fMax;
    int             fNum;
    
    uiTextDisplay (GUI* ui, FAUSTFLOAT* zone, GtkLabel* label, const char* names[], FAUSTFLOAT lo, FAUSTFLOAT hi)
                    : uiItem(ui, zone), fLabel(label), fNames(names), fMin(lo), fMax(hi)
    {
        fNum = 0;
        while (fNames[fNum] != 0) fNum++;
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v;
        
        int idx = int(fNum*(v-fMin)/(fMax-fMin));
        
        if      (idx < 0)       idx = 0; 
        else if (idx >= fNum)   idx = fNum-1;
                
        gtk_label_set_text(fLabel, fNames[idx]); 
    }
};
    
void GTKUI::addTextDisplay(const char* label, FAUSTFLOAT* zone, const char* names[], FAUSTFLOAT lo, FAUSTFLOAT hi)
{
    GtkWidget* lw = gtk_label_new("");
    new uiTextDisplay (this, zone, GTK_LABEL(lw), names, lo, hi);
    openFrameBox(label);
    addWidget(label, lw);
    closeBox();

    checkForTooltip(zone, lw);
}

void GTKUI::show() 
{
    assert(fTop == 0);
    gtk_widget_show  (fBox[0]);
    gtk_widget_show  (fWindow);
}

/**
 * Update all user items reflecting zone z
 */
    
static gboolean callUpdateAllGuis(gpointer)
{ 
    GUI::updateAllGuis(); 
    return TRUE;
}

void GTKUI::run() 
{
    assert(fTop == 0);
    gtk_widget_show  (fBox[0]);
    gtk_widget_show  (fWindow);
    gtk_timeout_add(40, callUpdateAllGuis, 0);
    gtk_main ();
    stop();
}

#endif

/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/
/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
#ifndef __jack_dsp__
#define __jack_dsp__

#include <stdio.h>
#include <cstdlib>
#include <list>
#include <string.h>
#include <jack/jack.h>
#ifdef JACK_IOS
#include <jack/custom.h>
#endif
/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
/******************************************************************************
*******************************************************************************

						An abstraction layer over audio layer

*******************************************************************************
*******************************************************************************/

#ifndef __audio__
#define __audio__
			
class dsp;

typedef void (* shutdown_callback)(const char* message, void* arg);

typedef void (* buffer_size_callback)(int frames, void* arg);

class audio {
    
 public:
			 audio() {}
	virtual ~audio() {}
	
	virtual bool init(const char* name, dsp*)               = 0;
	virtual bool start()                                    = 0;
	virtual void stop()                                     = 0;
    virtual void shutdown(shutdown_callback cb, void* arg)  {}
    
    virtual int get_buffer_size() = 0;
    virtual int get_sample_rate() = 0;
    
};
					
#endif
/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
/******************************************************************************
*******************************************************************************

								FAUST DSP

*******************************************************************************
*******************************************************************************/

#ifndef __dsp__
#define __dsp__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;

//----------------------------------------------------------------
//  signal processor definition
//----------------------------------------------------------------

class dsp {
 protected:
	int fSamplingFreq;
 public:
	dsp() {}
	virtual ~dsp() {}

	virtual int getNumInputs() 										= 0;
	virtual int getNumOutputs() 									= 0;
	virtual void buildUserInterface(UI* ui_interface) 				= 0;
	virtual void init(int samplingRate) 							= 0;
 	virtual void compute(int len, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) 	= 0;
};

// On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
// flags to avoid costly denormals
#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif

#if defined(_WIN32) && !defined(__MINGW32__)
#define snprintf _snprintf
#endif

/******************************************************************************
*******************************************************************************

							JACK AUDIO INTERFACE

*******************************************************************************
*******************************************************************************/

class jackaudio : public audio {
    
    protected:

        dsp*			fDsp;               // FAUST DSP
        jack_client_t*	fClient;            // JACK client
    
        int				fNumInChans;		// number of input channels
        int				fNumOutChans;       // number of output channels
    
        jack_port_t**	fInputPorts;        // JACK input ports
        jack_port_t**	fOutputPorts;       // JACK output ports
    
        shutdown_callback fShutdown;        // Shutdown callback to be called by libjack
        void*           fShutdownArg;       // Shutdown callback data
        void*           fIconData;          // iOS specific
        int             fIconSize;          // iOS specific
        bool            fAutoConnect;       // autoconnect with system in/out ports
        
        std::list<std::pair<std::string, std::string> > fConnections;		// Connections list
    
        static int _jack_srate(jack_nframes_t nframes, void* arg)
        {
            fprintf(stdout, "The sample rate is now %u/sec\n", nframes);
            return 0;
        }
        
        static void _jack_shutdown(void* arg) 
        {}
       
        static void _jack_info_shutdown(jack_status_t code, const char* reason, void* arg)
        {
            fprintf(stderr, "%s\n", reason);
            static_cast<jackaudio*>(arg)->shutdown(reason);
        }
        
        static int _jack_process(jack_nframes_t nframes, void* arg)
        {
            return static_cast<jackaudio*>(arg)->process(nframes);
        }
        
        static int _jack_buffersize(jack_nframes_t nframes, void* arg)
        {
            fprintf(stdout, "The buffer size is now %u/sec\n", nframes);
            return 0;
        }
     
        #ifdef _OPENMP
        static void* _jack_thread(void* arg)
        {
            jackaudio* audio = (jackaudio*)arg;
            audio->process_thread();
            return 0;
        }
        #endif
        
        void shutdown(const char* message)
        {
            fClient = NULL;
            
            if (fShutdown) {
                fShutdown(message, fShutdownArg);
            } else {
                exit(1); // By default
            }
        }
        
        // Save client connections
        void save_connections()
        {
            fConnections.clear();
            
             for (int i = 0; i < fNumInChans; i++) {
                const char** connected_port = jack_port_get_all_connections(fClient, fInputPorts[i]);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(connected_port[port], jack_port_name(fInputPorts[i])));
//                        printf("INPUT %s ==> %s\n", connected_port[port], jack_port_name(fInputPorts[i]));
                    }
                    jack_free(connected_port);
                }
            }
       
            for (int i = 0; i < fNumOutChans; i++) {
                const char** connected_port = jack_port_get_all_connections(fClient, fOutputPorts[i]);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(jack_port_name(fOutputPorts[i]), connected_port[port]));
//                        printf("OUTPUT %s ==> %s\n", jack_port_name(fOutputPorts[i]), connected_port[port]);
                    }
                    jack_free(connected_port);
                }
            }
        }

        // Load previous client connections
        void load_connections()
        {
            std::list<std::pair<std::string, std::string> >::const_iterator it;
            for (it = fConnections.begin(); it != fConnections.end(); it++) {
                std::pair<std::string, std::string> connection = *it;
                jack_connect(fClient, connection.first.c_str(), connection.second.c_str());
            }
        }

    public:
    
        jackaudio(const void* icon_data = 0, size_t icon_size = 0, bool auto_connect = true) 
            : fDsp(0), fClient(0), fNumInChans(0), fNumOutChans(0), 
            fInputPorts(0), fOutputPorts(0), 
            fShutdown(0), fShutdownArg(0),
            fAutoConnect(auto_connect)
        {
            if (icon_data) {
                fIconData = malloc(icon_size);
                fIconSize = icon_size;
                memcpy(fIconData, icon_data, icon_size);
            } else {
                fIconData = NULL;
                fIconSize = 0;
            }
        }
        
        virtual ~jackaudio() 
        { 
            if(fClient){
                stop();
                
                for (int i = 0; i < fNumInChans; i++) {
                    jack_port_unregister(fClient, fInputPorts[i]);
                }
                for (int i = 0; i < fNumOutChans; i++) {
                    jack_port_unregister(fClient, fOutputPorts[i]);
                }
                jack_client_close(fClient);
                
                delete[] fInputPorts;
                delete[] fOutputPorts;
                
                if (fIconData) {
                    free(fIconData);
                }
            }
        }
        
        jack_client_t* getClient() { return fClient; }

        virtual bool init(const char* name, dsp* DSP) 
        {
            if (!init(name)) {
                return false;
            }
            set_dsp(DSP);
            return true;
        }

        virtual bool init(const char* name) 
        {
            if ((fClient = jack_client_open(name, JackNullOption, NULL)) == 0) {
                fprintf(stderr, "JACK server not running ?\n");
                return false;
            }
        #ifdef JACK_IOS
            jack_custom_publish_data(fClient, "icon.png", fIconData, fIconSize);
        #endif
        
        #ifdef _OPENMP
            jack_set_process_thread(fClient, _jack_thread, this);
        #else
            jack_set_process_callback(fClient, _jack_process, this);
        #endif
        
            jack_set_sample_rate_callback(fClient, _jack_srate, this);
            jack_set_buffer_size_callback(fClient, _jack_buffersize, this);
            jack_on_info_shutdown(fClient, _jack_info_shutdown, this);
        
            return true;
        }    
    
        virtual bool set_dsp(dsp* DSP){
            fDsp = DSP;
            
            fNumInChans  = fDsp->getNumInputs();
            fNumOutChans = fDsp->getNumOutputs();
            
            fInputPorts = new jack_port_t*[fNumInChans];
            fOutputPorts = new jack_port_t*[fNumOutChans];
            
            for (int i = 0; i < fNumInChans; i++) {
                char buf[256];
                snprintf(buf, 256, "in_%d", i);
                fInputPorts[i] = jack_port_register(fClient, buf, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
            }
            for (int i = 0; i < fNumOutChans; i++) {
                char buf[256];
                snprintf(buf, 256, "out_%d", i);
                fOutputPorts[i] = jack_port_register(fClient, buf, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
            }
            
            fDsp->init(jack_get_sample_rate(fClient));
            return true;
        }
    
        virtual bool start() 
        {
            if (jack_activate(fClient)) {
                fprintf(stderr, "Cannot activate client");
                return false;
            }
            
            if (fConnections.size() > 0) {
                load_connections();
            } else if (fAutoConnect) {
                default_connections();
            }
            return true;
        }

        virtual void stop() 
        {
            if(fClient){

                save_connections();
                jack_deactivate(fClient);
            }
        }
    
        virtual void shutdown(shutdown_callback cb, void* arg)
        {
            fShutdown = cb;
            fShutdownArg = arg;
        }
        
        virtual int get_buffer_size() { return jack_get_buffer_size(fClient); }
        virtual int get_sample_rate() { return jack_get_sample_rate(fClient); }

        // jack callbacks
        virtual int	process(jack_nframes_t nframes) 
        {
            AVOIDDENORMALS;
            // Retrieve JACK inputs/output audio buffers
			float** fInChannel = (float**)alloca(fNumInChans*sizeof(float*));
            for (int i = 0; i < fNumInChans; i++) {
                fInChannel[i] = (float*)jack_port_get_buffer(fInputPorts[i], nframes);
            }
			float** fOutChannel = (float**)alloca(fNumOutChans*sizeof(float*));
            for (int i = 0; i < fNumOutChans; i++) {
                fOutChannel[i] = (float*)jack_port_get_buffer(fOutputPorts[i], nframes);
            }
            fDsp->compute(nframes, fInChannel, fOutChannel);
            return 0;
        }
    
        // Connect to physical inputs/outputs
        void default_connections()
        {
            // To avoid feedback at launch time, don't connect hardware inputs
            /*
            char** physicalOutPorts = (char**)jack_get_ports(fClient, NULL, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical|JackPortIsOutput);
            if (physicalOutPorts != NULL) {
                for (int i = 0; i < fNumInChans && physicalOutPorts[i]; i++) {
                jack_connect(fClient, physicalOutPorts[i], jack_port_name(fInputPorts[i]));
            }
                jack_free(physicalOutPorts);
            }
            */
            
            char** physicalInPorts = (char**)jack_get_ports(fClient, NULL, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical|JackPortIsInput);
            if (physicalInPorts != NULL) {
                for (int i = 0; i < fNumOutChans && physicalInPorts[i]; i++) {
                    jack_connect(fClient, jack_port_name(fOutputPorts[i]), physicalInPorts[i]);
                }
                jack_free(physicalInPorts);
            }
        }

    #ifdef _OPENMP
        void process_thread() 
        {
            jack_nframes_t nframes;
            while (1) {
                nframes = jack_cycle_wait(fClient);
                process(nframes);
                jack_cycle_signal(fClient, 0);
            }
        }
    #endif
};

#endif

#ifdef OSCCTRL
/*
   Copyright (C) 2011 Grame - Lyon
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted.
*/

#ifndef __OSCUI__
#define __OSCUI__

/*

  Faust Project

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __OSCControler__
#define __OSCControler__

#include <string>
/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __FaustFactory__
#define __FaustFactory__

#include <stack>
#include <string>
#include <sstream>

/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __FaustNode__
#define __FaustNode__

#include <string>
#include <vector>

/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __MessageDriven__
#define __MessageDriven__

#include <string>
#include <vector>

/*

  Copyright (C) 2010  Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __MessageProcessor__
#define __MessageProcessor__

namespace oscfaust
{

class Message;
//--------------------------------------------------------------------------
/*!
	\brief an abstract class for objects able to process OSC messages	
*/
class MessageProcessor
{
	public:
		virtual		~MessageProcessor() {}
		virtual void processMessage( const Message* msg ) = 0;
};

} // end namespoace

#endif
/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __smartpointer__
#define __smartpointer__

#include <cassert>

namespace oscfaust
{

/*!
\brief the base class for smart pointers implementation

	Any object that want to support smart pointers should
	inherit from the smartable class which provides reference counting
	and automatic delete when the reference count drops to zero.
*/
class smartable {
	private:
		unsigned 	refCount;		
	public:
		//! gives the reference count of the object
		unsigned refs() const         { return refCount; }
		//! addReference increments the ref count and checks for refCount overflow
		void addReference()           { refCount++; assert(refCount != 0); }
		//! removeReference delete the object when refCount is zero		
		void removeReference()		  { if (--refCount == 0) delete this; }
		
	protected:
		smartable() : refCount(0) {}
		smartable(const smartable&): refCount(0) {}
		//! destructor checks for non-zero refCount
		virtual ~smartable()    
        { 
            /* 
                See "Static SFaustNode create (const char* name, C* zone, C init, C min, C max, const char* prefix, GUI* ui)" comment.
                assert (refCount == 0); 
            */
         }
		smartable& operator=(const smartable&) { return *this; }
};

/*!
\brief the smart pointer implementation

	A smart pointer is in charge of maintaining the objects reference count 
	by the way of pointers operators overloading. It supports class 
	inheritance and conversion whenever possible.
\n	Instances of the SMARTP class are supposed to use \e smartable types (or at least
	objects that implements the \e addReference and \e removeReference
	methods in a consistent way).
*/
template<class T> class SMARTP {
	private:
		//! the actual pointer to the class
		T* fSmartPtr;

	public:
		//! an empty constructor - points to null
		SMARTP()	: fSmartPtr(0) {}
		//! build a smart pointer from a class pointer
		SMARTP(T* rawptr) : fSmartPtr(rawptr)              { if (fSmartPtr) fSmartPtr->addReference(); }
		//! build a smart pointer from an convertible class reference
		template<class T2> 
		SMARTP(const SMARTP<T2>& ptr) : fSmartPtr((T*)ptr) { if (fSmartPtr) fSmartPtr->addReference(); }
		//! build a smart pointer from another smart pointer reference
		SMARTP(const SMARTP& ptr) : fSmartPtr((T*)ptr)     { if (fSmartPtr) fSmartPtr->addReference(); }

		//! the smart pointer destructor: simply removes one reference count
		~SMARTP()  { if (fSmartPtr) fSmartPtr->removeReference(); }
		
		//! cast operator to retrieve the actual class pointer
		operator T*() const  { return fSmartPtr;	}

		//! '*' operator to access the actual class pointer
		T& operator*() const {
			// checks for null dereference
			assert (fSmartPtr != 0);
			return *fSmartPtr;
		}

		//! operator -> overloading to access the actual class pointer
		T* operator->() const	{ 
			// checks for null dereference
			assert (fSmartPtr != 0);
			return fSmartPtr;
		}

		//! operator = that moves the actual class pointer
		template <class T2>
		SMARTP& operator=(T2 p1_)	{ *this=(T*)p1_; return *this; }

		//! operator = that moves the actual class pointer
		SMARTP& operator=(T* p_)	{
			// check first that pointers differ
			if (fSmartPtr != p_) {
				// increments the ref count of the new pointer if not null
				if (p_ != 0) p_->addReference();
				// decrements the ref count of the old pointer if not null
				if (fSmartPtr != 0) fSmartPtr->removeReference();
				// and finally stores the new actual pointer
				fSmartPtr = p_;
			}
			return *this;
		}
		//! operator < to support SMARTP map with Visual C++
		bool operator<(const SMARTP<T>& p_)	const			  { return fSmartPtr < ((T *) p_); }
		//! operator = to support inherited class reference
		SMARTP& operator=(const SMARTP<T>& p_)                { return operator=((T *) p_); }
		//! dynamic cast support
		template<class T2> SMARTP& cast(T2* p_)               { return operator=(dynamic_cast<T*>(p_)); }
		//! dynamic cast support
		template<class T2> SMARTP& cast(const SMARTP<T2>& p_) { return operator=(dynamic_cast<T*>(p_)); }
};

}

#endif

namespace oscfaust
{

class Message;
class OSCRegexp;
class MessageDriven;
typedef class SMARTP<MessageDriven>	SMessageDriven;

//--------------------------------------------------------------------------
/*!
	\brief a base class for objects accepting OSC messages
	
	Message driven objects are hierarchically organized in a tree.
	They provides the necessary to dispatch an OSC message to its destination
	node, according to the message OSC address. 
	
	The principle of the dispatch is the following:
	- first the processMessage() method should be called on the top level node
	- next processMessage call propose 
	
	
*/
class MessageDriven : public MessageProcessor, public smartable
{
	std::string						fName;			///< the node name
	std::string						fOSCPrefix;		///< the node OSC address prefix (OSCAddress = fOSCPrefix + '/' + fName)
	std::vector<SMessageDriven>		fSubNodes;		///< the subnodes of the current node

	protected:
				 MessageDriven(const char *name, const char *oscprefix) : fName (name), fOSCPrefix(oscprefix) {}
		virtual ~MessageDriven() {}

	public:
		static SMessageDriven create (const char* name, const char *oscprefix)	{ return new MessageDriven(name, oscprefix); }

		/*!
			\brief OSC message processing method.
			\param msg the osc message to be processed
			The method should be called on the top level node.
		*/
		virtual void	processMessage( const Message* msg );

		/*!
			\brief propose an OSc message at a given hierarchy level.
			\param msg the osc message currently processed
			\param regexp a regular expression based on the osc address head
			\param addrTail the osc address tail
			
			The method first tries to match the regular expression with the object name. 
			When it matches:
			- it calls \c accept when \c addrTail is empty 
			- or it \c propose the message to its subnodes when \c addrTail is not empty. 
			  In this case a new \c regexp is computed with the head of \c addrTail and a new \c addrTail as well.
		*/
		virtual void	propose( const Message* msg, const OSCRegexp* regexp, const std::string addrTail);

		/*!
			\brief accept an OSC message. 
			\param msg the osc message currently processed
			\return true when the message is processed by the node
			
			The method is called only for the destination nodes. The real message acceptance is the node 
			responsability and may depend on the message content.
		*/
		virtual bool	accept( const Message* msg );

		/*!
			\brief handler for the \c 'get' message
			\param ipdest the output message destination IP
			
			The \c 'get' message is supported by every node:
			- it is propagated to the subnodes until it reaches terminal nodes
			- a terminal node send its state on \c 'get' request to the IP address given as parameter.
			The \c get method is basically called by the accept method.
		*/
		virtual void	get (unsigned long ipdest) const;

		/*!
			\brief handler for the \c 'get' 'attribute' message
			\param ipdest the output message destination IP
			\param what the requested attribute
			
			The \c 'get' message is supported by every node:
			- it is propagated to the subnodes until it reaches terminal nodes
			- a terminal node send its state on \c 'get' request to the IP address given as parameter.
			The \c get method is basically called by the accept method.
		*/
		virtual void	get (unsigned long ipdest, const std::string & what) const {}

		void			add ( SMessageDriven node )	{ fSubNodes.push_back (node); }
		const char*		getName() const				{ return fName.c_str(); }
		std::string		getOSCAddress() const;
		int				size () const				{ return fSubNodes.size (); }
		
		const std::string&	name() const			{ return fName; }
		SMessageDriven	subnode (int i) 			{ return fSubNodes[i]; }
};

} // end namespoace

#endif
/*

  Copyright (C) 2011  Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __Message__
#define __Message__

#include <string>
#include <vector>

namespace oscfaust
{

class OSCStream;
template <typename T> class MsgParam;
class baseparam;
typedef SMARTP<baseparam>	Sbaseparam;

//--------------------------------------------------------------------------
/*!
	\brief base class of a message parameters
*/
class baseparam : public smartable
{
	public:
		virtual ~baseparam() {}

		/*!
		 \brief utility for parameter type checking
		*/
		template<typename X> bool isType() const { return dynamic_cast<const MsgParam<X>*> (this) != 0; }
		/*!
		 \brief utility for parameter convertion
		 \param errvalue the returned value when no conversion applies
		 \return the parameter value when the type matches
		*/
		template<typename X> X	value(X errvalue) const 
			{ const MsgParam<X>* o = dynamic_cast<const MsgParam<X>*> (this); return o ? o->getValue() : errvalue; }
		/*!
		 \brief utility for parameter comparison
		*/
		template<typename X> bool	equal(const baseparam& p) const 
			{ 
				const MsgParam<X>* a = dynamic_cast<const MsgParam<X>*> (this); 
				const MsgParam<X>* b = dynamic_cast<const MsgParam<X>*> (&p);
				return a && b && (a->getValue() == b->getValue());
			}
		/*!
		 \brief utility for parameter comparison
		*/
		bool operator==(const baseparam& p) const 
			{ 
				return equal<float>(p) || equal<int>(p) || equal<std::string>(p);
			}
		bool operator!=(const baseparam& p) const
			{ 
				return !equal<float>(p) && !equal<int>(p) && !equal<std::string>(p);
			}
			
		virtual SMARTP<baseparam> copy() const = 0;
};

//--------------------------------------------------------------------------
/*!
	\brief template for a message parameter
*/
template <typename T> class MsgParam : public baseparam
{
	T fParam;
	public:
				 MsgParam(T val) : fParam(val)	{}
		virtual ~MsgParam() {}
		
		T	getValue() const { return fParam; }
		
		virtual SMARTP<baseparam> copy() const { return new MsgParam<T>(fParam); }
};

//--------------------------------------------------------------------------
/*!
	\brief a message description
	
	A message is composed of an address (actually an OSC address),
	a message string that may be viewed as a method name
	and a list of message parameters.
*/
class Message
{
	public:
		typedef SMARTP<baseparam>		argPtr;		///< a message argument ptr type
		typedef std::vector<argPtr>		argslist;	///< args list type

	private:
		unsigned long	fSrcIP;			///< the message source IP number
		std::string	fAddress;			///< the message osc destination address
		argslist	fArguments;			///< the message arguments
	
	public:
			/*!
				\brief an empty message constructor
			*/
			 Message() {}
			/*!
				\brief a message constructor
				\param address the message destination address
			*/
			 Message(const std::string& address) : fAddress(address) {}
			/*!
				\brief a message constructor
				\param address the message destination address
				\param args the message parameters
			*/
			 Message(const std::string& address, const argslist& args) 
				: fAddress(address), fArguments(args) {}
			/*!
				\brief a message constructor
				\param msg a message
			*/
			 Message(const Message& msg);
	virtual ~Message() {} //{ freed++; std::cout << "running messages: " << (allocated - freed) << std::endl; }

	/*!
		\brief adds a parameter to the message
		\param val the parameter
	*/
	template <typename T> void add(T val)	{ fArguments.push_back(new MsgParam<T>(val)); }
	/*!
		\brief adds a float parameter to the message
		\param val the parameter value
	*/
	void	add(float val)					{ add<float>(val); }
	/*!
		\brief adds an int parameter to the message
		\param val the parameter value
	*/
	void	add(int val)					{ add<int>(val); }
	/*!
		\brief adds a string parameter to the message
		\param val the parameter value
	*/
	void	add(const std::string& val)		{ add<std::string>(val); }
	
	/*!
		\brief adds a parameter to the message
		\param val the parameter
	*/
	void	add( argPtr val )				{ fArguments.push_back( val ); }

	/*!
		\brief sets the message address
		\param addr the address
	*/
	void				setSrcIP(unsigned long addr)		{ fSrcIP = addr; }

	/*!
		\brief sets the message address
		\param addr the address
	*/
	void				setAddress(const std::string& addr)		{ fAddress = addr; }
	/*!
		\brief print the message
		\param out the output stream
	*/
	void				print(std::ostream& out) const;
	/*!
		\brief send the message to OSC
		\param out the OSC output stream
	*/
	void				print(OSCStream& out) const;
	/*!
		\brief print message arguments
		\param out the OSC output stream
	*/
	void				printArgs(OSCStream& out) const;

	/// \brief gives the message address
	const std::string&	address() const		{ return fAddress; }
	/// \brief gives the message parameters list
	const argslist&		params() const		{ return fArguments; }
	/// \brief gives the message parameters list
	argslist&			params()			{ return fArguments; }
	/// \brief gives the message source IP 
	unsigned long		src() const			{ return fSrcIP; }
	/// \brief gives the message parameters count
	int					size() const		{ return fArguments.size(); }
	
	bool operator == (const Message& other) const;	


	/*!
		\brief gives a message float parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, float& val) const		{ val = params()[i]->value<float>(val); return params()[i]->isType<float>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, int& val) const		{ val = params()[i]->value<int>(val); return params()[i]->isType<int>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, unsigned int& val) const		{ val = params()[i]->value<int>(val); return params()[i]->isType<int>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
		\note a boolean value is handled as integer
	*/
	bool	param(int i, bool& val) const		{ int ival = 0; ival = params()[i]->value<int>(ival); val = ival!=0; return params()[i]->isType<int>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, long int& val) const	{ val = long(params()[i]->value<int>(val)); return params()[i]->isType<int>(); }
	/*!
		\brief gives a message string parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, std::string& val) const { val = params()[i]->value<std::string>(val); return params()[i]->isType<std::string>(); }
};


} // end namespoace

#endif

class GUI;

namespace oscfaust
{

/**
 * map (rescale) input values to output values
 */
template <typename C> struct mapping
{
//	const C fMinIn;	
//	const C fMaxIn;
	const C fMinOut;
	const C fMaxOut;
//	const C fScale;

//	mapping(C imin, C imax, C omin, C omax) : fMinIn(imin), fMaxIn(imax), 
//											fMinOut(omin), fMaxOut(omax), 
//											fScale( (fMaxOut-fMinOut)/(fMaxIn-fMinIn) ) {}
	mapping(C omin, C omax) : fMinOut(omin), fMaxOut(omax) {}
//	C scale (C x) { C z = (x < fMinIn) ? fMinIn : (x > fMaxIn) ? fMaxIn : x; return fMinOut + (z - fMinIn) * fScale; }
	C clip (C x) { return (x < fMinOut) ? fMinOut : (x > fMaxOut) ? fMaxOut : x; }
};

//--------------------------------------------------------------------------
/*!
	\brief a faust node is a terminal node and represents a faust parameter controler
*/
template <typename C> class FaustNode : public MessageDriven, public uiItem
{
	mapping<C>	fMapping;
	
	bool	store (C val)			{ *fZone = fMapping.clip(val); return true; }
	void	sendOSC () const;


	protected:
		FaustNode(const char *name, C* zone, C init, C min, C max, const char* prefix, GUI* ui, bool initZone) 
			: MessageDriven (name, prefix), uiItem (ui, zone), fMapping(min, max)
			{ 
                if(initZone)
                    *zone = init; 
            }
			
		virtual ~FaustNode() {}

	public:
		typedef SMARTP<FaustNode<C> > SFaustNode;
		static SFaustNode create (const char* name, C* zone, C init, C min, C max, const char* prefix, GUI* ui, bool initZone)	
        { 
            SFaustNode node = new FaustNode(name, zone, init, min, max, prefix, ui, initZone); 
            /*
                Since FaustNode is a subclass of uiItem, the pointer will also be kept in the GUI class, and it's desallocation will be done there.
                So we don't want to have smartpointer logic desallocate it and we increment the refcount.
            */
            node->addReference(); 
            return node; 
        }

		virtual bool	accept( const Message* msg )			///< handler for the 'accept' message
		{
			if (msg->size() == 1) {			// checks for the message parameters count
											// messages with a param count other than 1 are rejected
				int ival; float fval;
				if (msg->param(0, fval)) return store (C(fval));				// accepts float values
				else if (msg->param(0, ival)) return store (C(ival));	// but accepts also int values
			}
			return MessageDriven::accept(msg);
		}

		virtual void	get (unsigned long ipdest) const;		///< handler for the 'get' message
		virtual void 	reflectZone()			{ sendOSC (); fCache = *fZone;}
};



} // end namespoace

#endif

class GUI;
namespace oscfaust
{

class OSCIO;
class RootNode;
typedef class SMARTP<RootNode>		SRootNode;
class MessageDriven;
typedef class SMARTP<MessageDriven>	SMessageDriven;

//--------------------------------------------------------------------------
/*!
	\brief a factory to build a OSC UI hierarchy
	
	Actually, makes use of a stack to build the UI hierarchy.
	It includes a pointer to a OSCIO controler, but just to give it to the root node.
*/
class FaustFactory
{
	std::stack<SMessageDriven>	fNodes;		///< maintains the current hierarchy level
	SRootNode					fRoot;		///< keep track of the root node
	OSCIO * 					fIO;		///< hack to support audio IO via OSC, actually the field is given to the root node
	GUI *						fGUI;		///< a GUI pointer to support updateAllGuis(), required for bi-directionnal OSC

	private:
		std::string addressFirst (const std::string& address) const;
		std::string addressTail  (const std::string& address) const;

	public:
				 FaustFactory(GUI* ui, OSCIO * io=0); // : fIO(io), fGUI(ui) {}
		virtual ~FaustFactory(); // {}

		template <typename C> void addnode (const char* label, C* zone, C init, C min, C max, bool initZone);
		template <typename C> void addAlias (const std::string& fullpath, C* zone, C imin, C imax, C init, C min, C max, const char* label);
		void addAlias (const char* alias, const char* address, float imin, float imax, float omin, float omax);
		void opengroup (const char* label);
		void closegroup ();

		SRootNode		root() const; //	{ return fRoot; }
};

/**
 * Add a node to the OSC UI tree in the current group at the top of the stack 
 */
template <typename C> void FaustFactory::addnode (const char* label, C* zone, C init, C min, C max, bool initZone) 
{
//	SMessageDriven top = fNodes.size() ? fNodes.top() : fRoot;
	SMessageDriven top;
	if (fNodes.size()) top = fNodes.top();
	if (top) {
		std::string prefix = top->getOSCAddress();
		top->add( FaustNode<C>::create (label, zone, init, min, max, prefix.c_str(), fGUI, initZone));
	}
}

/**
 * Add an alias (actually stored and handled at root node level
 */
template <typename C> void FaustFactory::addAlias (const std::string& fullpath, C* zone, C imin, C imax, C init, C min, C max, const char* label)
{
	std::istringstream 	ss(fullpath);
	std::string 		realpath; 

	ss >> realpath >> imin >> imax;
	SMessageDriven top = fNodes.top();
	if (top ) {
		std::string target = top->getOSCAddress() + "/" + label;
		addAlias (realpath.c_str(), target.c_str(), float(imin), float(imax), float(min), float(max));
	}
}

} // end namespoace

#endif

class GUI;

typedef void (*ErrorCallback)(void*);  

namespace oscfaust
{

class OSCIO;
class OSCSetup;

//--------------------------------------------------------------------------
/*!
	\brief the main Faust OSC Lib API
	
	The OSCControler is essentially a glue between the memory representation (in charge of the FaustFactory),
	and the network services (in charge of OSCSetup).
*/
class OSCControler
{
	int fUDPPort, fUDPOut, fUPDErr;		// the udp ports numbers
	std::string		fDestAddress;		// the osc messages destination address, used at initialization only
										// to collect the address from the command line
	OSCSetup*		fOsc;				// the network manager (handles the udp sockets)
	OSCIO*			fIO;				// hack for OSC IO support (actually only relayed to the factory)
	FaustFactory *	fFactory;			// a factory to build the memory represetnatin

    bool            fInit;
    
	public:
		/*
			base udp port is chosen in an unassigned range from IANA PORT NUMBERS (last updated 2011-01-24)
			see at http://www.iana.org/assignments/port-numbers
			5507-5552  Unassigned
		*/
		enum { kUDPBasePort = 5510};
            
        OSCControler (int argc, char *argv[], GUI* ui, OSCIO* io = 0, ErrorCallback errCallback = NULL, void* arg = NULL, bool init = true);

        virtual ~OSCControler ();
	
		//--------------------------------------------------------------------------
		// addnode, opengroup and closegroup are simply relayed to the factory
		//--------------------------------------------------------------------------
		// Add a node in the current group (top of the group stack)
		template <typename T> void addnode (const char* label, T* zone, T init, T min, T max)
							{ fFactory->addnode (label, zone, init, min, max, fInit); }
		
		//--------------------------------------------------------------------------
		// This method is used for alias messages. The arguments imin and imax allow
		// to map incomming values from the alias input range to the actual range 
		template <typename T> void addAlias (const std::string& fullpath, T* zone, T imin, T imax, T init, T min, T max, const char* label)
							{ fFactory->addAlias (fullpath, zone, imin, imax, init, min, max, label); }

		void opengroup (const char* label)		{ fFactory->opengroup (label); }
		void closegroup ()						{ fFactory->closegroup(); }
	   
		//--------------------------------------------------------------------------
		void run ();				// starts the network services
		void quit ();				// stop the network services
		
		int	getUDPPort() const			{ return fUDPPort; }
		int	getUDPOut()	const			{ return fUDPOut; }
		int	getUDPErr()	const			{ return fUPDErr; }
		const char*	getDestAddress() const { return fDestAddress.c_str(); }
		const char*	getRootName() const;	// probably useless, introduced for UI extension experiments

		static float version();				// the Faust OSC library version number
		static const char* versionstr();	// the Faust OSC library version number as a string
		static bool	gXmit;				// a static variable to control the transmission of values
										// i.e. the use of the interface as a controler
};

}

#endif
#include <vector>

/******************************************************************************
*******************************************************************************

					OSC (Open Sound Control) USER INTERFACE

*******************************************************************************
*******************************************************************************/
/*

Note about the OSC addresses and the Faust UI names:
----------------------------------------------------
There are potential conflicts between the Faust UI objects naming scheme and 
the OSC address space. An OSC symbolic names is an ASCII string consisting of
printable characters other than the following:
	space 
#	number sign
*	asterisk
,	comma
/	forward
?	question mark
[	open bracket
]	close bracket
{	open curly brace
}	close curly brace

a simple solution to address the problem consists in replacing 
space or tabulation with '_' (underscore)
all the other osc excluded characters with '-' (hyphen)

This solution is implemented in the proposed OSC UI;
*/

///using namespace std;

//class oscfaust::OSCIO;

class OSCUI : public GUI 
{
     
	oscfaust::OSCControler*	fCtrl;
	std::vector<const char*> fAlias;
	
	const char* tr(const char* label) const;
	
	// add all accumulated alias
	void addalias(FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, const char* label)
	{
		for (unsigned int i=0; i<fAlias.size(); i++) {
			fCtrl->addAlias(fAlias[i], zone, (FAUSTFLOAT)0, (FAUSTFLOAT)1, init, min, max, label);
		}
		fAlias.clear();
	}
	
 public:

    OSCUI(const char* /*applicationname*/, int argc, char *argv[], oscfaust::OSCIO* io=0, ErrorCallback errCallback = NULL, void* arg = NULL, bool init = true) : GUI() 
    { 
		fCtrl = new oscfaust::OSCControler(argc, argv, this, io, errCallback, arg, init); 
        //		fCtrl->opengroup(applicationname);
	}
    
	virtual ~OSCUI() { delete fCtrl; }
    
    // -- widget's layouts
    
  	virtual void openTabBox(const char* label) 			{ fCtrl->opengroup( tr(label)); }
	virtual void openHorizontalBox(const char* label) 	{ fCtrl->opengroup( tr(label)); }
	virtual void openVerticalBox(const char* label) 	{ fCtrl->opengroup( tr(label)); }
	virtual void closeBox() 							{ fCtrl->closegroup(); }

	
	// -- active widgets
	virtual void addButton(const char* label, FAUSTFLOAT* zone) 		{ const char* l= tr(label); addalias(zone, 0, 0, 1, l); fCtrl->addnode( l, zone, (FAUSTFLOAT)0, (FAUSTFLOAT)0, (FAUSTFLOAT)1); }
	virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) 	{ const char* l= tr(label); addalias(zone, 0, 0, 1, l); fCtrl->addnode( l, zone, (FAUSTFLOAT)0, (FAUSTFLOAT)0, (FAUSTFLOAT)1); }
	virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT /*step*/)
																		{ const char* l= tr(label); addalias(zone, init, min, max, l); fCtrl->addnode( l, zone, init, min, max); }
	virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT /*step*/)
																		{ const char* l= tr(label); addalias(zone, init, min, max, l); fCtrl->addnode( l, zone, init, min, max); }
	virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT /*step*/)
																		{ const char* l= tr(label); addalias(zone, init, min, max, l); fCtrl->addnode( l, zone, init, min, max); }
	
	// -- passive widgets
	
	virtual void addHorizontalBargraph(const char* /*label*/, FAUSTFLOAT* /*zone*/, FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/) {}
	virtual void addVerticalBargraph(const char* /*label*/, FAUSTFLOAT* /*zone*/, FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/) {}
		
	// -- metadata declarations
    
	virtual void declare(FAUSTFLOAT* , const char* key , const char* alias) 
	{ 
		if (strcasecmp(key,"OSC")==0) fAlias.push_back(alias);
	}

	virtual void show() {}

	void run()
    {
        fCtrl->run(); 
    }
	const char* getRootName()		{ return fCtrl->getRootName(); }
    int getUDPPort()                { return fCtrl->getUDPPort(); }
    int	getUDPOut()                 { return fCtrl->getUDPOut(); }
    int	getUDPErr()                 { return fCtrl->getUDPErr(); }
    const char* getDestAddress()    {return fCtrl->getDestAddress();}
};

const char* OSCUI::tr(const char* label) const
{
	static char buffer[1024];
	char * ptr = buffer; int n=1;
	while (*label && (n++ < 1024)) {
		switch (*label) {
			case ' ': case '	':
				*ptr++ = '_';
				break;
			case '#': case '*': case ',': case '/': case '?':
			case '[': case ']': case '{': case '}':
				*ptr++ = '_';
				break;
			default: 
				*ptr++ = *label;
		}
		label++;
	}
	*ptr = 0;
	return buffer;
}

#endif
#endif

#ifdef HTTPCTRL
/*
   Copyright (C) 2012 Grame - Lyon
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted.
*/

#ifndef __httpdUI__
#define __httpdUI__

//#ifdef _WIN32
//#include "HTTPDControler.h"
//#include "UI.h"
//#else
/*

  Faust Project

  Copyright (C) 2012 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __HTTPDControler__
#define __HTTPDControler__

#include <string>
#include <map>

namespace httpdfaust
{

class HTTPDSetup;
class JSONDesc;
class FaustFactory;
class jsonfactory;
class htmlfactory;

//--------------------------------------------------------------------------
/*!
	\brief the main Faust HTTPD Lib API
	
	The HTTPDControler is essentially a glue between the memory representation (in charge of the FaustFactory), 
	and the network services (in charge of HTTPDSetup).
*/
class HTTPDControler
{
	int fTCPPort;				// the tcp port number
	FaustFactory *	fFactory;	// a factory to build the memory representation
	jsonfactory*	fJson;
	htmlfactory*	fHtml;
	HTTPDSetup*		fHttpd;		// the network manager
	std::string		fHTML;		// the corresponding HTML page
	std::map<std::string, std::string>	fCurrentMeta;	// the current meta declarations 

    bool            fInit;
    
	public:
		/*
			base udp port is chosen in an unassigned range from IANA PORT NUMBERS (last updated 2011-01-24)
			see at http://www.iana.org/assignments/port-numbers
			5507-5552  Unassigned
		*/
		enum { kTCPBasePort = 5510};

				 HTTPDControler (int argc, char *argv[], const char* applicationname, bool init = true);
		virtual ~HTTPDControler ();
	
		//--------------------------------------------------------------------------
		// addnode, opengroup and closegroup are simply relayed to the factory
		//--------------------------------------------------------------------------
		template <typename C> void addnode (const char* type, const char* label, C* zone);
		template <typename C> void addnode (const char* type, const char* label, C* zone, C min, C max);
		template <typename C> void addnode (const char* type, const char* label, C* zone, C init, C min, C max, C step);
							  void declare (const char* key, const char* val ) { fCurrentMeta[key] = val; }

		void opengroup (const char* type, const char* label);
		void closegroup ();

		//--------------------------------------------------------------------------
		void run ();				// start the httpd server
		void quit ();				// stop the httpd server
		
		int	getTCPPort()			{ return fTCPPort; }
        std::string getJSONInterface();
        void        setInputs(int numInputs);
        void        setOutputs(int numOutputs);

		static float version();				// the Faust httpd library version number
		static const char* versionstr();	// the Faust httpd library version number as a string
};

}

#endif
//#endif
/******************************************************************************
*******************************************************************************

					HTTPD USER INTERFACE

*******************************************************************************
*******************************************************************************/
/*

Note about URLs and the Faust UI names:
----------------------------------------------------
Characters in a url could be:
1. Reserved: ; / ? : @ & = + $ ,
   These characters delimit URL parts.
2. Unreserved: alphanum - _ . ! ~ * ' ( )
   These characters have no special meaning and can be used as is.
3. Excluded: control characters, space, < > # % ", { } | \ ^ [ ] `

To solve potential conflicts between the Faust UI objects naming scheme and
the URL allowed characters, the reserved and excluded characters are replaced
with '-' (hyphen).
Space or tabulation are replaced with '_' (underscore)
*/

//using namespace std;

class httpdUI : public UI 
{
	httpdfaust::HTTPDControler*	fCtrl;	
	const char* tr(const char* label) const;

 public:
		
	httpdUI(const char* applicationname, int inputs, int outputs, int argc, char *argv[], bool init = true) 
    { 
		fCtrl = new httpdfaust::HTTPDControler(argc, argv, applicationname, init); 
        fCtrl->setInputs(inputs);
        fCtrl->setOutputs(outputs);
	}
	
	virtual ~httpdUI() { delete fCtrl; }
		
    // -- widget's layouts
	virtual void openTabBox(const char* label) 			{ fCtrl->opengroup( "tgroup", tr(label)); }
	virtual void openHorizontalBox(const char* label) 	{ fCtrl->opengroup( "hgroup", tr(label)); }
	virtual void openVerticalBox(const char* label) 	{ fCtrl->opengroup( "vgroup", tr(label)); }
	virtual void closeBox() 							{ fCtrl->closegroup(); }
	
	// -- active widgets
	virtual void addButton(const char* label, FAUSTFLOAT* zone)			{ fCtrl->addnode( "button", tr(label), zone); }
	virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)	{ fCtrl->addnode( "checkbox", tr(label), zone); }
	virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
									{ fCtrl->addnode( "vslider", tr(label), zone, init, min, max, step); }
	virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) 	
									{ fCtrl->addnode( "hslider", tr(label), zone, init, min, max, step); }
	virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) 			
									{ fCtrl->addnode( "nentry", tr(label), zone, init, min, max, step); }
	
	// -- passive widgets	
	virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
									{ fCtrl->addnode( "hbargraph", tr(label), zone, min, max); }
	virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
									{ fCtrl->addnode( "vbargraph", tr(label), zone, min, max); }
	
    virtual void declare (FAUSTFLOAT*, const char* key, const char* val) { fCtrl->declare(key, val); }

	void run()						{ fCtrl->run(); }
	int getTCPPort()                { return fCtrl->getTCPPort(); }
    
    std::string getJSONInterface(){ return fCtrl->getJSONInterface(); }

};
					
const char* httpdUI::tr(const char* label) const
{
	static char buffer[1024];
	char * ptr = buffer; int n=1;
	while (*label && (n++ < 1024)) {
		switch (*label) {
			case ' ': case '	':
				*ptr++ = '_';
				break;
			case ';': case '/': case '?': case ':': case '@': 
			case '&': case '=': case '+': case '$': case ',':
			case '<': case '>': case '#': case '%': case '"': 
			case '{': case '}': case '|': case '\\': case '^': 
			case '[': case ']': case '`':
				*ptr++ = '_';
				break;
			default: 
				*ptr++ = *label;
		}
		label++;
	}
	*ptr = 0;
	return buffer;
}

#endif
#endif

#ifdef OCVCTRL
/************************************************
* 			OpenCV User Interface			  	*
*												*
* This architecture file allows the user to	  	*
*	use the OpenCV library in order to perform	*
*	image processing and use the result 		*
*	to control audio parameters.				*
*												*
* To use this mode, just add the option -ocv in *
* 	the command line. 							*
*												*
************************************************/
#ifndef _OCVUI_H
#define _OCVUI_H


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

// OpenCV includes
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

// Basic includes
#include <iostream>
#include <ctype.h>

// std::thread
#include <pthread.h>

// Main Loop Function Prototype

static void* mainLoop(void*);	// changer nom

//********	OpenCV User Interface CLASS DEFINITION ********//
class OCVUI : public UI
{
    
    public :
    
    ////////////////////////////////////////////
	////									////
	////			STRUCTURES				////
	////									////
	////////////////////////////////////////////
	
	struct object
	{
		int color;
		float centerX;
		float centerY;
		float area;
		int radius;
	};
	
	struct metadata
	{
		FAUSTFLOAT* zone;
		int color;
		std::string param;
		bool used;
	};
	
	////////////////////////////////////////////
	////									////
	////			FUNCTIONS				////
	////									////
	////////////////////////////////////////////
	
	/**********************************************/
	/*******	UI Functions Redefinition	*******/
	/**********************************************/
		// Functions inherited from the UI class
	
    // Constructor
	OCVUI() : objects_storage_(0), parameters_storage_(0), height_(0), width_(0){};
	
	// Destructor
	~OCVUI() 
	{exit_=true;};
	
	
	// -- WIDGETS LAYOUTS
	void openTabBox(const char* label){}
	void openHorizontalBox(const char* label){}
	void openVerticalBox(const char* label){}
	void closeBox(){}

	// -- ACTIVE WIDGETS
	void addButton(const char* label, FAUSTFLOAT* zone){}
	void addCheckButton(const char* label, FAUSTFLOAT* zone){}
	void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
	FAUSTFLOAT max, FAUSTFLOAT step){}
	void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, 
	FAUSTFLOAT max, FAUSTFLOAT step){}
	void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, 
	FAUSTFLOAT max, FAUSTFLOAT step){}
	
	// -- PASSIVE WIDGETS
	void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max){}
	void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max){}


	// -- METADATA DECLARATION
	
	
	//******** Parsing function	********//
		// This function analyses a string and stores
		// the different parameters in a metadata structure.
		
	bool parser(std::string string2parse, metadata *pmeta)
	{
	    int SPACE = 32; // Parameters separator
	    std::vector<std::string> parameters(0);
	    
	    //*** String analysis ***//
	    for (int i = 0 ; i < string2parse.size() ; i++)
	    {
	    	if (string2parse[i]==SPACE)
	    	{
	    	    std::string oneParameter= string2parse.substr(0,i);
	    	    parameters.push_back(oneParameter);
	    	    string2parse.erase(string2parse.begin(), string2parse.begin()+i+1);
	    	    i=0;
	    	}	
	    }
	    std::string lastParameter = string2parse;
	    parameters.push_back(lastParameter);
	    	    
	    //*** Store Parameters in a Metadata Structure ***//
	    
	    // Parameters count must be 2
	    if (parameters.size()==2)
	    {
	    	// Associate every color to a digit
	    		// red		= 1	;	green 	= 3	;	blue 	= 5	;
	    		// yellow	= 2	;	cyan	= 4	;	magenta = 6	.
	    		
	    	if (parameters[0]=="red")
	    	{
	    		pmeta->color = 1;
	    	}
	    	else if (parameters[0]=="yellow")
	    	{
	    		pmeta->color = 2;
	    	}
	    	else if (parameters[0]=="green")
	    	{
	    		pmeta->color = 3;
	    	}
	    	else if (parameters[0]=="cyan")
	    	{
	    		pmeta->color = 4;
	    	}
	    	else if (parameters[0]=="blue")
	    	{
	    		pmeta->color = 5;
	    	}
	    	else if (parameters[0]=="magenta")
	    	{
	    		pmeta->color = 6;
	    	}
	    	pmeta->param = parameters[1];
	    	pmeta->used = false;
	    	return true;
	    }
	    else
	    {
	    	std::cout<<"Wrong count of parameters. Please check if the OpenCV"
	    			 <<"metadata is correctly defined"<<std::endl;
	    	return false;
	    }
	}
	//******** Declare Function ********//
	// This function calls the parsing function if the declared key is "ocv"
		// and creates a new metadata structure.
	void declare(FAUSTFLOAT* zone, const char* key, const char* val) 
	{
		if (key=="ocv")
		{
			metadata newMeta;
			bool string_parsed = false;
						
			if (zone != 0)
			{
				newMeta.zone = zone;
			}
			string_parsed = parser(val, &newMeta);
			
			if (string_parsed)
			{
				parameters_storage_.push_back(newMeta);
			}
		}
	}
	
	/**************************************************/
    /*******	Image Processing Functions		*******/
    /**************************************************/
       
    //*** Contours processing ***//   
    	// This function approximates contours to rectangles,
    	// keeps the bigest one,
    	// and stores it as a new object.
    void contoursProcess(std::vector<std::vector<cv::Point> > contours, int color)
	{
		int tempArea=0;
		cv::Rect myRect;
		for (int j=0 ; j<contours.size() ; j++)								// for each contour
		{
			std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
			std::vector<cv::Rect> boundRect( contours.size() );
		
			if (contours[j].size()>40)										// Do not take care about small
																				// contours
			{
				approxPolyDP( cv::Mat(contours[j]), contours_poly[j], 3, true );// Approximate contours to 
																				// a polygone
				boundRect[j] = cv::boundingRect( cv::Mat(contours_poly[j]) );		// Bound a contour in a 
																				// rectangle
				if ((int)boundRect[j].area()>tempArea)	
				{
					tempArea=(int)boundRect[j].area();
					myRect = boundRect[j];
				}
			}	
		}
		if (tempArea != 0)
		{
			// Create a new object structure to store the object properties
			object newObject;
			newObject.color = color;
			newObject.centerX = myRect.x+myRect.width/2;
			newObject.centerY = myRect.y+myRect.height/2;
			newObject.area = 1.5*(float)tempArea/(width_*height_);
			newObject.radius= (int)MIN(myRect.width/2, myRect.height/2);
				
			// Put the new object in the objects storage.
			objects_storage_.push_back(newObject);
		}
	}
	//*** Morphological Opening (Erosion and Dilatation) ***//
		// Improve a mask shape
		// See OpenCV documentation for more informations :
		// http://docs.opencv.org/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html
	
	void erodeAndDilate(cv::Mat image)
	{
		cv::Mat element;
		element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
		
		// Erase small alone pixels
			// http://docs.opencv.org/modules/imgproc/doc/filtering.html#dilate
		for (int i = 0; i<2 ; i++)
		{
			cv::erode(image, image, element);
		}
		
		// Enlarge blocks of pixels
			// http://docs.opencv.org/modules/imgproc/doc/filtering.html#erode
		for (int i = 0; i<10 ; i++)
		{
			cv::dilate(image, image, element);
		}
	}

	
	//*** Image Threshold ***//
		// This function creates a mask for every defined color
	void thresholdHsv(cv::Mat image)
	{
		// Mask matrices (red, yellow, green, cyan, blue and magenta)
		cv::Mat r_mask, y_mask, g_mask, c_mask, b_mask, m_mask;
	
		// Objects contours
		std::vector<std::vector<cv::Point> > r_contours, y_contours, g_contours, 
											 c_contours, b_contours, m_contours;
		std::vector<cv::Vec4i> hierarchy;
		
		// Get every pixel whose value is between _min and _max
			// and put it into a mask
		cv::inRange(image, red_min, red_max, r_mask);
		cv::inRange(image, yellow_min, yellow_max, y_mask);
		cv::inRange(image, green_min, green_max, g_mask);
		cv::inRange(image, cyan_min, cyan_max, c_mask);
		cv::inRange(image, blue_min, blue_max, b_mask);
		cv::inRange(image, magenta_min, magenta_max, m_mask);
		
		// Improve masks shapes
		erodeAndDilate(r_mask);
		erodeAndDilate(y_mask);
		erodeAndDilate(g_mask);
		erodeAndDilate(c_mask);
		erodeAndDilate(b_mask);
		erodeAndDilate(m_mask);
	
		// Get the shapes contours from masks
		cv::findContours(r_mask, r_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(y_mask, y_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(g_mask, g_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(c_mask, c_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(b_mask, b_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(m_mask, m_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	
		// Process every contour. Note that color is taken in account.
		for (int i=1 ; i<=6 ; i++)
		{
			switch (i)
			{
			case 1:		// RED
				contoursProcess(r_contours, 1);
			
				break;
			
			case 2:		// YELLOW
				contoursProcess(y_contours, 2);
			
				break;
			
			case 3:		// GREEN
				contoursProcess(g_contours, 3);
			
				break;
			
			case 4:		// CYAN
				contoursProcess(c_contours, 4);
			
				break;
		
			case 5:		// BLUE
				contoursProcess(b_contours, 5);
			
				break;
			
			case 6:		// MAGENTA
				contoursProcess(m_contours, 6);
			
				break;
		
			default:	// You'll have to add a new color...
				break;
			}
		}
	
	}

	//*** Object Detection ***//
		// This function calls the previous image processing functions
		// and uses the objects and parameters storages to set the audio parameters
		// depending on the detected objects.
	void detectObjects(cv::Mat hsvImage, cv::Mat originalFrame)
	{	
		height_ = originalFrame.rows;
		width_ = originalFrame.cols;
		
		// Objects detection and storage 
		thresholdHsv(hsvImage);
		
		// Audio parameters setting
		for (int i=0 ; i<objects_storage_.size() ; i++)
		{
			for (int j=0 ; j<parameters_storage_.size() ; j++)
			{
				if(objects_storage_[i].color==parameters_storage_[j].color
					&& !parameters_storage_[j].used)
				{
					if (parameters_storage_[j].param=="color")
					{
						*parameters_storage_[j].zone=(float)objects_storage_[i].color;
					}
					else if (parameters_storage_[j].param=="x")
					{
						*parameters_storage_[j].zone=objects_storage_[i].centerX/width_;
					}
					else if (parameters_storage_[j].param=="y")
					{
						*parameters_storage_[j].zone=objects_storage_[i].centerY/height_;
					}
					else if (parameters_storage_[j].param=="area")
					{
						*parameters_storage_[j].zone=(float)objects_storage_[i].area;
					}
					parameters_storage_[j].used=true;
				}
			}
			
			
			
			// Draw a circle around the object for each object in the objects storage.
				// Circle color depends on the object color, of course !
			cv::Scalar bgr_color;
			switch (objects_storage_[i].color)
			{
				case 1: // RED
					bgr_color = cv::Scalar (0,0,255); // red in BGR (not RGB)
				
					break;
					
				case 2: //YELLOW
					bgr_color = cv::Scalar (0, 255, 255); // yellow in BGR
					
					break;
				
				case 3: // GREEN
					bgr_color = cv::Scalar (0, 255, 0);
				
					break;
				case 4: // CYAN
					bgr_color = cv::Scalar (255, 255, 0);
					
					break;
				
				case 5: // BLUE
					bgr_color = cv::Scalar (255,0,0); // blue in BGR (not RGB)
		
					break;
				
				case 6: // MAGENTA
					bgr_color = cv::Scalar (255, 0, 255);
					
					break;
	
				default: // Add a color !
					break;
			}
			// draw circle around every detected object
			cv::circle(originalFrame, cv::Point(objects_storage_[i].centerX, objects_storage_[i].centerY),
					   objects_storage_[i].radius, bgr_color, 2, 8, 0);
		}
	}
	
	/*******************************************/
	/********	Other Usefull Functions	********/
	/*******************************************/
	
	// Empty Function
		// This function empties both objects and parameters storages
	void empty()
    {
    	while (objects_storage_.size()>0)
    	{
    		objects_storage_.pop_back();
    	}
		
		for(int l=0 ; l<parameters_storage_.size() ; l++)
		{
			parameters_storage_[l].used=false;
		}
	}
	
	bool exit()
	{
		return exit_;
	}
	
	void run()
	{		
		exit_=false;
		int create_thread = 1;
	
		create_thread = pthread_create(&loop_, NULL, mainLoop, (void *) this);
		
		if (create_thread)
		{
			std::cout<<"Could not create thread. Thread Creation failed."<< std::endl;
		}

	}
       	
    ////////////////////////////////////////////
	////									////
	////		  MEMBER VARIABLES  		////
	////									////
	////////////////////////////////////////////
	
    private :
    	
	// HSV min and max values variables
	// #1 : RED
	static cv::Scalar red_min;
	static cv::Scalar red_max;
	
	// #2 : YELLOW
	static cv::Scalar yellow_min;
	static cv::Scalar yellow_max;
	
	// #3 : GREEN
	static cv::Scalar green_min;
	static cv::Scalar green_max;
	
	// #4 : CYAN
	static cv::Scalar cyan_min;
	static cv::Scalar cyan_max;
	
	// #5 : BLUE
	static cv::Scalar blue_min;	
	static cv::Scalar blue_max;
	
	// #6 : MAGENTA
	static cv::Scalar magenta_min;
	static cv::Scalar magenta_max;

	// Objects Storage
		// Where all the objects are stored
	std::vector<object> objects_storage_;
	
	// Parameters Storage
		// Where all the "ocv" metadata parameters are stored
	std::vector<metadata> parameters_storage_;
	
	// Matrix height and width
	int height_, width_;
	
	// Loop thread;
	pthread_t loop_;
	
	// Thread EXIT variable
	bool exit_;
		
};

// HSV min and max values
	// Note that H is between 0 and 180 
	// in openCV
	
// #1 = RED
cv::Scalar OCVUI::red_min = cv::Scalar (0,200,55);
cv::Scalar OCVUI::red_max = cv::Scalar (1,255,255);

// #2 = YELLOW
cv::Scalar OCVUI::yellow_min = cv::Scalar (25, 200, 55);
cv::Scalar OCVUI::yellow_max = cv::Scalar (35, 255, 255);

// #3 = GREEN
cv::Scalar OCVUI::green_min = cv::Scalar (30,155,55);
cv::Scalar OCVUI::green_max = cv::Scalar (40,255,255);

// #4 = CYAN
cv::Scalar OCVUI::cyan_min = cv::Scalar (85,200,55);
cv::Scalar OCVUI::cyan_max = cv::Scalar (95,200,55);

// #5 = BLUE
cv::Scalar OCVUI::blue_min = cv::Scalar (115,155,55);
cv::Scalar OCVUI::blue_max = cv::Scalar (125,255,255);

// #6 = MAGENTA
cv::Scalar OCVUI::magenta_min = cv::Scalar (145,200,55);
cv::Scalar OCVUI::magenta_max = cv::Scalar (155,255,255);


// Main Loop Function Implementation
	// This function is a loop that gets every frame from a camera
	// and calls the image processing functions.
	// This is the main function.
void* mainLoop(void* ocv_object)
{
	// The camera index allows to select the camera.
		// 0 stands for the default camera.
	int camIndex=0;
	//std::cout<<"camera index ?"<<std::endl;
	//std::cin>>camIndex;

	cv::Mat frame, hsv;
	OCVUI* ocv = (OCVUI*) ocv_object;
	cv::VideoCapture cap(camIndex);
	std::cout<<"Video Capture from camera n°"<<camIndex<<std::endl;
	
	if(!cap.isOpened())  // check if we succeeded to read frames
							// from camera
	{
		std::cout<<"Could not open camera n°"<<camIndex<<" !"<<std::endl;
		
	}
    	  
	cap.set(CV_CAP_PROP_FPS, 60); 	// Set frames rate
		
	cv::namedWindow( "Tracking", 1 );	// Create a window

	while(!ocv->exit())
   	{

   	    cap >> frame;							// Get frame from camera
   	    cv::cvtColor(frame, hsv, CV_BGR2HSV);		// Convert frame to HSV format 
   	    											// in order to use "inRange"
   	            														
		ocv->detectObjects(hsv, frame);				// Objects Detection function
	
   		/*** Show image ***/
   		cv::imshow("Tracking", frame);

   		ocv->empty();								// Empty the objects and parameters storages
     	
   		/*** break ***/
  		//if(cv::waitKey(27) >= 0) break;

   	}

}

#endif
#endif

/**************************BEGIN USER SECTION **************************/

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/



#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  

typedef long double quad;

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	int 	iConst0;
	float 	fConst1;
	float 	fConst2;
	FAUSTFLOAT 	fslider0;
	float 	fRec2[2];
	float 	fVec0[2];
	float 	fConst3;
	float 	fRec1[2];
	int 	IOTA;
	float 	fVec1[4096];
	float 	fConst4;
	float 	fConst5;
	int 	iRec12[2];
	int 	iRec25[2];
	float 	fRec27[2];
	int 	iRec26[2];
	float 	fRec11[2];
	float 	fConst6;
	float 	fRec10[3];
	float 	fVec2[2];
	FAUSTFLOAT 	fslider1;
	float 	fRec28[2];
	float 	fConst7;
	float 	fRec29[2];
	float 	fVec3[2];
	float 	fRec8[2];
	float 	fRec9[2];
	float 	fConst8;
	float 	fRec7[2];
	float 	fConst9;
	FAUSTFLOAT 	fslider2;
	float 	fRec32[2];
	float 	fVec4[2];
	float 	fRec30[2];
	float 	fRec31[2];
	float 	fRec39[2];
	int 	iRec38[2];
	float 	fRec37[2];
	float 	fRec36[3];
	float 	fVec5[2];
	FAUSTFLOAT 	fslider3;
	float 	fRec40[2];
	float 	fRec41[2];
	float 	fVec6[2];
	float 	fRec34[2];
	float 	fRec35[2];
	float 	fRec33[2];
	FAUSTFLOAT 	fslider4;
	float 	fRec44[2];
	float 	fVec7[2];
	float 	fRec42[2];
	float 	fRec43[2];
	float 	fRec3[2];
	float 	fRec4[2];
	float 	fRec5[2];
	float 	fRec6[2];
	float 	fRec55[2];
	int 	iRec54[2];
	float 	fRec53[2];
	float 	fRec52[3];
	float 	fVec8[2];
	float 	fRec56[2];
	float 	fVec9[2];
	float 	fRec50[2];
	float 	fRec51[2];
	float 	fRec49[2];
	float 	fVec10[2];
	float 	fRec57[2];
	float 	fRec58[2];
	float 	fRec65[2];
	int 	iRec64[2];
	float 	fRec63[2];
	float 	fRec62[3];
	float 	fVec11[2];
	float 	fRec66[2];
	float 	fVec12[2];
	float 	fRec60[2];
	float 	fRec61[2];
	float 	fRec59[2];
	float 	fVec13[2];
	float 	fRec67[2];
	float 	fRec68[2];
	float 	fRec45[2];
	float 	fRec46[2];
	float 	fRec47[2];
	float 	fRec48[2];
	float 	fRec79[2];
	int 	iRec78[2];
	float 	fRec77[2];
	float 	fRec76[3];
	float 	fVec14[2];
	float 	fRec80[2];
	float 	fVec15[2];
	float 	fRec74[2];
	float 	fRec75[2];
	float 	fRec73[2];
	float 	fVec16[2];
	float 	fRec81[2];
	float 	fRec82[2];
	float 	fRec89[2];
	int 	iRec88[2];
	float 	fRec87[2];
	float 	fRec86[3];
	float 	fVec17[2];
	float 	fRec90[2];
	float 	fVec18[2];
	float 	fRec84[2];
	float 	fRec85[2];
	float 	fRec83[2];
	float 	fVec19[2];
	float 	fRec91[2];
	float 	fRec92[2];
	float 	fRec69[2];
	float 	fRec70[2];
	float 	fRec71[2];
	float 	fRec72[2];
	float 	fRec0[2];
	float 	fVec20[2];
	float 	fRec94[2];
	float 	fVec21[4096];
	float 	fRec105[2];
	int 	iRec104[2];
	float 	fRec103[2];
	float 	fRec102[3];
	float 	fVec22[2];
	float 	fRec106[2];
	float 	fVec23[2];
	float 	fRec100[2];
	float 	fRec101[2];
	float 	fRec99[2];
	float 	fVec24[2];
	float 	fRec107[2];
	float 	fRec108[2];
	float 	fRec115[2];
	int 	iRec114[2];
	float 	fRec113[2];
	float 	fRec112[3];
	float 	fVec25[2];
	float 	fRec116[2];
	float 	fVec26[2];
	float 	fRec110[2];
	float 	fRec111[2];
	float 	fRec109[2];
	float 	fVec27[2];
	float 	fRec117[2];
	float 	fRec118[2];
	float 	fRec95[2];
	float 	fRec96[2];
	float 	fRec97[2];
	float 	fRec98[2];
	float 	fRec129[2];
	int 	iRec128[2];
	float 	fRec127[2];
	float 	fRec126[3];
	float 	fVec28[2];
	float 	fRec130[2];
	float 	fVec29[2];
	float 	fRec124[2];
	float 	fRec125[2];
	float 	fRec123[2];
	float 	fVec30[2];
	float 	fRec131[2];
	float 	fRec132[2];
	float 	fRec139[2];
	int 	iRec138[2];
	float 	fRec137[2];
	float 	fRec136[3];
	float 	fVec31[2];
	float 	fRec140[2];
	float 	fVec32[2];
	float 	fRec134[2];
	float 	fRec135[2];
	float 	fRec133[2];
	float 	fVec33[2];
	float 	fRec141[2];
	float 	fRec142[2];
	float 	fRec119[2];
	float 	fRec120[2];
	float 	fRec121[2];
	float 	fRec122[2];
	float 	fRec153[2];
	int 	iRec152[2];
	float 	fRec151[2];
	float 	fRec150[3];
	float 	fVec34[2];
	float 	fRec154[2];
	float 	fVec35[2];
	float 	fRec148[2];
	float 	fRec149[2];
	float 	fRec147[2];
	float 	fVec36[2];
	float 	fRec155[2];
	float 	fRec156[2];
	float 	fRec163[2];
	int 	iRec162[2];
	float 	fRec161[2];
	float 	fRec160[3];
	float 	fVec37[2];
	float 	fRec164[2];
	float 	fVec38[2];
	float 	fRec158[2];
	float 	fRec159[2];
	float 	fRec157[2];
	float 	fVec39[2];
	float 	fRec165[2];
	float 	fRec166[2];
	float 	fRec143[2];
	float 	fRec144[2];
	float 	fRec145[2];
	float 	fRec146[2];
	float 	fRec93[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "Chorus");
		m->declare("version", "0.1");
		m->declare("author", "Bart Brouns");
		m->declare("license", "GNU 3.0");
		m->declare("copyright", "(c) Bart Brouns 2014");
		m->declare("coauthors", "ported from a pd patch by Scott Nordlund, 2011");
		m->declare("oscillator.lib/name", "Faust Oscillator Library");
		m->declare("oscillator.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("oscillator.lib/copyright", "Julius O. Smith III");
		m->declare("oscillator.lib/version", "1.11");
		m->declare("oscillator.lib/license", "STK-4.3");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("maxmsp.lib/name", "MaxMSP compatibility Library");
		m->declare("maxmsp.lib/author", "GRAME");
		m->declare("maxmsp.lib/copyright", "GRAME");
		m->declare("maxmsp.lib/version", "1.1");
		m->declare("maxmsp.lib/license", "LGPL");
		m->declare("effect.lib/name", "Faust Audio Effect Library");
		m->declare("effect.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("effect.lib/copyright", "Julius O. Smith III");
		m->declare("effect.lib/version", "1.33");
		m->declare("effect.lib/license", "STK-4.3");
		m->declare("effect.lib/exciter_name", "Harmonic Exciter");
		m->declare("effect.lib/exciter_author", "Priyanka Shekar (pshekar@ccrma.stanford.edu)");
		m->declare("effect.lib/exciter_copyright", "Copyright (c) 2013 Priyanka Shekar");
		m->declare("effect.lib/exciter_version", "1.0");
		m->declare("effect.lib/exciter_license", "MIT License (MIT)");
	}

	virtual int getNumInputs() 	{ return 2; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (628.3185307179587f / float(iConst0));
		fConst2 = (1 - fConst1);
		fslider0 = 0.0f;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		for (int i=0; i<2; i++) fVec0[i] = 0;
		fConst3 = (1.0f / (1 + fConst1));
		for (int i=0; i<2; i++) fRec1[i] = 0;
		IOTA = 0;
		for (int i=0; i<4096; i++) fVec1[i] = 0;
		fConst4 = (4.444f * iConst0);
		fConst5 = (0.667f * iConst0);
		for (int i=0; i<2; i++) iRec12[i] = 0;
		for (int i=0; i<2; i++) iRec25[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<2; i++) iRec26[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		fConst6 = (0.001f * iConst0);
		for (int i=0; i<3; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) fVec2[i] = 0;
		fslider1 = 1.0f;
		for (int i=0; i<2; i++) fRec28[i] = 0;
		fConst7 = (1.0f / float(iConst0));
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<2; i++) fVec3[i] = 0;
		for (int i=0; i<2; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		fConst8 = float(iConst0);
		for (int i=0; i<2; i++) fRec7[i] = 0;
		fConst9 = (1.0f / fConst8);
		fslider2 = 4.0f;
		for (int i=0; i<2; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) fVec4[i] = 0;
		for (int i=0; i<2; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<2; i++) iRec38[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<3; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) fVec5[i] = 0;
		fslider3 = 0.57f;
		for (int i=0; i<2; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<2; i++) fVec6[i] = 0;
		for (int i=0; i<2; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		fslider4 = 4.0f;
		for (int i=0; i<2; i++) fRec44[i] = 0;
		for (int i=0; i<2; i++) fVec7[i] = 0;
		for (int i=0; i<2; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) fRec55[i] = 0;
		for (int i=0; i<2; i++) iRec54[i] = 0;
		for (int i=0; i<2; i++) fRec53[i] = 0;
		for (int i=0; i<3; i++) fRec52[i] = 0;
		for (int i=0; i<2; i++) fVec8[i] = 0;
		for (int i=0; i<2; i++) fRec56[i] = 0;
		for (int i=0; i<2; i++) fVec9[i] = 0;
		for (int i=0; i<2; i++) fRec50[i] = 0;
		for (int i=0; i<2; i++) fRec51[i] = 0;
		for (int i=0; i<2; i++) fRec49[i] = 0;
		for (int i=0; i<2; i++) fVec10[i] = 0;
		for (int i=0; i<2; i++) fRec57[i] = 0;
		for (int i=0; i<2; i++) fRec58[i] = 0;
		for (int i=0; i<2; i++) fRec65[i] = 0;
		for (int i=0; i<2; i++) iRec64[i] = 0;
		for (int i=0; i<2; i++) fRec63[i] = 0;
		for (int i=0; i<3; i++) fRec62[i] = 0;
		for (int i=0; i<2; i++) fVec11[i] = 0;
		for (int i=0; i<2; i++) fRec66[i] = 0;
		for (int i=0; i<2; i++) fVec12[i] = 0;
		for (int i=0; i<2; i++) fRec60[i] = 0;
		for (int i=0; i<2; i++) fRec61[i] = 0;
		for (int i=0; i<2; i++) fRec59[i] = 0;
		for (int i=0; i<2; i++) fVec13[i] = 0;
		for (int i=0; i<2; i++) fRec67[i] = 0;
		for (int i=0; i<2; i++) fRec68[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<2; i++) fRec46[i] = 0;
		for (int i=0; i<2; i++) fRec47[i] = 0;
		for (int i=0; i<2; i++) fRec48[i] = 0;
		for (int i=0; i<2; i++) fRec79[i] = 0;
		for (int i=0; i<2; i++) iRec78[i] = 0;
		for (int i=0; i<2; i++) fRec77[i] = 0;
		for (int i=0; i<3; i++) fRec76[i] = 0;
		for (int i=0; i<2; i++) fVec14[i] = 0;
		for (int i=0; i<2; i++) fRec80[i] = 0;
		for (int i=0; i<2; i++) fVec15[i] = 0;
		for (int i=0; i<2; i++) fRec74[i] = 0;
		for (int i=0; i<2; i++) fRec75[i] = 0;
		for (int i=0; i<2; i++) fRec73[i] = 0;
		for (int i=0; i<2; i++) fVec16[i] = 0;
		for (int i=0; i<2; i++) fRec81[i] = 0;
		for (int i=0; i<2; i++) fRec82[i] = 0;
		for (int i=0; i<2; i++) fRec89[i] = 0;
		for (int i=0; i<2; i++) iRec88[i] = 0;
		for (int i=0; i<2; i++) fRec87[i] = 0;
		for (int i=0; i<3; i++) fRec86[i] = 0;
		for (int i=0; i<2; i++) fVec17[i] = 0;
		for (int i=0; i<2; i++) fRec90[i] = 0;
		for (int i=0; i<2; i++) fVec18[i] = 0;
		for (int i=0; i<2; i++) fRec84[i] = 0;
		for (int i=0; i<2; i++) fRec85[i] = 0;
		for (int i=0; i<2; i++) fRec83[i] = 0;
		for (int i=0; i<2; i++) fVec19[i] = 0;
		for (int i=0; i<2; i++) fRec91[i] = 0;
		for (int i=0; i<2; i++) fRec92[i] = 0;
		for (int i=0; i<2; i++) fRec69[i] = 0;
		for (int i=0; i<2; i++) fRec70[i] = 0;
		for (int i=0; i<2; i++) fRec71[i] = 0;
		for (int i=0; i<2; i++) fRec72[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) fVec20[i] = 0;
		for (int i=0; i<2; i++) fRec94[i] = 0;
		for (int i=0; i<4096; i++) fVec21[i] = 0;
		for (int i=0; i<2; i++) fRec105[i] = 0;
		for (int i=0; i<2; i++) iRec104[i] = 0;
		for (int i=0; i<2; i++) fRec103[i] = 0;
		for (int i=0; i<3; i++) fRec102[i] = 0;
		for (int i=0; i<2; i++) fVec22[i] = 0;
		for (int i=0; i<2; i++) fRec106[i] = 0;
		for (int i=0; i<2; i++) fVec23[i] = 0;
		for (int i=0; i<2; i++) fRec100[i] = 0;
		for (int i=0; i<2; i++) fRec101[i] = 0;
		for (int i=0; i<2; i++) fRec99[i] = 0;
		for (int i=0; i<2; i++) fVec24[i] = 0;
		for (int i=0; i<2; i++) fRec107[i] = 0;
		for (int i=0; i<2; i++) fRec108[i] = 0;
		for (int i=0; i<2; i++) fRec115[i] = 0;
		for (int i=0; i<2; i++) iRec114[i] = 0;
		for (int i=0; i<2; i++) fRec113[i] = 0;
		for (int i=0; i<3; i++) fRec112[i] = 0;
		for (int i=0; i<2; i++) fVec25[i] = 0;
		for (int i=0; i<2; i++) fRec116[i] = 0;
		for (int i=0; i<2; i++) fVec26[i] = 0;
		for (int i=0; i<2; i++) fRec110[i] = 0;
		for (int i=0; i<2; i++) fRec111[i] = 0;
		for (int i=0; i<2; i++) fRec109[i] = 0;
		for (int i=0; i<2; i++) fVec27[i] = 0;
		for (int i=0; i<2; i++) fRec117[i] = 0;
		for (int i=0; i<2; i++) fRec118[i] = 0;
		for (int i=0; i<2; i++) fRec95[i] = 0;
		for (int i=0; i<2; i++) fRec96[i] = 0;
		for (int i=0; i<2; i++) fRec97[i] = 0;
		for (int i=0; i<2; i++) fRec98[i] = 0;
		for (int i=0; i<2; i++) fRec129[i] = 0;
		for (int i=0; i<2; i++) iRec128[i] = 0;
		for (int i=0; i<2; i++) fRec127[i] = 0;
		for (int i=0; i<3; i++) fRec126[i] = 0;
		for (int i=0; i<2; i++) fVec28[i] = 0;
		for (int i=0; i<2; i++) fRec130[i] = 0;
		for (int i=0; i<2; i++) fVec29[i] = 0;
		for (int i=0; i<2; i++) fRec124[i] = 0;
		for (int i=0; i<2; i++) fRec125[i] = 0;
		for (int i=0; i<2; i++) fRec123[i] = 0;
		for (int i=0; i<2; i++) fVec30[i] = 0;
		for (int i=0; i<2; i++) fRec131[i] = 0;
		for (int i=0; i<2; i++) fRec132[i] = 0;
		for (int i=0; i<2; i++) fRec139[i] = 0;
		for (int i=0; i<2; i++) iRec138[i] = 0;
		for (int i=0; i<2; i++) fRec137[i] = 0;
		for (int i=0; i<3; i++) fRec136[i] = 0;
		for (int i=0; i<2; i++) fVec31[i] = 0;
		for (int i=0; i<2; i++) fRec140[i] = 0;
		for (int i=0; i<2; i++) fVec32[i] = 0;
		for (int i=0; i<2; i++) fRec134[i] = 0;
		for (int i=0; i<2; i++) fRec135[i] = 0;
		for (int i=0; i<2; i++) fRec133[i] = 0;
		for (int i=0; i<2; i++) fVec33[i] = 0;
		for (int i=0; i<2; i++) fRec141[i] = 0;
		for (int i=0; i<2; i++) fRec142[i] = 0;
		for (int i=0; i<2; i++) fRec119[i] = 0;
		for (int i=0; i<2; i++) fRec120[i] = 0;
		for (int i=0; i<2; i++) fRec121[i] = 0;
		for (int i=0; i<2; i++) fRec122[i] = 0;
		for (int i=0; i<2; i++) fRec153[i] = 0;
		for (int i=0; i<2; i++) iRec152[i] = 0;
		for (int i=0; i<2; i++) fRec151[i] = 0;
		for (int i=0; i<3; i++) fRec150[i] = 0;
		for (int i=0; i<2; i++) fVec34[i] = 0;
		for (int i=0; i<2; i++) fRec154[i] = 0;
		for (int i=0; i<2; i++) fVec35[i] = 0;
		for (int i=0; i<2; i++) fRec148[i] = 0;
		for (int i=0; i<2; i++) fRec149[i] = 0;
		for (int i=0; i<2; i++) fRec147[i] = 0;
		for (int i=0; i<2; i++) fVec36[i] = 0;
		for (int i=0; i<2; i++) fRec155[i] = 0;
		for (int i=0; i<2; i++) fRec156[i] = 0;
		for (int i=0; i<2; i++) fRec163[i] = 0;
		for (int i=0; i<2; i++) iRec162[i] = 0;
		for (int i=0; i<2; i++) fRec161[i] = 0;
		for (int i=0; i<3; i++) fRec160[i] = 0;
		for (int i=0; i<2; i++) fVec37[i] = 0;
		for (int i=0; i<2; i++) fRec164[i] = 0;
		for (int i=0; i<2; i++) fVec38[i] = 0;
		for (int i=0; i<2; i++) fRec158[i] = 0;
		for (int i=0; i<2; i++) fRec159[i] = 0;
		for (int i=0; i<2; i++) fRec157[i] = 0;
		for (int i=0; i<2; i++) fVec39[i] = 0;
		for (int i=0; i<2; i++) fRec165[i] = 0;
		for (int i=0; i<2; i++) fRec166[i] = 0;
		for (int i=0; i<2; i++) fRec143[i] = 0;
		for (int i=0; i<2; i++) fRec144[i] = 0;
		for (int i=0; i<2; i++) fRec145[i] = 0;
		for (int i=0; i<2; i++) fRec146[i] = 0;
		for (int i=0; i<2; i++) fRec93[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->declare(0, "0", "");
		interface->openHorizontalBox("chorus");
		interface->declare(&fslider3, "0", "");
		interface->addVerticalSlider("slow freq", &fslider3, 0.57f, 0.0f, 5.0f, 0.01f);
		interface->declare(&fslider4, "1", "");
		interface->addVerticalSlider("slow depth", &fslider4, 4.0f, 0.0f, 5.0f, 0.01f);
		interface->declare(&fslider1, "2", "");
		interface->addVerticalSlider("fast freq", &fslider1, 1.0f, 0.0f, 1e+01f, 0.01f);
		interface->declare(&fslider2, "3", "");
		interface->addVerticalSlider("fast depth", &fslider2, 4.0f, 0.0f, 1e+01f, 0.01f);
		interface->declare(&fslider0, "4", "");
		interface->addVerticalSlider("feedback", &fslider0, 0.0f, -1.0f, 1.0f, 0.01f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = (0.0008000000000000008f * float(fslider0));
		float 	fSlow1 = (0.0010000000000000009f * float(fslider1));
		float 	fSlow2 = (0.0010000000000000009f * faustpower<2>(float(fslider2)));
		float 	fSlow3 = (0.0010000000000000009f * float(fslider3));
		float 	fSlow4 = (0.0010000000000000009f * faustpower<2>(float(fslider4)));
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			fRec2[0] = ((0.999f * fRec2[1]) + fSlow0);
			float fTemp0 = (fRec2[0] * fRec0[1]);
			fVec0[0] = fTemp0;
			fRec1[0] = (fConst3 * ((fVec0[0] - fVec0[1]) + (fConst2 * fRec1[1])));
			float fTemp1 = ((float)input0[i] + (0.25f * tanhf((2 * fRec1[0]))));
			fVec1[IOTA&4095] = fTemp1;
			int iTemp2 = (1103515245 * (12345 + iRec12[1]));
			int iTemp3 = (1103515245 * (12345 + iTemp2));
			int iTemp4 = (1103515245 * (12345 + iTemp3));
			int iTemp5 = (1103515245 * (12345 + iTemp4));
			int iTemp6 = (1103515245 * (12345 + iTemp5));
			int iTemp7 = (1103515245 * (12345 + iTemp6));
			int iTemp8 = (1103515245 * (12345 + iTemp7));
			int iTemp9 = (1103515245 * (12345 + iTemp8));
			int iTemp10 = (1103515245 * (12345 + iTemp9));
			int iTemp11 = (1103515245 * (12345 + iTemp10));
			int iTemp12 = (1103515245 * (12345 + iTemp11));
			int iTemp13 = (1103515245 * (12345 + iTemp12));
			iRec12[0] = (1103515245 * (12345 + iTemp13));
			int 	iRec13 = iTemp13;
			int 	iRec14 = iTemp12;
			int 	iRec15 = iTemp11;
			int 	iRec16 = iTemp10;
			int 	iRec17 = iTemp9;
			int 	iRec18 = iTemp8;
			int 	iRec19 = iTemp7;
			int 	iRec20 = iTemp6;
			int 	iRec21 = iTemp5;
			int 	iRec22 = iTemp4;
			int 	iRec23 = iTemp3;
			int 	iRec24 = iTemp2;
			float fTemp14 = (0.5f + (2.3283064376228985e-10f * iRec18));
			iRec25[0] = (1 + iRec25[1]);
			int iTemp15 = ((iRec25[0] - 1) < 1);
			float fTemp16 = min(fConst4, max(fConst5, fRec10[1]));
			fRec27[0] = ((int((((fabsf((fRec10[1] - fRec10[2])) > 0) * iRec26[1]) > 0)))?0:min(fTemp16, (1 + fRec27[1])));
			iRec26[0] = (fRec27[0] == fTemp16);
			int iTemp17 = ((abs((iRec26[0] - iRec26[1])) > 0) | iTemp15);
			fRec11[0] = ((fRec11[1] * (1 - iTemp17)) + (iTemp17 * fTemp14));
			fRec10[0] = (fConst6 * (0 - (1000 * logf(fRec11[0]))));
			float fTemp18 = min(fConst4, max(fConst5, fRec10[0]));
			fVec2[0] = fTemp18;
			fRec28[0] = (fSlow1 + (0.999f * fRec28[1]));
			float fTemp19 = logf(fRec28[0]);
			float fTemp20 = (logf((1.75f * fRec28[0])) - fTemp19);
			int iTemp21 = (fabsf((fConst7 * ((1e+03f * fVec2[0]) - (1e+03f * fVec2[1])))) > 0);
			float fTemp22 = (0.5f + (2.3283064376228985e-10f * iRec17));
			fRec29[0] = ((fRec29[1] * (1 - iTemp21)) + (fTemp22 * iTemp21));
			float fTemp23 = expf((fTemp19 + (fRec29[0] * fTemp20)));
			fVec3[0] = fTemp23;
			float fTemp24 = ((int((fVec3[0] != fVec3[1])))?fVec2[0]:(fRec8[1] - 1));
			fRec8[0] = fTemp24;
			fRec9[0] = ((int((fTemp24 <= 0)))?fVec3[0]:(fRec9[1] + ((fVec3[0] - fRec9[1]) / fTemp24)));
			fRec7[0] = fmodf((1.0f + fRec7[1]),(fConst8 / fRec9[0]));
			fRec32[0] = (fSlow2 + (0.999f * fRec32[1]));
			float fTemp25 = powf(2,(0.0008333333333333334f * fRec32[0]));
			float fTemp26 = (1 + fTemp25);
			float fTemp27 = (fTemp25 - 1);
			float fTemp28 = (fTemp27 / (fTemp26 * fVec3[0]));
			fVec4[0] = fTemp28;
			float fTemp29 = (318.309886f * fVec4[0]);
			float fTemp30 = ((int((fTemp29 != (318.309886f * fVec4[1]))))?fVec2[0]:(fRec30[1] - 1));
			fRec30[0] = fTemp30;
			fRec31[0] = ((int((fTemp30 <= 0)))?fTemp29:(fRec31[1] + ((fTemp29 - fRec31[1]) / fTemp30)));
			float fTemp31 = min(fConst4, max(fConst5, fRec36[1]));
			fRec39[0] = ((int((((fabsf((fRec36[1] - fRec36[2])) > 0) * iRec38[1]) > 0)))?0:min(fTemp31, (1 + fRec39[1])));
			iRec38[0] = (fRec39[0] == fTemp31);
			int iTemp32 = ((abs((iRec38[0] - iRec38[1])) > 0) | iTemp15);
			fRec37[0] = ((fRec37[1] * (1 - iTemp32)) + (iTemp32 * fTemp22));
			fRec36[0] = (fConst6 * (0 - (1000 * logf(fRec37[0]))));
			float fTemp33 = min(fConst4, max(fConst5, fRec36[0]));
			fVec5[0] = fTemp33;
			fRec40[0] = (fSlow3 + (0.999f * fRec40[1]));
			float fTemp34 = logf(fRec40[0]);
			float fTemp35 = (logf((1.25f * fRec40[0])) - fTemp34);
			int iTemp36 = (fabsf((fConst7 * ((1e+03f * fVec5[0]) - (1e+03f * fVec5[1])))) > 0);
			float fTemp37 = (0.5f + (2.3283064376228985e-10f * iRec16));
			fRec41[0] = ((fRec41[1] * (1 - iTemp36)) + (fTemp37 * iTemp36));
			float fTemp38 = expf((fTemp34 + (fRec41[0] * fTemp35)));
			fVec6[0] = fTemp38;
			float fTemp39 = ((int((fVec6[0] != fVec6[1])))?fVec5[0]:(fRec34[1] - 1));
			fRec34[0] = fTemp39;
			fRec35[0] = ((int((fTemp39 <= 0)))?fVec6[0]:(fRec35[1] + ((fVec6[0] - fRec35[1]) / fTemp39)));
			fRec33[0] = fmodf((1.0f + fRec33[1]),(fConst8 / fRec35[0]));
			fRec44[0] = (fSlow4 + (0.999f * fRec44[1]));
			float fTemp40 = powf(2,(0.0008333333333333334f * fRec44[0]));
			float fTemp41 = (1 + fTemp40);
			float fTemp42 = (fTemp40 - 1);
			float fTemp43 = (fTemp42 / (fTemp41 * expf((fTemp19 + (fRec41[0] * fTemp20)))));
			fVec7[0] = fTemp43;
			float fTemp44 = (2000 * fVec7[0]);
			float fTemp45 = ((int((fTemp44 != (2000 * fVec7[1]))))?fVec5[0]:(fRec42[1] - 1));
			fRec42[0] = fTemp45;
			fRec43[0] = ((int((fTemp45 <= 0)))?fTemp44:(fRec43[1] + ((fTemp44 - fRec43[1]) / fTemp45)));
			float fTemp46 = (fConst6 * (10 + ((fRec43[0] * fabsf(((fConst9 * (fRec35[0] * fRec33[0])) - 0.5f))) + (fRec31[0] * (1 + sinf((fConst9 * (fRec9[0] * fRec7[0]))))))));
			float fTemp47 = ((int((fRec3[1] != 0.0f)))?((int(((fRec4[1] > 0.0f) & (fRec4[1] < 1.0f))))?fRec3[1]:0):((int(((fRec4[1] == 0.0f) & (fTemp46 != fRec5[1]))))?0.0009765625f:((int(((fRec4[1] == 1.0f) & (fTemp46 != fRec6[1]))))?-0.0009765625f:0)));
			fRec3[0] = fTemp47;
			fRec4[0] = max(0.0f, min(1.0f, (fRec4[1] + fTemp47)));
			fRec5[0] = ((int(((fRec4[1] >= 1.0f) & (fRec6[1] != fTemp46))))?fTemp46:fRec5[1]);
			fRec6[0] = ((int(((fRec4[1] <= 0.0f) & (fRec5[1] != fTemp46))))?fTemp46:fRec6[1]);
			float fTemp48 = min(fConst4, max(fConst5, fRec52[1]));
			fRec55[0] = ((int((((fabsf((fRec52[1] - fRec52[2])) > 0) * iRec54[1]) > 0)))?0:min(fTemp48, (1 + fRec55[1])));
			iRec54[0] = (fRec55[0] == fTemp48);
			int iTemp49 = ((abs((iRec54[0] - iRec54[1])) > 0) | iTemp15);
			fRec53[0] = ((fRec53[1] * (1 - iTemp49)) + (iTemp49 * fTemp37));
			fRec52[0] = (fConst6 * (0 - (1000 * logf(fRec53[0]))));
			float fTemp50 = min(fConst4, max(fConst5, fRec52[0]));
			fVec8[0] = fTemp50;
			int iTemp51 = (fabsf((fConst7 * ((1e+03f * fVec8[0]) - (1e+03f * fVec8[1])))) > 0);
			float fTemp52 = (0.5f + (2.3283064376228985e-10f * iRec15));
			fRec56[0] = ((fRec56[1] * (1 - iTemp51)) + (fTemp52 * iTemp51));
			float fTemp53 = expf((fTemp19 + (fRec56[0] * fTemp20)));
			fVec9[0] = fTemp53;
			float fTemp54 = ((int((fVec9[0] != fVec9[1])))?fVec8[0]:(fRec50[1] - 1));
			fRec50[0] = fTemp54;
			fRec51[0] = ((int((fTemp54 <= 0)))?fVec9[0]:(fRec51[1] + ((fVec9[0] - fRec51[1]) / fTemp54)));
			fRec49[0] = fmodf((1.0f + fRec49[1]),(fConst8 / fRec51[0]));
			float fTemp55 = (fTemp27 / (fTemp26 * fVec9[0]));
			fVec10[0] = fTemp55;
			float fTemp56 = (318.309886f * fVec10[0]);
			float fTemp57 = ((int((fTemp56 != (318.309886f * fVec10[1]))))?fVec8[0]:(fRec57[1] - 1));
			fRec57[0] = fTemp57;
			fRec58[0] = ((int((fTemp57 <= 0)))?fTemp56:(fRec58[1] + ((fTemp56 - fRec58[1]) / fTemp57)));
			float fTemp58 = min(fConst4, max(fConst5, fRec62[1]));
			fRec65[0] = ((int((((fabsf((fRec62[1] - fRec62[2])) > 0) * iRec64[1]) > 0)))?0:min(fTemp58, (1 + fRec65[1])));
			iRec64[0] = (fRec65[0] == fTemp58);
			int iTemp59 = ((abs((iRec64[0] - iRec64[1])) > 0) | iTemp15);
			fRec63[0] = ((fRec63[1] * (1 - iTemp59)) + (iTemp59 * fTemp52));
			fRec62[0] = (fConst6 * (0 - (1000 * logf(fRec63[0]))));
			float fTemp60 = min(fConst4, max(fConst5, fRec62[0]));
			fVec11[0] = fTemp60;
			int iTemp61 = (fabsf((fConst7 * ((1e+03f * fVec11[0]) - (1e+03f * fVec11[1])))) > 0);
			float fTemp62 = (0.5f + (2.3283064376228985e-10f * iRec14));
			fRec66[0] = ((fRec66[1] * (1 - iTemp61)) + (fTemp62 * iTemp61));
			float fTemp63 = expf((fTemp34 + (fRec66[0] * fTemp35)));
			fVec12[0] = fTemp63;
			float fTemp64 = ((int((fVec12[0] != fVec12[1])))?fVec11[0]:(fRec60[1] - 1));
			fRec60[0] = fTemp64;
			fRec61[0] = ((int((fTemp64 <= 0)))?fVec12[0]:(fRec61[1] + ((fVec12[0] - fRec61[1]) / fTemp64)));
			fRec59[0] = fmodf((1.0f + fRec59[1]),(fConst8 / fRec61[0]));
			float fTemp65 = (fTemp42 / (fTemp41 * expf((fTemp19 + (fRec66[0] * fTemp20)))));
			fVec13[0] = fTemp65;
			float fTemp66 = (2000 * fVec13[0]);
			float fTemp67 = ((int((fTemp66 != (2000 * fVec13[1]))))?fVec11[0]:(fRec67[1] - 1));
			fRec67[0] = fTemp67;
			fRec68[0] = ((int((fTemp67 <= 0)))?fTemp66:(fRec68[1] + ((fTemp66 - fRec68[1]) / fTemp67)));
			float fTemp68 = (fConst6 * (7 + ((fRec68[0] * fabsf(((fConst9 * (fRec61[0] * fRec59[0])) - 0.5f))) + (fRec58[0] * (1 + sinf((fConst9 * (fRec51[0] * fRec49[0]))))))));
			float fTemp69 = ((int((fRec45[1] != 0.0f)))?((int(((fRec46[1] > 0.0f) & (fRec46[1] < 1.0f))))?fRec45[1]:0):((int(((fRec46[1] == 0.0f) & (fTemp68 != fRec47[1]))))?0.0009765625f:((int(((fRec46[1] == 1.0f) & (fTemp68 != fRec48[1]))))?-0.0009765625f:0)));
			fRec45[0] = fTemp69;
			fRec46[0] = max(0.0f, min(1.0f, (fRec46[1] + fTemp69)));
			fRec47[0] = ((int(((fRec46[1] >= 1.0f) & (fRec48[1] != fTemp68))))?fTemp68:fRec47[1]);
			fRec48[0] = ((int(((fRec46[1] <= 0.0f) & (fRec47[1] != fTemp68))))?fTemp68:fRec48[1]);
			float fTemp70 = min(fConst4, max(fConst5, fRec76[1]));
			fRec79[0] = ((int((((fabsf((fRec76[1] - fRec76[2])) > 0) * iRec78[1]) > 0)))?0:min(fTemp70, (1 + fRec79[1])));
			iRec78[0] = (fRec79[0] == fTemp70);
			int iTemp71 = ((abs((iRec78[0] - iRec78[1])) > 0) | iTemp15);
			fRec77[0] = ((fRec77[1] * (1 - iTemp71)) + (iTemp71 * fTemp62));
			fRec76[0] = (fConst6 * (0 - (1000 * logf(fRec77[0]))));
			float fTemp72 = min(fConst4, max(fConst5, fRec76[0]));
			fVec14[0] = fTemp72;
			int iTemp73 = (fabsf((fConst7 * ((1e+03f * fVec14[0]) - (1e+03f * fVec14[1])))) > 0);
			float fTemp74 = (0.5f + (2.3283064376228985e-10f * iRec13));
			fRec80[0] = ((fRec80[1] * (1 - iTemp73)) + (fTemp74 * iTemp73));
			float fTemp75 = expf((fTemp19 + (fRec80[0] * fTemp20)));
			fVec15[0] = fTemp75;
			float fTemp76 = ((int((fVec15[0] != fVec15[1])))?fVec14[0]:(fRec74[1] - 1));
			fRec74[0] = fTemp76;
			fRec75[0] = ((int((fTemp76 <= 0)))?fVec15[0]:(fRec75[1] + ((fVec15[0] - fRec75[1]) / fTemp76)));
			fRec73[0] = fmodf((1.0f + fRec73[1]),(fConst8 / fRec75[0]));
			float fTemp77 = (fTemp27 / (fVec15[0] * fTemp26));
			fVec16[0] = fTemp77;
			float fTemp78 = (318.309886f * fVec16[0]);
			float fTemp79 = ((int((fTemp78 != (318.309886f * fVec16[1]))))?fVec14[0]:(fRec81[1] - 1));
			fRec81[0] = fTemp79;
			fRec82[0] = ((int((fTemp79 <= 0)))?fTemp78:(fRec82[1] + ((fTemp78 - fRec82[1]) / fTemp79)));
			float fTemp80 = min(fConst4, max(fConst5, fRec86[1]));
			fRec89[0] = ((int((((fabsf((fRec86[1] - fRec86[2])) > 0) * iRec88[1]) > 0)))?0:min(fTemp80, (1 + fRec89[1])));
			iRec88[0] = (fRec89[0] == fTemp80);
			int iTemp81 = ((abs((iRec88[0] - iRec88[1])) > 0) | iTemp15);
			fRec87[0] = ((fRec87[1] * (1 - iTemp81)) + (iTemp81 * fTemp74));
			fRec86[0] = (fConst6 * (0 - (1000 * logf(fRec87[0]))));
			float fTemp82 = min(fConst4, max(fConst5, fRec86[0]));
			fVec17[0] = fTemp82;
			int iTemp83 = (fabsf((fConst7 * ((1e+03f * fVec17[0]) - (1e+03f * fVec17[1])))) > 0);
			fRec90[0] = ((fRec90[1] * (1 - iTemp83)) + (iTemp83 * (0.5f + (2.3283064376228985e-10f * iRec12[0]))));
			float fTemp84 = expf((fTemp34 + (fRec90[0] * fTemp35)));
			fVec18[0] = fTemp84;
			float fTemp85 = ((int((fVec18[0] != fVec18[1])))?fVec17[0]:(fRec84[1] - 1));
			fRec84[0] = fTemp85;
			fRec85[0] = ((int((fTemp85 <= 0)))?fVec18[0]:(fRec85[1] + ((fVec18[0] - fRec85[1]) / fTemp85)));
			fRec83[0] = fmodf((1.0f + fRec83[1]),(fConst8 / fRec85[0]));
			float fTemp86 = (fTemp42 / (fTemp41 * expf((fTemp19 + (fRec90[0] * fTemp20)))));
			fVec19[0] = fTemp86;
			float fTemp87 = (2000 * fVec19[0]);
			float fTemp88 = ((int((fTemp87 != (2000 * fVec19[1]))))?fVec17[0]:(fRec91[1] - 1));
			fRec91[0] = fTemp88;
			fRec92[0] = ((int((fTemp88 <= 0)))?fTemp87:(fRec92[1] + ((fTemp87 - fRec92[1]) / fTemp88)));
			float fTemp89 = (fConst6 * (3 + ((fRec92[0] * fabsf(((fConst9 * (fRec85[0] * fRec83[0])) - 0.5f))) + (fRec82[0] * (1 + sinf((fConst9 * (fRec75[0] * fRec73[0]))))))));
			float fTemp90 = ((int((fRec69[1] != 0.0f)))?((int(((fRec70[1] > 0.0f) & (fRec70[1] < 1.0f))))?fRec69[1]:0):((int(((fRec70[1] == 0.0f) & (fTemp89 != fRec71[1]))))?0.0009765625f:((int(((fRec70[1] == 1.0f) & (fTemp89 != fRec72[1]))))?-0.0009765625f:0)));
			fRec69[0] = fTemp90;
			fRec70[0] = max(0.0f, min(1.0f, (fRec70[1] + fTemp90)));
			fRec71[0] = ((int(((fRec70[1] >= 1.0f) & (fRec72[1] != fTemp89))))?fTemp89:fRec71[1]);
			fRec72[0] = ((int(((fRec70[1] <= 0.0f) & (fRec71[1] != fTemp89))))?fTemp89:fRec72[1]);
			fRec0[0] = ((fRec4[0] * fVec1[(IOTA-int((int(fRec6[0]) & 4095)))&4095]) + (((fRec46[0] * fVec1[(IOTA-int((int(fRec48[0]) & 4095)))&4095]) + ((((1.0f - fRec70[0]) * fVec1[(IOTA-int((int(fRec71[0]) & 4095)))&4095]) + (fRec70[0] * fVec1[(IOTA-int((int(fRec72[0]) & 4095)))&4095])) + ((1.0f - fRec46[0]) * fVec1[(IOTA-int((int(fRec47[0]) & 4095)))&4095]))) + ((1.0f - fRec4[0]) * fVec1[(IOTA-int((int(fRec5[0]) & 4095)))&4095])));
			output0[i] = (FAUSTFLOAT)fRec0[0];
			float fTemp91 = (fRec2[0] * fRec93[1]);
			fVec20[0] = fTemp91;
			fRec94[0] = (fConst3 * ((fVec20[0] - fVec20[1]) + (fConst2 * fRec94[1])));
			float fTemp92 = ((float)input1[i] + (0.25f * tanhf((2 * fRec94[0]))));
			fVec21[IOTA&4095] = fTemp92;
			float fTemp93 = min(fConst4, max(fConst5, fRec102[1]));
			fRec105[0] = ((int((((fabsf((fRec102[1] - fRec102[2])) > 0) * iRec104[1]) > 0)))?0:min(fTemp93, (1 + fRec105[1])));
			iRec104[0] = (fRec105[0] == fTemp93);
			int iTemp94 = ((abs((iRec104[0] - iRec104[1])) > 0) | iTemp15);
			fRec103[0] = ((fRec103[1] * (1 - iTemp94)) + (iTemp94 * (0.5f + (2.3283064376228985e-10f * iRec24))));
			fRec102[0] = (fConst6 * (0 - (1000 * logf(fRec103[0]))));
			float fTemp95 = min(fConst4, max(fConst5, fRec102[0]));
			fVec22[0] = fTemp95;
			int iTemp96 = (fabsf((fConst7 * ((1e+03f * fVec22[0]) - (1e+03f * fVec22[1])))) > 0);
			float fTemp97 = (0.5f + (2.3283064376228985e-10f * iRec23));
			fRec106[0] = ((fRec106[1] * (1 - iTemp96)) + (fTemp97 * iTemp96));
			float fTemp98 = expf((fTemp19 + (fRec106[0] * fTemp20)));
			fVec23[0] = fTemp98;
			float fTemp99 = ((int((fVec23[0] != fVec23[1])))?fVec22[0]:(fRec100[1] - 1));
			fRec100[0] = fTemp99;
			fRec101[0] = ((int((fTemp99 <= 0)))?fVec23[0]:(fRec101[1] + ((fVec23[0] - fRec101[1]) / fTemp99)));
			fRec99[0] = fmodf((1.0f + fRec99[1]),(fConst8 / fRec101[0]));
			float fTemp100 = (fTemp27 / (fTemp26 * fVec23[0]));
			fVec24[0] = fTemp100;
			float fTemp101 = (318.309886f * fVec24[0]);
			float fTemp102 = ((int((fTemp101 != (318.309886f * fVec24[1]))))?fVec22[0]:(fRec107[1] - 1));
			fRec107[0] = fTemp102;
			fRec108[0] = ((int((fTemp102 <= 0)))?fTemp101:(fRec108[1] + ((fTemp101 - fRec108[1]) / fTemp102)));
			float fTemp103 = min(fConst4, max(fConst5, fRec112[1]));
			fRec115[0] = ((int((((fabsf((fRec112[1] - fRec112[2])) > 0) * iRec114[1]) > 0)))?0:min(fTemp103, (1 + fRec115[1])));
			iRec114[0] = (fRec115[0] == fTemp103);
			int iTemp104 = ((abs((iRec114[0] - iRec114[1])) > 0) | iTemp15);
			fRec113[0] = ((fRec113[1] * (1 - iTemp104)) + (iTemp104 * fTemp97));
			fRec112[0] = (fConst6 * (0 - (1000 * logf(fRec113[0]))));
			float fTemp105 = min(fConst4, max(fConst5, fRec112[0]));
			fVec25[0] = fTemp105;
			int iTemp106 = (fabsf((fConst7 * ((1e+03f * fVec25[0]) - (1e+03f * fVec25[1])))) > 0);
			float fTemp107 = (0.5f + (2.3283064376228985e-10f * iRec22));
			fRec116[0] = ((fRec116[1] * (1 - iTemp106)) + (fTemp107 * iTemp106));
			float fTemp108 = expf((fTemp34 + (fRec116[0] * fTemp35)));
			fVec26[0] = fTemp108;
			float fTemp109 = ((int((fVec26[0] != fVec26[1])))?fVec25[0]:(fRec110[1] - 1));
			fRec110[0] = fTemp109;
			fRec111[0] = ((int((fTemp109 <= 0)))?fVec26[0]:(fRec111[1] + ((fVec26[0] - fRec111[1]) / fTemp109)));
			fRec109[0] = fmodf((1.0f + fRec109[1]),(fConst8 / fRec111[0]));
			float fTemp110 = (fTemp42 / (fTemp41 * expf((fTemp19 + (fRec116[0] * fTemp20)))));
			fVec27[0] = fTemp110;
			float fTemp111 = (2000 * fVec27[0]);
			float fTemp112 = ((int((fTemp111 != (2000 * fVec27[1]))))?fVec25[0]:(fRec117[1] - 1));
			fRec117[0] = fTemp112;
			fRec118[0] = ((int((fTemp112 <= 0)))?fTemp111:(fRec118[1] + ((fTemp111 - fRec118[1]) / fTemp112)));
			float fTemp113 = (fConst6 * (10 + ((fRec118[0] * fabsf(((fConst9 * (fRec111[0] * fRec109[0])) - 0.5f))) + (fRec108[0] * (1 + sinf((fConst9 * (fRec101[0] * fRec99[0]))))))));
			float fTemp114 = ((int((fRec95[1] != 0.0f)))?((int(((fRec96[1] > 0.0f) & (fRec96[1] < 1.0f))))?fRec95[1]:0):((int(((fRec96[1] == 0.0f) & (fTemp113 != fRec97[1]))))?0.0009765625f:((int(((fRec96[1] == 1.0f) & (fTemp113 != fRec98[1]))))?-0.0009765625f:0)));
			fRec95[0] = fTemp114;
			fRec96[0] = max(0.0f, min(1.0f, (fRec96[1] + fTemp114)));
			fRec97[0] = ((int(((fRec96[1] >= 1.0f) & (fRec98[1] != fTemp113))))?fTemp113:fRec97[1]);
			fRec98[0] = ((int(((fRec96[1] <= 0.0f) & (fRec97[1] != fTemp113))))?fTemp113:fRec98[1]);
			float fTemp115 = min(fConst4, max(fConst5, fRec126[1]));
			fRec129[0] = ((int((((fabsf((fRec126[1] - fRec126[2])) > 0) * iRec128[1]) > 0)))?0:min(fTemp115, (1 + fRec129[1])));
			iRec128[0] = (fRec129[0] == fTemp115);
			int iTemp116 = ((abs((iRec128[0] - iRec128[1])) > 0) | iTemp15);
			fRec127[0] = ((fRec127[1] * (1 - iTemp116)) + (iTemp116 * fTemp107));
			fRec126[0] = (fConst6 * (0 - (1000 * logf(fRec127[0]))));
			float fTemp117 = min(fConst4, max(fConst5, fRec126[0]));
			fVec28[0] = fTemp117;
			int iTemp118 = (fabsf((fConst7 * ((1e+03f * fVec28[0]) - (1e+03f * fVec28[1])))) > 0);
			float fTemp119 = (0.5f + (2.3283064376228985e-10f * iRec21));
			fRec130[0] = ((fRec130[1] * (1 - iTemp118)) + (fTemp119 * iTemp118));
			float fTemp120 = expf((fTemp19 + (fRec130[0] * fTemp20)));
			fVec29[0] = fTemp120;
			float fTemp121 = ((int((fVec29[0] != fVec29[1])))?fVec28[0]:(fRec124[1] - 1));
			fRec124[0] = fTemp121;
			fRec125[0] = ((int((fTemp121 <= 0)))?fVec29[0]:(fRec125[1] + ((fVec29[0] - fRec125[1]) / fTemp121)));
			fRec123[0] = fmodf((1.0f + fRec123[1]),(fConst8 / fRec125[0]));
			float fTemp122 = (fTemp27 / (fTemp26 * fVec29[0]));
			fVec30[0] = fTemp122;
			float fTemp123 = (318.309886f * fVec30[0]);
			float fTemp124 = ((int((fTemp123 != (318.309886f * fVec30[1]))))?fVec28[0]:(fRec131[1] - 1));
			fRec131[0] = fTemp124;
			fRec132[0] = ((int((fTemp124 <= 0)))?fTemp123:(fRec132[1] + ((fTemp123 - fRec132[1]) / fTemp124)));
			float fTemp125 = min(fConst4, max(fConst5, fRec136[1]));
			fRec139[0] = ((int((((fabsf((fRec136[1] - fRec136[2])) > 0) * iRec138[1]) > 0)))?0:min(fTemp125, (1 + fRec139[1])));
			iRec138[0] = (fRec139[0] == fTemp125);
			int iTemp126 = ((abs((iRec138[0] - iRec138[1])) > 0) | iTemp15);
			fRec137[0] = ((fRec137[1] * (1 - iTemp126)) + (iTemp126 * fTemp119));
			fRec136[0] = (fConst6 * (0 - (1000 * logf(fRec137[0]))));
			float fTemp127 = min(fConst4, max(fConst5, fRec136[0]));
			fVec31[0] = fTemp127;
			int iTemp128 = (fabsf((fConst7 * ((1e+03f * fVec31[0]) - (1e+03f * fVec31[1])))) > 0);
			float fTemp129 = (0.5f + (2.3283064376228985e-10f * iRec20));
			fRec140[0] = ((fRec140[1] * (1 - iTemp128)) + (fTemp129 * iTemp128));
			float fTemp130 = expf((fTemp34 + (fRec140[0] * fTemp35)));
			fVec32[0] = fTemp130;
			float fTemp131 = ((int((fVec32[0] != fVec32[1])))?fVec31[0]:(fRec134[1] - 1));
			fRec134[0] = fTemp131;
			fRec135[0] = ((int((fTemp131 <= 0)))?fVec32[0]:(fRec135[1] + ((fVec32[0] - fRec135[1]) / fTemp131)));
			fRec133[0] = fmodf((1.0f + fRec133[1]),(fConst8 / fRec135[0]));
			float fTemp132 = (fTemp42 / (fTemp41 * expf((fTemp19 + (fRec140[0] * fTemp20)))));
			fVec33[0] = fTemp132;
			float fTemp133 = (2000 * fVec33[0]);
			float fTemp134 = ((int((fTemp133 != (2000 * fVec33[1]))))?fVec31[0]:(fRec141[1] - 1));
			fRec141[0] = fTemp134;
			fRec142[0] = ((int((fTemp134 <= 0)))?fTemp133:(fRec142[1] + ((fTemp133 - fRec142[1]) / fTemp134)));
			float fTemp135 = (fConst6 * (7 + ((fRec142[0] * fabsf(((fConst9 * (fRec135[0] * fRec133[0])) - 0.5f))) + (fRec132[0] * (1 + sinf((fConst9 * (fRec125[0] * fRec123[0]))))))));
			float fTemp136 = ((int((fRec119[1] != 0.0f)))?((int(((fRec120[1] > 0.0f) & (fRec120[1] < 1.0f))))?fRec119[1]:0):((int(((fRec120[1] == 0.0f) & (fTemp135 != fRec121[1]))))?0.0009765625f:((int(((fRec120[1] == 1.0f) & (fTemp135 != fRec122[1]))))?-0.0009765625f:0)));
			fRec119[0] = fTemp136;
			fRec120[0] = max(0.0f, min(1.0f, (fRec120[1] + fTemp136)));
			fRec121[0] = ((int(((fRec120[1] >= 1.0f) & (fRec122[1] != fTemp135))))?fTemp135:fRec121[1]);
			fRec122[0] = ((int(((fRec120[1] <= 0.0f) & (fRec121[1] != fTemp135))))?fTemp135:fRec122[1]);
			float fTemp137 = min(fConst4, max(fConst5, fRec150[1]));
			fRec153[0] = ((int((((fabsf((fRec150[1] - fRec150[2])) > 0) * iRec152[1]) > 0)))?0:min(fTemp137, (1 + fRec153[1])));
			iRec152[0] = (fRec153[0] == fTemp137);
			int iTemp138 = ((abs((iRec152[0] - iRec152[1])) > 0) | iTemp15);
			fRec151[0] = ((fRec151[1] * (1 - iTemp138)) + (iTemp138 * fTemp129));
			fRec150[0] = (fConst6 * (0 - (1000 * logf(fRec151[0]))));
			float fTemp139 = min(fConst4, max(fConst5, fRec150[0]));
			fVec34[0] = fTemp139;
			int iTemp140 = (fabsf((fConst7 * ((1e+03f * fVec34[0]) - (1e+03f * fVec34[1])))) > 0);
			float fTemp141 = (0.5f + (2.3283064376228985e-10f * iRec19));
			fRec154[0] = ((fRec154[1] * (1 - iTemp140)) + (fTemp141 * iTemp140));
			float fTemp142 = expf((fTemp19 + (fRec154[0] * fTemp20)));
			fVec35[0] = fTemp142;
			float fTemp143 = ((int((fVec35[0] != fVec35[1])))?fVec34[0]:(fRec148[1] - 1));
			fRec148[0] = fTemp143;
			fRec149[0] = ((int((fTemp143 <= 0)))?fVec35[0]:(fRec149[1] + ((fVec35[0] - fRec149[1]) / fTemp143)));
			fRec147[0] = fmodf((1.0f + fRec147[1]),(fConst8 / fRec149[0]));
			float fTemp144 = (fTemp27 / (fTemp26 * fVec35[0]));
			fVec36[0] = fTemp144;
			float fTemp145 = (318.309886f * fVec36[0]);
			float fTemp146 = ((int((fTemp145 != (318.309886f * fVec36[1]))))?fVec34[0]:(fRec155[1] - 1));
			fRec155[0] = fTemp146;
			fRec156[0] = ((int((fTemp146 <= 0)))?fTemp145:(fRec156[1] + ((fTemp145 - fRec156[1]) / fTemp146)));
			float fTemp147 = min(fConst4, max(fConst5, fRec160[1]));
			fRec163[0] = ((int((((fabsf((fRec160[1] - fRec160[2])) > 0) * iRec162[1]) > 0)))?0:min(fTemp147, (1 + fRec163[1])));
			iRec162[0] = (fRec163[0] == fTemp147);
			int iTemp148 = ((abs((iRec162[0] - iRec162[1])) > 0) | iTemp15);
			fRec161[0] = ((fRec161[1] * (1 - iTemp148)) + (iTemp148 * fTemp141));
			fRec160[0] = (fConst6 * (0 - (1000 * logf(fRec161[0]))));
			float fTemp149 = min(fConst4, max(fConst5, fRec160[0]));
			fVec37[0] = fTemp149;
			int iTemp150 = (fabsf((fConst7 * ((1e+03f * fVec37[0]) - (1e+03f * fVec37[1])))) > 0);
			fRec164[0] = ((fRec164[1] * (1 - iTemp150)) + (fTemp14 * iTemp150));
			float fTemp151 = expf((fTemp34 + (fRec164[0] * fTemp35)));
			fVec38[0] = fTemp151;
			float fTemp152 = ((int((fVec38[0] != fVec38[1])))?fVec37[0]:(fRec158[1] - 1));
			fRec158[0] = fTemp152;
			fRec159[0] = ((int((fTemp152 <= 0)))?fVec38[0]:(fRec159[1] + ((fVec38[0] - fRec159[1]) / fTemp152)));
			fRec157[0] = fmodf((1.0f + fRec157[1]),(fConst8 / fRec159[0]));
			float fTemp153 = (fTemp42 / (fTemp41 * expf((fTemp19 + (fRec164[0] * fTemp20)))));
			fVec39[0] = fTemp153;
			float fTemp154 = (2000 * fVec39[0]);
			float fTemp155 = ((int((fTemp154 != (2000 * fVec39[1]))))?fVec37[0]:(fRec165[1] - 1));
			fRec165[0] = fTemp155;
			fRec166[0] = ((int((fTemp155 <= 0)))?fTemp154:(fRec166[1] + ((fTemp154 - fRec166[1]) / fTemp155)));
			float fTemp156 = (fConst6 * (3 + ((fRec166[0] * fabsf(((fConst9 * (fRec159[0] * fRec157[0])) - 0.5f))) + (fRec156[0] * (1 + sinf((fConst9 * (fRec149[0] * fRec147[0]))))))));
			float fTemp157 = ((int((fRec143[1] != 0.0f)))?((int(((fRec144[1] > 0.0f) & (fRec144[1] < 1.0f))))?fRec143[1]:0):((int(((fRec144[1] == 0.0f) & (fTemp156 != fRec145[1]))))?0.0009765625f:((int(((fRec144[1] == 1.0f) & (fTemp156 != fRec146[1]))))?-0.0009765625f:0)));
			fRec143[0] = fTemp157;
			fRec144[0] = max(0.0f, min(1.0f, (fRec144[1] + fTemp157)));
			fRec145[0] = ((int(((fRec144[1] >= 1.0f) & (fRec146[1] != fTemp156))))?fTemp156:fRec145[1]);
			fRec146[0] = ((int(((fRec144[1] <= 0.0f) & (fRec145[1] != fTemp156))))?fTemp156:fRec146[1]);
			fRec93[0] = ((fRec96[0] * fVec21[(IOTA-int((int(fRec98[0]) & 4095)))&4095]) + (((fRec120[0] * fVec21[(IOTA-int((int(fRec122[0]) & 4095)))&4095]) + ((((1.0f - fRec144[0]) * fVec21[(IOTA-int((int(fRec145[0]) & 4095)))&4095]) + (fRec144[0] * fVec21[(IOTA-int((int(fRec146[0]) & 4095)))&4095])) + ((1.0f - fRec120[0]) * fVec21[(IOTA-int((int(fRec121[0]) & 4095)))&4095]))) + ((1.0f - fRec96[0]) * fVec21[(IOTA-int((int(fRec97[0]) & 4095)))&4095])));
			output1[i] = (FAUSTFLOAT)fRec93[0];
			// post processing
			fRec93[1] = fRec93[0];
			fRec146[1] = fRec146[0];
			fRec145[1] = fRec145[0];
			fRec144[1] = fRec144[0];
			fRec143[1] = fRec143[0];
			fRec166[1] = fRec166[0];
			fRec165[1] = fRec165[0];
			fVec39[1] = fVec39[0];
			fRec157[1] = fRec157[0];
			fRec159[1] = fRec159[0];
			fRec158[1] = fRec158[0];
			fVec38[1] = fVec38[0];
			fRec164[1] = fRec164[0];
			fVec37[1] = fVec37[0];
			fRec160[2] = fRec160[1]; fRec160[1] = fRec160[0];
			fRec161[1] = fRec161[0];
			iRec162[1] = iRec162[0];
			fRec163[1] = fRec163[0];
			fRec156[1] = fRec156[0];
			fRec155[1] = fRec155[0];
			fVec36[1] = fVec36[0];
			fRec147[1] = fRec147[0];
			fRec149[1] = fRec149[0];
			fRec148[1] = fRec148[0];
			fVec35[1] = fVec35[0];
			fRec154[1] = fRec154[0];
			fVec34[1] = fVec34[0];
			fRec150[2] = fRec150[1]; fRec150[1] = fRec150[0];
			fRec151[1] = fRec151[0];
			iRec152[1] = iRec152[0];
			fRec153[1] = fRec153[0];
			fRec122[1] = fRec122[0];
			fRec121[1] = fRec121[0];
			fRec120[1] = fRec120[0];
			fRec119[1] = fRec119[0];
			fRec142[1] = fRec142[0];
			fRec141[1] = fRec141[0];
			fVec33[1] = fVec33[0];
			fRec133[1] = fRec133[0];
			fRec135[1] = fRec135[0];
			fRec134[1] = fRec134[0];
			fVec32[1] = fVec32[0];
			fRec140[1] = fRec140[0];
			fVec31[1] = fVec31[0];
			fRec136[2] = fRec136[1]; fRec136[1] = fRec136[0];
			fRec137[1] = fRec137[0];
			iRec138[1] = iRec138[0];
			fRec139[1] = fRec139[0];
			fRec132[1] = fRec132[0];
			fRec131[1] = fRec131[0];
			fVec30[1] = fVec30[0];
			fRec123[1] = fRec123[0];
			fRec125[1] = fRec125[0];
			fRec124[1] = fRec124[0];
			fVec29[1] = fVec29[0];
			fRec130[1] = fRec130[0];
			fVec28[1] = fVec28[0];
			fRec126[2] = fRec126[1]; fRec126[1] = fRec126[0];
			fRec127[1] = fRec127[0];
			iRec128[1] = iRec128[0];
			fRec129[1] = fRec129[0];
			fRec98[1] = fRec98[0];
			fRec97[1] = fRec97[0];
			fRec96[1] = fRec96[0];
			fRec95[1] = fRec95[0];
			fRec118[1] = fRec118[0];
			fRec117[1] = fRec117[0];
			fVec27[1] = fVec27[0];
			fRec109[1] = fRec109[0];
			fRec111[1] = fRec111[0];
			fRec110[1] = fRec110[0];
			fVec26[1] = fVec26[0];
			fRec116[1] = fRec116[0];
			fVec25[1] = fVec25[0];
			fRec112[2] = fRec112[1]; fRec112[1] = fRec112[0];
			fRec113[1] = fRec113[0];
			iRec114[1] = iRec114[0];
			fRec115[1] = fRec115[0];
			fRec108[1] = fRec108[0];
			fRec107[1] = fRec107[0];
			fVec24[1] = fVec24[0];
			fRec99[1] = fRec99[0];
			fRec101[1] = fRec101[0];
			fRec100[1] = fRec100[0];
			fVec23[1] = fVec23[0];
			fRec106[1] = fRec106[0];
			fVec22[1] = fVec22[0];
			fRec102[2] = fRec102[1]; fRec102[1] = fRec102[0];
			fRec103[1] = fRec103[0];
			iRec104[1] = iRec104[0];
			fRec105[1] = fRec105[0];
			fRec94[1] = fRec94[0];
			fVec20[1] = fVec20[0];
			fRec0[1] = fRec0[0];
			fRec72[1] = fRec72[0];
			fRec71[1] = fRec71[0];
			fRec70[1] = fRec70[0];
			fRec69[1] = fRec69[0];
			fRec92[1] = fRec92[0];
			fRec91[1] = fRec91[0];
			fVec19[1] = fVec19[0];
			fRec83[1] = fRec83[0];
			fRec85[1] = fRec85[0];
			fRec84[1] = fRec84[0];
			fVec18[1] = fVec18[0];
			fRec90[1] = fRec90[0];
			fVec17[1] = fVec17[0];
			fRec86[2] = fRec86[1]; fRec86[1] = fRec86[0];
			fRec87[1] = fRec87[0];
			iRec88[1] = iRec88[0];
			fRec89[1] = fRec89[0];
			fRec82[1] = fRec82[0];
			fRec81[1] = fRec81[0];
			fVec16[1] = fVec16[0];
			fRec73[1] = fRec73[0];
			fRec75[1] = fRec75[0];
			fRec74[1] = fRec74[0];
			fVec15[1] = fVec15[0];
			fRec80[1] = fRec80[0];
			fVec14[1] = fVec14[0];
			fRec76[2] = fRec76[1]; fRec76[1] = fRec76[0];
			fRec77[1] = fRec77[0];
			iRec78[1] = iRec78[0];
			fRec79[1] = fRec79[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec46[1] = fRec46[0];
			fRec45[1] = fRec45[0];
			fRec68[1] = fRec68[0];
			fRec67[1] = fRec67[0];
			fVec13[1] = fVec13[0];
			fRec59[1] = fRec59[0];
			fRec61[1] = fRec61[0];
			fRec60[1] = fRec60[0];
			fVec12[1] = fVec12[0];
			fRec66[1] = fRec66[0];
			fVec11[1] = fVec11[0];
			fRec62[2] = fRec62[1]; fRec62[1] = fRec62[0];
			fRec63[1] = fRec63[0];
			iRec64[1] = iRec64[0];
			fRec65[1] = fRec65[0];
			fRec58[1] = fRec58[0];
			fRec57[1] = fRec57[0];
			fVec10[1] = fVec10[0];
			fRec49[1] = fRec49[0];
			fRec51[1] = fRec51[0];
			fRec50[1] = fRec50[0];
			fVec9[1] = fVec9[0];
			fRec56[1] = fRec56[0];
			fVec8[1] = fVec8[0];
			fRec52[2] = fRec52[1]; fRec52[1] = fRec52[0];
			fRec53[1] = fRec53[0];
			iRec54[1] = iRec54[0];
			fRec55[1] = fRec55[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			fRec43[1] = fRec43[0];
			fRec42[1] = fRec42[0];
			fVec7[1] = fVec7[0];
			fRec44[1] = fRec44[0];
			fRec33[1] = fRec33[0];
			fRec35[1] = fRec35[0];
			fRec34[1] = fRec34[0];
			fVec6[1] = fVec6[0];
			fRec41[1] = fRec41[0];
			fRec40[1] = fRec40[0];
			fVec5[1] = fVec5[0];
			fRec36[2] = fRec36[1]; fRec36[1] = fRec36[0];
			fRec37[1] = fRec37[0];
			iRec38[1] = iRec38[0];
			fRec39[1] = fRec39[0];
			fRec31[1] = fRec31[0];
			fRec30[1] = fRec30[0];
			fVec4[1] = fVec4[0];
			fRec32[1] = fRec32[0];
			fRec7[1] = fRec7[0];
			fRec9[1] = fRec9[0];
			fRec8[1] = fRec8[0];
			fVec3[1] = fVec3[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fVec2[1] = fVec2[0];
			fRec10[2] = fRec10[1]; fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			iRec26[1] = iRec26[0];
			fRec27[1] = fRec27[0];
			iRec25[1] = iRec25[0];
			iRec12[1] = iRec12[0];
			IOTA = IOTA+1;
			fRec1[1] = fRec1[0];
			fVec0[1] = fVec0[0];
			fRec2[1] = fRec2[0];
		}
	}
};



/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/
					
mydsp		DSP;
std::list<GUI*>	GUI::fGuiList;

//-------------------------------------------------------------------------
// 									MAIN
//-------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	char appname[256];
	char rcfilename[256];
	char* home = getenv("HOME");
	
	snprintf(appname, 255, "%s", basename(argv[0]));
	snprintf(rcfilename, 255, "%s/.%src", home, appname);

	GUI* interface 	= new GTKUI (appname, &argc, &argv);
	FUI* finterface	= new FUI();
	DSP.buildUserInterface(interface);
	DSP.buildUserInterface(finterface);
    DSP.buildUserInterface(new PrintUI());

#ifdef HTTPCTRL
	httpdUI* httpdinterface = new httpdUI(appname, DSP.getNumInputs(), DSP.getNumOutputs(), argc, argv);
	DSP.buildUserInterface(httpdinterface);
 	std::cout << "HTTPD is on" << std::endl;
#endif

#ifdef OSCCTRL
	GUI* oscinterface = new OSCUI(appname, argc, argv);
	DSP.buildUserInterface(oscinterface);
#endif

#ifdef OCVCTRL
	std::cout<<"OCVCTRL defined"<<std::endl;
	OCVUI* ocvinterface = new OCVUI();
	DSP.buildUserInterface(ocvinterface);
#endif

	jackaudio audio;
	audio.init(appname, &DSP);
	finterface->recallState(rcfilename);	
	audio.start();
	
#ifdef HTTPCTRL
	httpdinterface->run();
#endif

#ifdef OSCCTRL
	oscinterface->run();
#endif

#ifdef OCVCTRL
	ocvinterface->run();
#endif

	interface->run();
	
	audio.stop();
	finterface->saveState(rcfilename);
    
    // desallocation
    delete interface;
    delete finterface;
#ifdef HTTPCTRL
	 delete httpdinterface;
#endif
#ifdef OSCCTRL
	 delete oscinterface;
#endif

#ifdef OCVCTRL
	 delete ocvinterface;
#endif

  	return 0;
}

		
/********************END ARCHITECTURE SECTION (part 2/2)****************/

