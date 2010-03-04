#ifndef _PROMELABACKEND_H
#define _PROMELABACKEND_H

struct Frontend;

void launch_promelabackend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks);

#endif
