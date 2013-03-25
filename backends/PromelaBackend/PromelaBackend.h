#ifndef _PROMELABACKEND_H
#define _PROMELABACKEND_H

#include "BackendOption.h"
class Frontend;

extern void launch_promelabackend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks, bool bug);

void launch_promelabackend(Frontend * fe,
			BackendOption& option)
{
    launch_promelabackend(fe, option.OutputFilename,
        option.EventsAsBool, option.RelativeClocks, option.Bug);
};
#endif
