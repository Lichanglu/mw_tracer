
SRCDIR += alg/track/src


SRCS_TRACK = track.c
SRCS_TRACK += tracking.c
SRCS_TRACK += AutoTrack.c

SRCS_c6xdsp +=  $(SRCS_TRACK)
INCDIR += $(SRCDIR)

