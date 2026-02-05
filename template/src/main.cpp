#include <iostream>
#include "root.hpp"

#define DEBUG _DEBUG

int main(int argc, char *argv[])
{
    Root root;
    
    /* Process command line options */
    root.ParseOptions(argc, argv);
    
    /* Update settings.yaml and exit */
    if(root.export_files)
    {
        root.ExportFiles();
        return 0;
    }

    /* Run simulator */
    root.Run();
    return 0;
}
