/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/15 13:58:39 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/15 13:59:21 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "../include/env.h"
#include "../include/utils.h"

int g_cmd_error_code = 0; 

static void set_error_and_code(const char *cmd, const char *msg, int code)
{
    fprintf(stderr, "minishell: %s: %s\n", cmd, msg);
    g_cmd_error_code = code;
}

char *get_command_path(t_env *env, char *cmd)
{
    struct stat st;
    g_cmd_error_code = 0;

    if (!cmd || !*cmd)
    {
        set_error_and_code(cmd, "command not found", 127);
        return NULL;
    }

    if (ft_strchr(cmd, '/'))
    {
        if (stat(cmd, &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
            {
                set_error_and_code(cmd, "Is a directory", 126);
                return NULL;
            }
            if (access(cmd, X_OK) != 0)
            {
                if (errno == EACCES)
                    set_error_and_code(cmd, "Permission denied", 126);
                else
                    set_error_and_code(cmd, "No such file or directory", 127);
                return NULL;
            }
            return ft_strdup(cmd);
        }
        else
        {
            set_error_and_code(cmd, "No such file or directory", 127);
            return NULL;
        }
    }

    char *env_path = env_get_value(env, "PATH");
    if (!env_path || !*env_path)
    {
        set_error_and_code(cmd, "command not found", 127);
        return NULL;
    }

    char **dirs = NULL;
    {
        int i = 0;
        char *p = env_path;
        int count = 1;
        while (*p)
        {
            if (*p == ':') count++;
            p++;
        }
        dirs = malloc(sizeof(char*)*(count+1));
        if (!dirs)
            return NULL;
        p = env_path;
        int start = 0;
        i = 0;
        for (int c = 0; env_path[c]; c++)
        {
            if (env_path[c] == ':')
            {
                int length = c - start;
                dirs[i] = malloc(length+1);
                strncpy(dirs[i], env_path+start, length);
                dirs[i][length] = '\0';
                i++;
                start = c+1;
            }
        }
        if (start <= (int)strlen(env_path))
        {
            dirs[i] = ft_strdup(env_path+start);
            i++;
        }
        dirs[i] = NULL;
    }

    int i=0;
    while (dirs[i])
    {
        char *tmp = ft_strjoin(dirs[i], "/");
        char *full = ft_strjoin(tmp, cmd);
        free(tmp);
        if (stat(full, &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
            {
                free(full);
                int j=0;while(dirs[j]) free(dirs[j++]); free(dirs);
                set_error_and_code(cmd, "Is a directory", 126);
                return NULL;
            }
            if (access(full, X_OK) != 0)
            {
                free(full);
                int j=0;while(dirs[j]) free(dirs[j++]); free(dirs);
                set_error_and_code(cmd, "Permission denied", 126);
                return NULL;
            }
            int j=0;while(dirs[j]) free(dirs[j++]); free(dirs);
            return full;
        }
        free(full);
        i++;
    }
    int j=0;while(dirs[j]) free(dirs[j++]); free(dirs);
    set_error_and_code(cmd, "command not found", 127);
    return NULL;
}
