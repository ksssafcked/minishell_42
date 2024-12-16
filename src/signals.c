/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qudomino <qudomino@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 13:53:04 by qudomino          #+#    #+#             */
/*   Updated: 2024/12/14 13:53:22 by qudomino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include "../include/signals.h"


// On veut juste que le prompt soit réaffiché sur une nouvelle ligne

static void sigint_handler(int signum)
{
    (void)signum; // On intercepte le SIGINT de Ctrl-C mais on le cast en void car on l utilisera plus.

    write(STDOUT_FILENO, "\n", 1); // On passe a une nouvelle ligne. 
    rl_replace_line("", 0); // On efface la ligne en cours d edition.
    rl_on_new_line(); // re affiche le prompt.
    rl_redisplay(); // re affiche le prompt.
}

static void sigquit_handler(int signum)
{
    (void)signum; // On intercepte le SIGQUIT pour Ctrl-l 
    // On ne fait rien car mode interactif.
    // Pas de message, pas de nouvelle ligne, readline le fera.
}

 // Pour l'instant, SIGINT provoquera juste un retour à la ligne et un prompt.
 // SIGQUIT ne fera rien.

void init_signals(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;
    // Remplit les structures de maniere sure

	// Nettoie le masque des signaux.
    sigemptyset(&sa_int.sa_mask); // initialise le signal en vide.
    sa_int.sa_flags = SA_RESTART; // flag pour reassayer automatiquement les appels systems interrompu par ce signal comme readline.
    sa_int.sa_handler = &sigint_handler; 

    sigemptyset(&sa_quit.sa_mask); // initialise le signal en vide.
    sa_quit.sa_flags = SA_RESTART;
    sa_quit.sa_handler = &sigquit_handler;

    sigaction(SIGINT, &sa_int, NULL); // Associe SIGINT a sigint_handler 
    sigaction(SIGQUIT, &sa_quit, NULL); // Associe SIGQUIT a sigquit_handler 
}
