#ifndef _42BACKEND_H
#define _42BACKEND_H

#include "BackendOption.h"
struct Frontend;

extern void launch_42backend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks, bool bug);

void launch_42backend(Frontend * fe,
			BackendOption& option)
{
    launch_42backend(fe, option.OutputFilename,
        option.EventsAsBool, option.RelativeClocks, option.Bug);
};
#endif
