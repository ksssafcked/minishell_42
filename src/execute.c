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
#include "../include/env.h"
#include "../include/utils.h"

//Verifie si le path pointe vers un fichier executable (genre a.out).
static int is_executable(char *path)
{
    struct stat st; // stat est une structure definie dans <sys/stat.h> qui contient des informations sur un fichier ou un repertoire.
	// Cette structure sera utilisee pour stocker les metadonnees du fichier specifie par le Path.
    if (stat(path, &st) == 0 && (st.st_mode & S_IXUSR)) // on extrait les metadonnes du fichier pointe par path et les stocks dans st. // == 0 on verifie que le fichier existe // st.st_mode ... verifie qu on peut l exec.
        return (1);
    return (0);
}

// Si commande contient un '/', on essaie directement execve.
// Si is_executable(path) est verifie, on renvoie un ft_strdup(path) sinon Null.
static char *try_direct_path(char *cmd)
{
    if (ft_strchr(cmd, '/'))
    {
        if (is_executable(cmd))
            return (ft_strdup(cmd));
    }
    return (NULL);
}

// Cherche la commande dans le PATH si pas de '/' dans cmd.
static char *find_executable(t_env *env, char *cmd)
{
    char *path;
	char **dirs;
	char *full_path;
	char *p;
	int i;
	int j;
	int k;
	int start;
	int idx;
	int count;
	int len_dir;
	int len_cmd;

	path = env_get_value(env, "PATH");
    if (!path)
        return (NULL); // Pas de PATH, on ne peut pas chercher
    full_path = NULL;
    i = 0;

    // On split sur ':' 
    // Ici, fonction ft_split_char(path, ':') qui split sur ':'
    p = ft_strdup(path);
    if (!p)
        return (NULL);
    count = 1;
	j = 0;
    while (p[j])
	{
        if (p[j] == ':')
            count++;
		j++;
	}
    dirs = (char **)malloc((count + 1) * sizeof(char *));
    if (!dirs)
    {
        free(p);
        return (NULL);
    }
    // Split basique sur ':'
    {
        start = 0; 
        idx = 0;
		k = 0;
        while (p[k])
        {
            if (p[k] == ':')
            {
                p[k] = '\0';
                dirs[idx++] = ft_strdup(p + start);
                start = k+1;
            }
			k++;
        }
        dirs[idx++] = ft_strdup(p + start);
        dirs[idx] = NULL;
    }
    free(p);

    // On parcourt chaque repertoire
	i = 0;
    while (dirs[i])
    {
        len_dir = ft_strlen(dirs[i]);
        len_cmd = ft_strlen(cmd);
        full_path = (char *)malloc((len_dir + 1 + len_cmd + 1) * sizeof(char));
        if (!full_path)
            break;
        sprintf(full_path, "%s/%s", dirs[i], cmd);

        if (is_executable(full_path))
        {
            // On libère dirs
			j = 0;
            while (dirs[j])
			{
				free(dirs[j]);
				j++;
			}
            free(dirs);
            return (full_path);
        }
        free(full_path);
        full_path = NULL;
		i++;
    }
    // On libere dirs si pas trouve
	j = 0;
    while (dirs[j])
	{
		free(dirs[j]);
		j++;
	}
    free(dirs);
    return (NULL);
}

  // Si cmd contient '/', on tente l'execution directe
 // Sinon, on cherche dans le PATH
char *get_command_path(t_env *env, char *cmd)
{
    char *path;

	path = try_direct_path(cmd);
    if (path)
        return (path);
    return (find_executable(env, cmd));
}
