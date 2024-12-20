/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 16:52:12 by lsaiti            #+#    #+#             */
/*   Updated: 2024/12/17 17:09:52 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "../include/parser.h"
#include "../include/utils.h"
#include "../include/built_in.h"
#include "../include/env.h"

char	*ft_substr(const char *s, unsigned int start)
{
	char	*extract;
	size_t	i;
	size_t len;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		len = 0;
	else
		len = ft_strlen(s + start);
	extract = malloc(sizeof(char) * (len + 1));
	if (!extract)
		return (NULL);
	while (start-- && *s)
		s++;
	i = 0;
	while (*s && i < len)
		extract[i++] = *(s++);
	extract[i] = '\0';
	return (extract);
}

t_env	*get_env(t_env *env, char *env_name)
{
	t_env	*final;

	final = env;
	while (final && !ft_strnstr(final->var, env_name, ft_strlen(env_name)))
		final = final->next;
	return (final);
}

char	*delete_one_path(char *path)
{
	size_t	len;
	size_t	i;
	char	*new_path;

	if (!path)
		return (NULL);
	len = ft_strlen(path);
	if (path[len - 1] == '/' && len > 0)
		len--;
	while (--len)
	{
		if (path[len] == '/')
			break ;
	}
	new_path = malloc(sizeof(char) * (len + 1));
	if (!new_path)
		return (NULL);
	i = 0;
	while (i < len)
	{
		new_path[i] = path[i];
		i++;
	}
	free(path);
	new_path[i] = '\0';
	return (new_path);
}

char	*get_corr_path(char *dir)
{
	DIR	*directory;
	char	*path;

	directory = opendir(dir);
	if (!directory)
		return (fprintf(stderr, "minishell: %s: no such file or directory\n", dir), NULL);
	closedir(directory);
	path = NULL;
	if (!ft_strncmp(dir, ".", 1))
	{
		path = getcwd(path, 0);
		if (!ft_strncmp(dir, "..", 2))
			path = delete_one_path(path);
	}
	else if (dir[0] != '/')
	{
		path = getcwd(path, 0);
		path = ft_strjoin(path, "/", path);
		path = ft_strjoin(path, dir, path);
	}
	// printf("%s", path);
	return (path);
}

char	*get_path(t_env *env, char *dir)
{
	char *full_path;

	if (!dir || ft_strcmp(dir, "~") == 0)
	{
		dir = (get_env(env, "HOME="))->var;
		full_path = ft_substr(dir, 5);
	}
	else
	{
		full_path = get_corr_path(dir);
	}
	return (full_path);
}

t_env	*cd_do_cmd(t_env *env, char *new_dir)
{
	char *curr_dir;
	t_env *pwd;
	t_env *oldpwd;

	new_dir = get_path(env, new_dir);
	if (!new_dir)
		return (env);
	pwd = get_env(env, "PWD=");
	oldpwd = get_env(env, "OLDPWD=");
	if (pwd && oldpwd)
	{
		curr_dir = ft_substr(pwd->var,4);
		free(oldpwd->var);
		oldpwd->var = ft_strjoin("OLDPWD=", curr_dir, NULL);
		free(pwd->var);
		chdir(new_dir);
		pwd->var = ft_strjoin("PWD=", new_dir, NULL);
		free(curr_dir);
		free(new_dir);
		return (env);
	}
	free(new_dir);
	return (env);
}

char	*get_var(char *var) // renvoie le nom de variable, exemple : VAR=VALEUR renvoie VAR
{
	char	*str;
	int	i;
	int	j;
	
	i = 0;
	while (var[i] && var[i] != '=')
		i++;
	str = malloc(sizeof(char) * (i + 1));
	if (!str)
		return (NULL);
	j = 0;
	while (j < i)
	{
		str[j] = var[j];
		j++;
	}
	str[j] = '\0';
	return (str);
}

// check si la variable qu'on veut creer est sous la bonne forme
void	export_cmd(t_env *env, char *new_env)
{
	t_env	*target_env;
	t_env	*new_target;
	char	*var;

	if (!new_env || !env)
		return ;
	var = get_var(new_env);
	target_env = get_env(env, var);
	if (target_env)
	{
		free(target_env->var);
		new_target = env_new_elem(new_env);
		target_env->var = new_target->var;
		free(new_target);
	}
	else if (!target_env)
	{
		target_env = env;
		while (target_env && target_env->next)
			target_env = target_env->next;
		target_env->next = env_new_elem(new_env);
	}
	free(var);
}


// check si la variable est bonne sinon seg fault
void	unset_cmd(t_env *env, char *delete_var)
{
	t_env *to_delete;

	if (!env)
		return ;
	to_delete = get_env(env, delete_var);
	while (env->next && env->next != to_delete)
		env = env->next;
	env->next = to_delete->next;
	free(to_delete->var);
	free(to_delete); 
}

void	env_cmd(t_env *env)
{
	if (!env)
		return ;
	while (env)
	{
		printf("%s\n", env->var);
		env = env->next;
	}
	return ;
}

void	echo_cmd(char **args)
{
	size_t	i;

	i = 0;
	if (args[i])
	{
		if (!ft_strcmp(args[i], "-n"))
			i++;
		while (args[i])
			printf("%s", args[i++]);
	}
	if (*args && (ft_strcmp(*args, "-n")))
		printf("\n");
}
