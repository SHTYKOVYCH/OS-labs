//
// Created by dimitis on 27.02.2022.
//

#include "error_codes.h"
#include "error_handler.h"
#include "parse-args.h"
#include "json_stringify.h"
#include "depack.h"

int main(int argc, char* argv[])
{
    int errorCode;
    Args args = {};

    errorCode = parseArgs(argc, argv, &args);

    if (errorCode != SUCCESS) {
        return 1;
    }

    if (args.depack) {
        depack(&args);
    }

    return 0;
}