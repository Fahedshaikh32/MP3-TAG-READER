#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "view.h"
#include "edit.h"

// This function only prints the help menu.
// It tells the user how to use the program correctly
// and what each flag (-t, -a etc.) means.
void print_help()
{
    printf("usage: mp3tag -[tTaAycg] \"value\" file1\n");
    printf("       mp3tag -v\n\n");

    printf("    -t   Edit Title Tag (Song Name)\n");
    printf("    -T   Edit Track Number Tag\n");
    printf("    -a   Edit Artist Tag (Singer Name)\n");
    printf("    -A   Edit Album Tag\n");
    printf("    -y   Edit Year Tag (Release Year)\n");
    printf("    -c   Edit Comment Tag\n");
    printf("    -g   Edit Genre Tag (Music Type)\n");
    printf("    -h   Display Help Menu\n");
    printf("    -v   View Tag Information of MP3 File\n");
}

// This function prints an error message when the user types
// wrong command or wrong number of arguments.
// It also shows correct usage so user can fix the mistake.
void print_usage(int ac, char *av[])
{
    printf("--------------------------------------------------------------\n");

    printf("ERROR: ");
    for (int i = 0; i < ac; i++)
    {
        printf("%s ", av[i]);
    }
    printf(": INVALID ARGUMENTS\n");

    printf("USAGE :\n");
    printf("To view please pass like: ./a.out -v mp3filename\n");
    printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c <new_value> mp3filename\n");

    printf("--------------------------------------------------------------\n");
}

void print_edit_error()
{
    printf("------------------SELECTED EDIT DETAILS-----------------\n\n");
    printf("Please pass tag arguments which you want to edit\n");
    printf("eg: ./a.out -e -t/-a/-A/-y/-m/-c <new_value> mp3filename\n");
    printf("\n--------------------------------------------------------------\n");
}



// This function checks what user wants to do based on
// the first argument (-v, -e, -h).
// It returns a code so main() can take decision.
static Op get_op(int ac, char *av[])
{
    if (ac < 2)
    {
        return OP_NONE;   // no argument given → invalid
    }

    if (strcmp(av[1], "-v") == 0)
    {
        return OP_V;      // user wants to VIEW tags
    }

    if (strcmp(av[1], "-e") == 0)
    {
        return OP_E;      // user wants to EDIT tags
    }

    if (strcmp(av[1], "-h") == 0 || strcmp(av[1], "--help") == 0)
    {
        return OP_H;      // user wants HELP menu
    }

    return OP_NONE;       // unknown option → treat as invalid
}

int main(int ac, char *av[])
{
    // First, check user input using get_op()
    Op op = get_op(ac, av);

    switch (op)
    {
        case OP_H:
            // If user asks help → just print help menu and exit
            print_help();
            return 0;

        case OP_V:
            // For viewing tags → we need exactly 3 arguments:
            // ./a.out -v filename.mp3
            if (ac != 3)
            {
                print_usage(ac, av);  // Wrong number of arguments
                return 1;
            }

            // Call view function with filename (3rd argument)
            return (view_file(av[2]) == STATUS_SUCCESS) ? 0 : 1;

        case OP_E:
            // For editing a tag → we need exactly 5 arguments:
            // ./a.out -e -t "New Name" filename.mp3
            if (ac != 5)
            {
                print_edit_error();
                return 1;
            }

            // edit_tag(file, flag, value)
            // file → av[4]
            // flag → av[2] (-t, -a etc.)
            // value → av[3] ("New value")
            if (edit_tag(av[4], av[2], av[3]) == STATUS_SUCCESS)
            {
                printf("Edit done successfully.\n");
                return 0;   // success
            }
            else
            {
                printf("Edit failed.\n");
                return 1;   // something went wrong
            }
    }

    // If the switch didn't match any valid option:
    print_usage(ac, av);
    return 1;
}
