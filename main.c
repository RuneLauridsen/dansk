#include "base/base.h"
#include "dk.h"
#include "dk.c"

#include "dk_tests.h"
#include "dk_tests.c"

int main(int argc, char **argv) {
    SetConsoleOutputCP(65001); // utf8

    dk_run_all_tests();
    //dk_run_test_file_filtered(str("W:\\dansk\\tests\\tests.dk"), str("err mix"));

    return 0;
}
