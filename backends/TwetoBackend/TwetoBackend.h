#ifndef _TWETOBACKEND_H
#define _TWETOBACKEND_H

struct Frontend;

void launch_twetobackend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks, bool bug);

#endif
