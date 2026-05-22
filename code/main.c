#define F_CPU 16000000UL


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "app.h"




int main()
{

	if( !app_init() )
	{
		return -1;
	}

	app_run();

	return 0;
}

