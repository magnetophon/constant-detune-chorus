declare name 		"Chorus";
declare version 		"0.1";
declare author 		"Bart Brouns";
declare license 		"GNU 3.0";
declare copyright 	"(c) Bart Brouns 2014";
declare coauthors	"ported from a pd patch by Scott Nordlund, 2011";

/*
Scott Nordlund, 2011

Poisson distribution event generator
en = enable (on/off)
lam = lambda (mean number of events per second); determines density of events 
min = minimum time between events in ms (this is clipped, not added). 
The second outlet is useful for creating piecewise linear functions or automatically scaling envelope times. Note that unlike other abstractions, this doesn't use a separate "panel", and the inlets aren't in a sensible order due to a re-arranged layout. Usually "pois" parameters in other abstractions refer to the lambda setting here. If lam is set to a new value, it will take effect once the next event is triggered. If lam is set to a very low value, there may be a very long delay before the next event. In this case, it can be turned off and on once to restart with a new lambda value.
*/


//-----------------------------------------------
// imports
//-----------------------------------------------

import ("oscillator.lib");
import ("maxmsp.lib");
import ("effect.lib");

//-----------------------------------------------
// contants
//-----------------------------------------------
noiseMax	= 6;	// the number of unique noise generators
start = time<1; //the initial trigger for the SH
SampleRate = 44100;
ms			= SR/1000; //milisecond in samples
//-----------------------------------------------
// the GUI
//-----------------------------------------------
chorusGroup(x)  = hgroup("[0]chorus", x);

slowFreq	= chorusGroup(vslider("[0]slow freq",	1, 0, 1.25, 0.01):smooth(0.999));
slowDepth	= chorusGroup(vslider("[1]slow depth",	0.3, 0, 1, 0.01):pow(2):smooth(0.999));
fastFreq	= chorusGroup(vslider("[2]fast freq",	1, 0, 1.75, 0.01):smooth(0.999));
fastDepth	= chorusGroup(vslider("[3]fast depth",	0.3, 0, 1, 0.01):pow(2):smooth(0.999));
feedback	= chorusGroup(vslider("[4]feedback",	1, 0, 1, 0.01)
*2:_-1<:(_,((_>0)+1)):/:smooth(0.999));
//-----------------------------------------------
// the DSP
//-----------------------------------------------
SH(trig,x) = (*(1 - trig) + x * trig) ~_; //sample x and hold untill the next trig
changePulse= _ <: _, mem: - : abs:_>(0); //one sample pulse at start

pois(nr) = ((SH((_|start),noise):log:vbargraph("foo", -10, 0):*(-1000):*(ms)) ~ (silentFor<:_,_)) :max(666):min(3333)// split needed because SH uses x twice
with {
noise =(noises(noiseMax,nr)/2.1)+0.5;	// [0.05, 0.98] uniform
delayedPulse(x) = (x:changePulse:@(x:max(666*ms):min(3333*ms))); //clipping needed against compiler error
silentFor(time) =  (countup((time:max(666*ms):min(3333*ms)), ((time:changePulse)*_))==(time:max(666*ms):min(3333*ms)))~_:changePulse;
};
//12:(+)~_;
//log((noises(noiseMax,nr):SH(_|start))*(-1000):max(666))~_;

//(((_<x),_:+)~(_<:_,_)) ; 


//process = silentfor(vslider("[4]feedback",	0, 0, 44100, 1),button("foo"));
//process =  button("foo"):silentfor(vslider("[4]feedback",	0, 0, 1000*ms, 1));
process = pois(1);

//process = vslider("[4]feedback",	0.5, 0.002, 0.995, 0.01):log:vbargraph("s", -10, 0);
//vslider("[4]feedback",	1, 0, 1, 0.01):changePulse:vbargraph("s", -1, 1);
//
//SH(oscs(10)==1,(noises(noiseMax,1)/2)+0.5):vbargraph("s", -1, 2);
//pois(2);



