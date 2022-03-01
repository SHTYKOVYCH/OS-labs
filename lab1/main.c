//
// Created by dimitis on 27.02.2022.
//

#include "error_codes.h"
#include "error_handler.h"
#include "parse-args.h"

int main(int argc, char* argv[])
{
    int errorCode;
    Args args = {};

    errorCode = parseArgs(argc, argv, &args);

    if (errorCode != SUCCESS) {
        return 1;
    }

    return 0;
}