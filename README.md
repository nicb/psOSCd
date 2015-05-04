# psOSCd

psOSCd is a daemon that transforms PhaseSpace Impulse markers into
OSC-protocol triplets (x/y/z) for each marker.

It has been initially developed on an Ubuntu Karmic box.

## USAGE

    psOSCd -p phasespace_server_address -o osc_server_address

## REQUIREMENTS

+ liblo0     - libOSC ligthweight library (as per Ubuntu distribution)
+ libconfig8 - configuration library
+ libowlsock - PhaseSpace library         (available when purchasing a PhaseSpase system)

## LICENSE

Since the PhaseSpace library is  not  licensed  under  a  Free  Software
compliant library, psOSCd is licensed under the GNU LGPL v.2.0 license.
