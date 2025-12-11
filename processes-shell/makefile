wish: wish.o logger.o path_manager.o cmd_executor.o
	gcc -Wall -Werror -o wish wish.o logger.o path_manager.o cmd_executor.o

wish.o: wish.c
	gcc -g -c -Wextra -O0 -Wall -Werror -o wish.o wish.c

logger.o: logger.c
	gcc -g -c -Wextra -O0 -Wall -Werror -o logger.o logger.c

path_manager.o: path_manager.c
	gcc -g -c -Wextra -O0 -Wall -Werror -o path_manager.o path_manager.c

cmd_executor.o: cmd_executor.c
	gcc -g -c -Wextra -O0 -Wall -Werror -o cmd_executor.o cmd_executor.c
