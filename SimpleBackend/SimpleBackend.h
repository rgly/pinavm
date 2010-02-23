#ifndef _SIMPLEBACKEND_H
#define _SIMPLEBACKEND_H

struct Frontend;

void launch_simplebackend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks);
#endif
