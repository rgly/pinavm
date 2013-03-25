#ifndef _SIMPLEBACKEND_H
#define _SIMPLEBACKEND_H

#include "BackendOption.h"
struct Frontend;

extern void launch_simplebackend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks);

void launch_simplebackend(Frontend * fe,
			BackendOption& option)
{
    launch_simplebackend(fe, option.OutputFilename,
        option.EventsAsBool, option.RelativeClocks);
};
#endif
