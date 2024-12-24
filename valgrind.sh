#!/bin/sh
make re
valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=valgrind.supp --gen-suppressions=all --track-fds=yes ./minishell