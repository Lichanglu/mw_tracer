
SRCDIR += links_c6xdsp/system links_c6xdsp/utils/src \
		links_c6xdsp/alg_link links_c6xdsp/alg_link/swosd \
		links_c6xdsp/alg_link/scd \
		links_c6xdsp/alg_link/track \
		links_c6xdsp/alg_link/stutrack \
		links_c6xdsp/alg_link/stusidetrack \
		links_c6xdsp/alg_link/studoubletrack \
		links_c6xdsp/alg_link/ceiltrack \
		links_c6xdsp/alg_link/blackboardtrack

SRCS_UTILS = utils_dsp.c

SRCS_ALGLINK    = algLink_tsk.c algLink_priv.c

SRCS_OSDLINK    = osdLink_alg.c  

SRCS_SCDLINK    = scdLink_alg.c 

SRCS_TRACKLINK  = trackLink_alg.c 

SRCS_STUTRACKLINK  = stutrackLink_alg.c 

SRCS_STUSIDETRACKLINK  = stusidetrackLink_alg.c 

SRCS_STUDOUBLETRACKLINK  = studoubletrackLink_alg.c 

SRCS_CEILTRACKLINK  = ceiltrackLink_alg.c 

SRCS_BLACKBOARDTRACKLINK  = blackboardtrackLink_alg.c 

SRCS_SYSTEMDSP  = system_c6xdsp.c systemLink_tsk_c6xdsp.c 

SRCS_c6xdsp += $(SRCS_SYSTEMDSP) $(SRCS_UTILS) $(SRCS_ALGLINK) $(SRCS_OSDLINK) $(SRCS_SCDLINK) $(SRCS_TRACKLINK) $(SRCS_STUTRACKLINK) $(SRCS_STUSIDETRACKLINK) $(SRCS_STUDOUBLETRACKLINK) $(SRCS_CEILTRACKLINK) $(SRCS_BLACKBOARDTRACKLINK) 


