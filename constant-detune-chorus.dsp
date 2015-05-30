declare name 		"Chorus";
declare version 	"0.1";
declare author 		"Bart Brouns";
declare license 	"GNU 3.0";
declare copyright 	"(c) Bart Brouns 2014";
declare coauthors	"ported from a pd patch by Scott Nordlund, 2011";

/*

Scott Nordlund, 2011
http://puredata.hurleur.com/viewtopic.php?pid=29232#p29232
Strangely it turns out that the equation used to warp the phase of the cosine into an approximate sawtooth is useful for other things. I've used it to control a piecewise linear random modulator, and in a chorus algorithm that maintains constant and symmetric detuning depth (in cents), regardless of modulation rate


http://puredata.hurleur.com/viewtopic.php?pid=29407#p29407
Now that I'm looking at it, I don't really remember exactly how this relates to the CZ phase distortion stuff, but it's in there somewhere. You can one equation to map a [phasor~] to a piecewise linear function that turns a [cos~] into a phase distortion pseudo-sawtooth, or for controlling the distribution of a piecewise linear random modulator, or (if I remember right) to keep the "pitch shift up" and "pitch shift down" portions of a triangle-modulated delay symmetric. This last one isn't really obvious, but if you modulate a delay with a depth of, say, 1 octave, the rising portion will play back at 2x speed (+ 1 octave) while the falling portion will effectively "stop" the read pointer by slowing it to 0x speed (-infinite octaves, so to speak). The trick is to distort the triangle such that it's rising over one third of its period and falling over two thirds. So then you get 2x alternating with 0.5x, as you'd expect.
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
noiseMax	= 13;	// the number of unique noise generators
poisStart	= noiseMax/2; //the start of the noises generators used by pois
start = time<1; //the initial trigger for the SH
//SampleRate = 44100;
ms		= SR/1000; //milisecond in samples
poisMin		= 667;
poisMax 	= 4444;
delMax		= 4096; //21ms at 192 kHz
//-----------------------------------------------
// the GUI
//-----------------------------------------------
chorusGroup(x)  = hgroup("[0]chorus", x);

slowFreq	= chorusGroup(vslider("[0]slow freq",	0.57, 0, 5, 0.01):smooth(0.999));
slowDepth	= chorusGroup(vslider("[1]slow depth",	4, 0, 5, 0.01):pow(2):smooth(0.999));
fastFreq	= chorusGroup(vslider("[2]fast freq",	1, 0, 10, 0.01):smooth(0.999));
fastDepth	= chorusGroup(vslider("[3]fast depth",	4, 0, 10, 0.01):pow(2):smooth(0.999));
feedback	= chorusGroup(vslider("[4]feedback",	0, -1, 1, 0.01)*0.8:smooth(0.999));

//-----------------------------------------------
// the DSP
//-----------------------------------------------
SH(trig,x) = (*(1 - trig) + x * trig) ~_; //sample x and hold untill the next trig
changePulse= _ <: _, mem: - : abs:_>(0); //one sample pulse at start
noiseNr(nr) =(noises(noiseMax,nr)/2)+0.5;	// [0, 1] uniform

//-----------------------------------------------
// Poisson distribution event generator
//-----------------------------------------------
/*
en = enable (on/off)
lam = lambda (mean number of events per second); determines density of events 
min = minimum time between events in ms (this is clipped, not added). 
The second outlet is useful for creating piecewise linear functions or automatically scaling envelope times. Note that unlike other abstractions, this doesn't use a separate "panel", and the inlets aren't in a sensible order due to a re-arranged layout. Usually "pois" parameters in other abstractions refer to the lambda setting here. If lam is set to a new value, it will take effect once the next event is triggered. If lam is set to a very low value, there may be a very long delay before the next event. In this case, it can be turned off and on once to restart with a new lambda value.
*/
pois(nr) = ((SH((_|start),noiseNr(nr+1)):log:*(-1000):*(ms)) ~ (silentFor<:_,_)) :max(poisMin*ms):min(poisMax*ms):_/ms// split needed because SH uses x twice
with {
silentFor(time) =  (countup((time:max(poisMin*ms):min(poisMax*ms)), ((time:changePulse)*_))==(time:max(poisMin*ms):min(poisMax*ms)))~_:changePulse;
};
//:vbargraph("foo", poisMin, poisMax)

//-----------------------------------------------
// slow modulation
//-----------------------------------------------
smin = slowFreq;
smax = slowFreq*1.25;

freq(Fmin,Fmax,nr) = ((SH((pois(nr):changePulse),noiseNr(nr))  * ((Fmax:log)-(Fmin:log))) + (Fmin:log)):exp ;

slowD(nr) = (slowDepth:expr1:expr2) / freq(Fmin,Fmax,nr)
  with {
  expr1 = pow(2, _/1200);
  expr2 = _<:(2000*(_-1)/(_+1));
  };

slowTotal(nr) = ((lf_sawpos(line(freq(smin,smax,nr), pois(nr)))-0.5):abs) * (line((slowD(nr)),pois(nr)));
//-----------------------------------------------
// fast modulation
//-----------------------------------------------
Fmin = fastFreq;
Fmax = fastFreq*1.75;


fastD(nr) = (fastDepth:expr1:expr2) / freq(Fmin,Fmax,nr)
  with {
  expr1 = pow(2, _/1200);
  expr2 = _<:(318.309886*(_-1)/(_+1));
  };

fastTotal(nr) = ((lf_sawpos(line(freq(Fmin,Fmax,nr), pois(nr))):sin:_+1) * (line((fastD(nr)),pois(nr))) );


//-----------------------------------------------
// the chorus
//-----------------------------------------------

delayed(nr,add) = sdelay(delMax,1024,((slowTotal(nr)+fastTotal(nr+1)+add)*ms));

monoChorus(nr) = (_,_:+:_<:delayed(0+nr,3),delayed(2+nr,7),delayed(4+nr,10):>_)~( (_*feedback) : dcblockerat(200)*2: tanh*0.25) ;

stereoChorus = monoChorus(0),monoChorus(6);

process = stereoChorus;


