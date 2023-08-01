#include "mv.h"

int main(int argc, char** argv) {
    Mv mv = new_mv();
    if (argc == 2) {
        mv_program_from_file(&mv, argv[1]);
    }

    mv_run(mv, true);

    mv_close(mv);
}
