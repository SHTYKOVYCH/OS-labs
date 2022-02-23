/*
	Возвращает 1 если всё хорошо.
	Возвращает 0 в случае ошибки.

	Алярма: сейчас код ошибки 0 - единый для всех случаев.
*/
int parseArgs(int argc, char **argv, Args *args)
{
	args = (Args*) malloc(sizeof(Args));

	args.depack = 0;
	args.outputFile = "archive";
	args.inputFile = "";

	if ((argc > 6) || (argc < 3))
	{
		return 0;
	}

	// 1 - следующий аргумент обрабатывается как имя файла
	int parseInput = 0;
    int parseOutput = 0;

    /*
    	TODO:

    	1) Подумать, можно ли избавиться от стены if-ов
    	2) Найти более извращённые тест-кейсы
    	3) Возможно, есть смысл разработать различные коды ошибок

    */
    for (int i = 1; i < argc; i++)
    {
    	char *arg = argv[i];
    	if (!strcmp(arg, "-d"))	// Найден флаг деархивации
    	{
    		args.depack = 1;
    	}
    	else if (!strcmp(arg, "-i")) // Найден флаг входного файла
    	{
    		parseInput = 1;
    	}
    	else if (!strcmp(arg, "-o")) // Найден флаг выходного файла (архива)
    	{
    		parseOutput = 1;
    	}
    	else
    	{
    		if(parseInput && parseOutput)	// Два флага подряд => указан ключ без имени файла
    		{
    			return 0;
    		}

    		if (strstr(arg, ".") || strstr(arg, "/"))	// Исключение запрещённых символов в имени файла 
    		{
    			return 0;
    		}

    		if (arg[0] == '-' || strlen(arg) > 255)	// Неизвестный ключ / слишком большое имя файла = смерть
    		{
    			return 0;
    		}
    		else if (parseInput)
        	{
        		args.inputFile = arg;
        		parseInput = 0;
        	}
        	else if (parseOutput)
        	{
        		args.outputFile = arg;
        		parseOutput = 0;
        	}
        	else
        	{
        		return 0;
        	}
    	}
    }

    if (parseOutput || parseInput)	// Остался необработанный ключ = смерть
    {
    	return 0;
    }

	return 1;
}