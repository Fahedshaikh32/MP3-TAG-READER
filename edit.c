#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edit.h"

static unsigned int read_be_uint32(unsigned char buf[4])
{
    return ((unsigned int)buf[0] << 24) | ((unsigned int)buf[1] << 16) | ((unsigned int)buf[2] << 8) | buf[3];   // convert 4 bytes to proper size number
}

static void write_be_uint32(unsigned char buf[4], unsigned int val)
{
    buf[0] = (val >> 24) & 0xFF;    // store biggest byte first
    buf[1] = (val >> 16) & 0xFF;    // big endian format
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

static unsigned int syncsafe_to_int(unsigned char buf[4])
{
    return ((buf[0] & 0x7F) << 21) | ((buf[1] & 0x7F) << 14) | ((buf[2] & 0x7F) << 7) | (buf[3] & 0x7F);  // convert ID3v2 sync-safe size to normal number
}

Status edit_tag(const char *filename, const char *flag, const char *new_value)
{
    char frame_to_find[5] = {0};     // will store frame name like TIT2, TPE1 etc.

    if (strcmp(flag, "-t") == 0)
    {
        strcpy(frame_to_find, "TIT2");   // -t → Title
    }
    else if (strcmp(flag, "-a") == 0)
    {
        strcpy(frame_to_find, "TPE1");   // -a → Artist
    }
    else if (strcmp(flag, "-A") == 0)
    {
        strcpy(frame_to_find, "TALB");   // -A → Album
    }
    else if (strcmp(flag, "-y") == 0)
    {
        strcpy(frame_to_find, "TYER");   // -y → Year
    }
    else if (strcmp(flag, "-c") == 0)
    {
        strcpy(frame_to_find, "COMM");   // -c → Comment
    }
    else if (strcmp(flag, "-T") == 0)
    {
        strcpy(frame_to_find, "TRCK");   // -T → Track number
    }
    else if (strcmp(flag, "-g") == 0)
    {
        strcpy(frame_to_find, "TCON");   // -g → Genre
    }
    else
    {
        printf("ERROR: Unknown flag\n");   // invalid request given by user
        return STATUS_FAILURE;
    }

    FILE *mp3 = fopen(filename, "rb");   // open original mp3 file for reading
    if (!mp3)
    {
        printf("ERROR: Cannot open file\n"); 
        return STATUS_FAILURE; 
    }

    FILE *temp = fopen("temp.mp3", "wb");  // new temporary file where we will write modifications
    if (!temp)
    {
        fclose(mp3);
        printf("ERROR: Cannot create temporary output\n");
        return STATUS_FAILURE;
    }

    unsigned char header[10];
    fread(header, 1, 10, mp3);    // read ID3 header from original
    fwrite(header, 1, 10, temp);  // write same header to temp file

    unsigned int tag_size = syncsafe_to_int(header + 6);   // read total metadata size from header

    unsigned int used = 0;   // count how many bytes processed inside tag frames

    while (used < tag_size)   // loop until all frames processed
    {
        unsigned char frame_header[10];
        fread(frame_header, 1, 10, mp3);  // read frame header
        used += 10;

        if (!frame_header[0])            // padding reached → no more frames
        {
            fwrite(frame_header, 1, 10, temp);
            break;
        }

        char fid[5] = {0};
        memcpy(fid, frame_header, 4);    // store frame name like TIT2

        unsigned int frame_size = read_be_uint32(frame_header + 4);   // read size of this frame

        unsigned char *data = malloc(frame_size);
        fread(data, 1, frame_size, mp3);   // read actual frame content
        used += frame_size;

        if (strcmp(fid, frame_to_find) == 0)
        {
            printf("Updating %s -> %s\n", fid, new_value);   // tell which tag is being changed

            unsigned int new_size = 1 + strlen(new_value);    // size for new text + encoding byte
            unsigned char *new_data = malloc(new_size);

            new_data[0] = 0x00;                // encoding = ISO Latin text
            memcpy(new_data + 1, new_value, strlen(new_value));  // copy new text

            fwrite(fid, 1, 4, temp);          // write same frame name back

            unsigned char sizebuf[4];
            write_be_uint32(sizebuf, new_size); 
            fwrite(sizebuf, 1, 4, temp);      // store new size

            fwrite("\0\0", 1, 2, temp);       // flags always zero

            fwrite(new_data, 1, new_size, temp);  // write updated text

            free(new_data);
        }
        else
        {
            fwrite(frame_header, 1, 10, temp);  // copy header as it is for other frames
            fwrite(data, 1, frame_size, temp);  // copy rest of frame untouched
        }

        free(data);
    }

    unsigned char buff[4096];
    size_t n;

    while ((n = fread(buff, 1, sizeof(buff), mp3)) > 0)   // copy remaining audio section bytes
    {
        fwrite(buff, 1, n, temp);    // write audio part in temp file as it is
    }

    fclose(mp3);
    fclose(temp);

    remove(filename);                // delete old file
    rename("temp.mp3", filename);    // rename temp to original file name

    printf("Tag updated successfully!\n");
    return STATUS_SUCCESS;           // everything done properly
}
