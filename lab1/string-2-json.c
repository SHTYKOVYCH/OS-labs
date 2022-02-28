#include <stdlib.h>
#include <string.h>

#include "string-2-json.h"
#include "parse-args.h"

int string2json(const char* string, struct json* jason)
{
	char buff[255];
	memset(buff, 0, 255);

	for (int i = 1; i < strlen(string) - 1; ++i)
	{
		buff[strlen(buff)] = string[i];	// Грузим символ
		if (!strcmp(buff, "name"))	// Если встретили параметр
		{
			i += 3;	// Исходя из предположения о верной структуре, заключаем, что после ключа идёт :" а после этого уже значение
			memset(buff, 0, strlen(buff));	// Чистим

			for (; i < strlen(string); ++i)
			{
				char c = string[i];
				if ((c == '\\') && (string[i + 1] == '\"'))	// Кошда за \ следует ", запоминаем " без \ и идём дальше
				{
					buff[strlen(buff)] = string[i + 1];
					i++;
					continue;
				}
				if (c == '\"')	// Если просто ", закрываемся
				{
					jason->name = (char*)malloc(strlen(buff));
					strcpy(jason->name, parseFilename(buff, 'f'));
					break;
				}
				buff[strlen(buff)] = c;
			}

			i += 1;
		}
		else if (!strcmp(buff, "type"))
		{
			i += 5;	// Не ну в принипе type не нужен. Он бы пригодился если шёл первым аргументом. Тогда и дублирования кода ниже, может быть, и не было
		}
		else if (!strcmp(buff, "parentDir"))
		{
			i += 3;
			memset(buff, 0, strlen(buff));

			for (; i < strlen(string); ++i)	// Некоторые скажут: "Коля, но это же дублирование кода"
			{								// На что я брошу загадочный взгляд в мексиканской шляпи и отвечу: "Да."
				char c = string[i];
				if ((c == '\\') && (string[i + 1] == '\"'))
				{
					buff[strlen(buff)] = string[i + 1];
					i++;
					continue;
				}
				if (c == '\"')
				{
					jason->parentDir = (char*)malloc(strlen(buff));
					strcpy(jason->parentDir, parseFilename(buff, 'd'));
					break;
				}
				buff[strlen(buff)] = c;
			}

			i += 1;
		}
		else if (!strcmp(buff, "size"))
		{
			i += 3;
			memset(buff, 0, strlen(buff));

			for (; i < strlen(string); ++i)
			{
				char c = string[i];
				if (c == '\"')
				{
					jason->size = (unsigned int)atoi(buff);
					break;
				}
				else
				{
					buff[strlen(buff)] = c;
				}
			}
		}
		else
		{
			continue;
		}
		memset(buff, 0, strlen(buff));
	}
	return 1;
}