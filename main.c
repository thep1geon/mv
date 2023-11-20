#define STACK_CAP 10240

#ifndef INCLUDE_PATH
#define INCLUDE_PATH "./include/"
#endif  //INCLUDE_PATH

#include "src/mv.h"
#include <string.h>

int main(int argc, char** argv){
    if (argc >= 2) {
        Mv mv = new_mv();
        mv_program_from_file(&mv, argv[1]);
        
        bool debug = false;
        if (argc == 3) {
            if (strcmp(argv[2], "-debug") == 0) {
                debug = true;
            };
        }

        mv_run(mv, debug);
        mv_close(mv);
    }
}
