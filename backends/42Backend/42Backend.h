#ifndef _42BACKEND_H
#define _42BACKEND_H

struct Frontend;

void launch_42backend(Frontend * fe,
			std::string OutputFilename,
			bool useBoolInsteadOfInts,
			bool relativeClocks, bool bug);

#endif
