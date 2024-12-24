/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsaiti <lsaiti@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 16:52:12 by lsaiti            #+#    #+#             */
/*   Updated: 2024/12/24 17:48:15 by lsaiti           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/built_in.h"
#include "../include/env.h"
#include "../include/parser.h"
#include "../include/utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char	*ft_substr_custom(const char *s, unsigned int start)
{
	char	*extract;
	size_t	len;
	size_t	i;

	if (!s)
		return (NULL);
	len = ft_strlen(s + start);
	extract = malloc(sizeof(char) * (len + 1));
	if (!extract)
		return (NULL);
	i = 0;
	while (i < len)
	{
		extract[i] = s[start + i];
		i++;
	}
	extract[len] = '\0';
	return (extract);
}

static t_env	*get_env_var(t_env *env, char *env_name)
{
	size_t	len;

	len = ft_strlen(env_name);
	while (env)
	{
		if (ft_strncmp(env->var, env_name, len) == 0 && env->var[len] == '=')
			return (env);
		env = env->next;
	}
	return (NULL);
}

void	change_pwd(t_env *env)
{
	char	*old_val;
	char	*new_pwd;
	char	*tmp;
	t_env	*old_env;
	t_env	*pwd_env;

	old_env = get_env_var(env, "OLDPWD");
	pwd_env = get_env_var(env, "PWD");
	if (!pwd_env || !old_env)
		return ;
	old_val = ft_substr_custom(pwd_env->var, 4);
	free(old_env->var);
	{
		tmp = ft_strjoin("OLDPWD=", old_val);
		old_env->var = tmp;
	}
	free(old_val);
	new_pwd = getcwd(NULL, 0);
	if (new_pwd)
	{
		free(pwd_env->var);
		pwd_env->var = ft_strjoin("PWD=", new_pwd);
		free(new_pwd);
	}
}

int	cd_cmd(t_env *env, char **args)
{
	char	*dir;
	char	*home;

	home = env_get_value(env, "HOME");
	if (args[1] && args[2])
		return (fprintf(stderr, "minishell: cd: too many arguments\n"), 1);
	if (!args[1] || !ft_strncmp(args[1], "~", 2))
	{
		if (!home)
			return (fprintf(stderr, "minishell: cd: HOME not set\n"), 1);
		dir = ft_strdup(home);
	}
	else
		dir = ft_strdup(args[1]);
	if (chdir(dir) != 0)
	{
		fprintf(stderr, "minishell: cd: %s: ", dir);
		perror("");
		free(dir);
		return (1);
	}
	change_pwd(env);
	free(dir);
	return (0);
}

int	exit_cmd(char **args, int last_exit_code, t_env *env)
{
	int		i;
	int		is_numeric;
	int		exit_code;
	int		arg_count;
	long	val;

	arg_count = 0;
	// fprintf(stderr, "exit\n");
	while (args[arg_count])
		arg_count++;
	if (!args[1])
	{
		exit_code = last_exit_code;
		env_free(env);
		exit(exit_code);
	}
	i = 0;
	is_numeric = 1;
	if (args[1][i] == '+' || args[1][i] == '-')
		i++;
	while (args[1][i])
	{
		if (!ft_isdigit((unsigned char)args[1][i]))
		{
			is_numeric = 0;
			break ;
		}
		i++;
	}
	if (!is_numeric)
	{
		fprintf(stderr, "minishell: exit: %s: numeric argument required\n",
			args[1]);
		env_free(env);
		exit(2);
	}
	if (arg_count > 2)
	{
		fprintf(stderr, "minishell: exit: too many arguments\n");
		return (1);
	}
	val = strtol(args[1], NULL, 10);
	exit_code = (unsigned char)val;
	env_free(env);
	exit(exit_code);
	return (0);
}

void	env_cmd(t_env *env)
{
	env_print(env);
}

void	pwd_cmd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("pwd");
		return ;
	}
	printf("%s\n", cwd);
	free(cwd);
}

int	is_nl_arg(char *args)
{
	if (*args != '-')
		return (0);
	args++;
	if (*args != 'n')
		return (0);
	while (*args)
	{
		if (*args != 'n')
			return (0);
		args++;
	}
	return (1);
}

void	echo_cmd(char **args)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (args[i] && !ft_strcmp(args[i], "-"))
		i++;
	if (args[i] && is_nl_arg(args[i]))
	{
		newline = 0;
		i++;
		while (args[i] && is_nl_arg(args[i]))
			i++;
	}
	while (args[i])
	{
		write(STDOUT_FILENO, args[i], strlen(args[i]));
		if (args[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (newline)
		write(STDOUT_FILENO, "\n", 1);
}

static int	cmp_strings(const void *a, const void *b)
{
	char *const *sa;
	char *const *sb;
	sa = (char *const *)a;
	sb = (char *const *)b;
	return (ft_strcmp(*sa, *sb));
}

static void	print_export(t_env *env)
{
	char	**arr;
	char	*eq;
	int		size;
	int		i;

	size = env_size(env);
	arr = env_to_char_array(env);
	if (!arr)
		return ;
	qsort(arr, size, sizeof(char *), cmp_strings);
	i = 0;
	while (i < size)
	{
		eq = ft_strchr(arr[i], '=');
		if (eq)
		{
			*eq = '\0';
			printf("declare -x %s=\"%s\"\n", arr[i], eq + 1);
			*eq = '=';
		}
		else
			printf("declare -x %s\n", arr[i]);
		i++;
	}
	i = 0;
	while (i < size)
		free(arr[i++]);
	free(arr);
}

static t_env	*env_add_var(t_env *env, const char *new_var)
{
	t_env	*cur;
	t_env	*new;

	cur = env;
	while (cur->next)
		cur = cur->next;
	new = env_new_elem((char *)new_var);
	if (!new)
		return (env);
	if (!ft_strchr(new_var, '='))
		new->printable = 0;
	cur->next = new;
	return (env);
}

static void	set_env_var(t_env *env, const char *new_var)
{
	char	*eq;
	size_t	var_len;
	t_env	*cur;

	eq = ft_strchr(new_var, '=');
	if (!eq)
		return ;
	var_len = eq - new_var;
	cur = env;
	while (cur)
	{
		if (!ft_strncmp(cur->var, new_var, var_len) && (cur->var[var_len] == '=' || cur->var[var_len] == '\0'))
		{
			free(cur->var);
			cur->var = ft_strdup(new_var);
			return ;
		}
		cur = cur->next;
	}
	env_add_var(env, new_var);
}

static void	set_env_no_value(t_env *env, const char *new_var)
{
	size_t	var_len;
	t_env	*cur;

	var_len = ft_strlen(new_var);
	cur = env;
	while (cur)
	{
		if (!ft_strncmp(new_var, cur->var, var_len))
			return ;
		cur = cur->next;
	}
	env_add_var(env, new_var);
}

static int	is_valid_identifier(const char *var)
{
	int	i;

	if (!((var[0] >= 'a' && var[0] <= 'z') || (var[0] >= 'A' && var[0] <= 'Z')
			|| var[0] == '_'))
		return (0);
	i = 1;
	while (var[i] && var[i] != '=')
	{
		if (!((var[i] >= 'a' && var[i] <= 'z') || (var[i] >= 'A'
					&& var[i] <= 'Z') || (var[i] >= '0' && var[i] <= '9')
				|| var[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

size_t	has_value(char *str)
{
	while (*str && *str != '=')
		str++;
	if (*str != '=')
		return (0);
	return (1);
}

void	export_cmd(t_shell *shell, t_env *env, char **args)
{
	int		exit_code;
	int	i;

	exit_code = 0;
	if (!args[1])
	{
		print_export(env);
		return ;
	}
	i = 0;
	while (args[++i])
	{
		if (!is_valid_identifier(args[i]))
		{
			fprintf(stderr, "minishell: export: %s: not a valid identifier\n",
				args[i]);
			exit_code = 1;
		}
		else if (has_value(args[i]))
			set_env_var(env, args[i]);
		else
		{
			if (args[i][0])
				set_env_no_value(env, args[i]);
		}
	}
	shell->last_exit_code = exit_code;
}

static t_env	*remove_env_var(t_env *prev, const char *var_name)
{
	
}


static t_env	*remove_env_var(t_env *env, const char *var_name)
{
	t_env	*prev;
	t_env	*cur;
	t_env	*temp;
	size_t	len;

	len = ft_strlen(var_name);
	prev = NULL;
	cur = env;
	while (cur)
	{
		if (ft_strncmp(cur->var, var_name, len) == 0 && cur->var[len] == '=')
		{
			if (prev == NULL)
			{
				temp = cur->next;
				return (free(cur->var), free(cur), temp);
			}
			else
			{
				prev->next = cur->next;
				return (free(cur->var), free(cur), env);
			}
		}
		prev = cur;
		cur = cur->next;
	}
	return (env);
}

t_env	*unset_cmd(t_env *env, char **args)
{
	int	i;

	if (!args[1])
		return (env);
	i = 1;
	while (args[i])
	{
		env = remove_env_var(env, args[i]);
		i++;
	}
	return (env);
}

int	run_builtin(t_shell *shell, char **args)
{
	int	ret;

	if (!args[0])
		return (0);
	if (ft_strcmp(args[0], "echo") == 0)
	{
		echo_cmd(args);
		shell->last_exit_code = 0;
		return (1);
	}
	else if (ft_strcmp(args[0], "cd") == 0)
	{
		shell->last_exit_code = cd_cmd(shell->env, args);
		return (1);
	}
	else if (ft_strcmp(args[0], "pwd") == 0)
	{
		pwd_cmd();
		shell->last_exit_code = 0;
		return (1);
	}
	else if (ft_strcmp(args[0], "export") == 0)
	{
		export_cmd(shell, shell->env, args);
		return (1);
	}
	else if (ft_strcmp(args[0], "unset") == 0)
	{
		shell->env = unset_cmd(shell->env, args);
		shell->last_exit_code = 0;
		return (1);
	}
	else if (ft_strcmp(args[0], "env") == 0)
	{
		env_cmd(shell->env);
		shell->last_exit_code = 0;
		return (1);
	}
	else if (ft_strcmp(args[0], "exit") == 0)
	{
		ret = exit_cmd(args, shell->last_exit_code, shell->env);
		shell->last_exit_code = ret;
		return (1);
	}
	return (0);
}
