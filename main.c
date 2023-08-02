#include "src/mv.h"

int main(int argc, char** argv){
    if (argc == 2) {
        Mv mv = new_mv();
        mv_program_from_file(&mv, argv[1]);
        mv_run(mv, false);
        mv_close(mv);
    }
}
