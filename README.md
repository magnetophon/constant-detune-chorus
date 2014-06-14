constant-detune-chorus
======================


A chorus algorithm that maintains constant and symmetric detuning depth (in cents), regardless of modulation rate.

Ported from a pd patch by Scott Nordlund, 2011



Some explanaition by Scott:

If you modulate a delay with a depth of, say, 1 octave, the rising portion will play back at 2x speed (+ 1 octave) while the falling portion will effectively "stop" the read pointer by slowing it to 0x speed (-infinite octaves, so to speak). 

The trick is to distort the triangle such that it's rising over one third of its period and falling over two thirds. So then you get 2x alternating with 0.5x, as you'd expect.
