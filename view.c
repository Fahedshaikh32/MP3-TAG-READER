#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "view.h"

// This function converts 4 bytes into a single number.
// In ID3 tags, size is stored in 4 bytes (big-endian).
static unsigned int get_size(unsigned char b[4])
{
    return ((unsigned int)b[0] << 24) |    // shift byte 0 and merge
           ((unsigned int)b[1] << 16) |    // shift byte 1 and merge
           ((unsigned int)b[2] << 8 ) |    // shift byte 2 and merge
            (unsigned int)b[3];            // last byte stays same
}

Status view_file(const char *filename)
{
    FILE *file = fopen(filename, "rb");    // open file in binary read mode
    if (!file)
    {
        printf("ERROR: Cannot open file '%s'\n", filename);
        return STATUS_FAILURE;
    }

    unsigned char header[10];
    fread(header, 1, 10, file);            // Read first 10 bytes -> ID3 header

    // Every valid MP3 tag starts with "ID3".
    // If not found → no metadata present.
    if (strncmp((char *)header, "ID3", 3) != 0)
    {
        printf("ERROR: No ID3 tag found in file\n");
        fclose(file);
        return STATUS_FAILURE;
    }

    // These 4 bytes tell how big the tag area is.
    // Masking (0x7F) is required because highest bit is not used.
    unsigned int tag_size =
        ((header[6] & 0x7F) << 21) |
        ((header[7] & 0x7F) << 14) |
        ((header[8] & 0x7F) << 7)  |
         (header[9] & 0x7F);

    printf("--------------------------------------------------\n");
    printf(" MP3 TAG INFORMATION\n");
    printf(" File Name : %s\n", filename);
    printf(" Tag Size  : %u bytes\n", tag_size);
    printf("--------------------------------------------------\n");

    unsigned int used_bytes = 0;           // Track how many bytes we have read
    int frame_count = 0;                   // To avoid too much output

    // Loop to read all frames inside the tag area
    while (used_bytes < tag_size && frame_count < 20)
    {
        unsigned char frame_header[10];
        fread(frame_header, 1, 10, file);  // Read next frame header
        used_bytes += 10;

        // If all header bytes are zero, it means padding (end of frames)
        if (!frame_header[0] && !frame_header[1] &&
            !frame_header[2] && !frame_header[3])
        {
            break;
        }

        char frame_id[5] = {0};            // Store frame name (TIT2..TPE1 etc.)
        memcpy(frame_id, frame_header, 4);

        unsigned int frame_size = get_size(frame_header + 4); // Read size value

        unsigned char *data = malloc(frame_size + 1);
        fread(data, 1, frame_size, file); // Read actual content
        data[frame_size] = '\0';          // Make sure string ends

        // In most text frames, first byte is encoding info,
        // so we skip 1 byte to show only readable text.
        char *text = (char *)(data + 1);

        // Compare frame_id and print known tags clearly
        if (!strcmp(frame_id, "TIT2"))
        {
            printf("Title    : %s\n", text);    // Song name
        }
        else if (!strcmp(frame_id, "TPE1"))
        {
            printf("Artist   : %s\n", text);    // Singer name
        }
        else if (!strcmp(frame_id, "TALB"))
        {
            printf("Album    : %s\n", text);    // Album name
        }
        else if (!strcmp(frame_id, "TYER") || !strcmp(frame_id, "TDRC"))
        {
            printf("Year     : %s\n", text);    // Release Year
        }
        else if (!strcmp(frame_id, "TCON"))
        {
            printf("Genre    : %s\n", text);    // Music type
        }
        else if (!strcmp(frame_id, "COMM"))
        {
            printf("Comment  : %s\n", text + 4); // Skip language code
        }

        free(data);                        // Release memory
        used_bytes += frame_size;          // Count data read
        frame_count++;                     // Count printed frames
    }

    printf("--------------------------------------------------\n");
    fclose(file);
    return STATUS_SUCCESS;
}
