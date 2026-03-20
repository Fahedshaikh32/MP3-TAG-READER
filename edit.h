#ifndef EDIT_H
#define EDIT_H

#include "common.h"

// This function edits a specific tag inside the MP3 file.
// filename : which MP3 file to edit
// flag     : which tag to change (-t, -a, -A, -y, etc.)
// new_value: what new text the user wants to put in the tag
Status edit_tag(const char *filename, const char *flag, const char *new_value);

#endif // EDIT_H
