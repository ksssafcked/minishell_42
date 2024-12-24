#!/bin/bash

MINISHELL_PATH="./minishell"
TEST_DIR="minishell_tests"
DIFF_FILE="$TEST_DIR/differences.txt"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Création du répertoire de test s'il n'existe pas
mkdir -p "$TEST_DIR"

# Fonction pour afficher le contenu d'un fichier avec un préfixe pour chaque ligne
display_content() {
    local file="$1"
    local prefix="$2"
    while IFS= read -r line; do
        echo -e "${prefix}${line}"
    done < "$file"
}

# Fonction pour exécuter une commande dans bash et dans minishell
run_test() {
    local cmd="$1"
    local test_name="$2"
    
    # Exécution dans bash
    echo "$cmd" | bash > "$TEST_DIR/bash_output.txt" 2> "$TEST_DIR/bash_error.txt"
    bash_exit=$?
    
    # Création d'un script d'entrée pour minishell
    echo "$cmd" > "$TEST_DIR/minishell_input.txt"
    
    # Exécution dans minishell
    $MINISHELL_PATH < "$TEST_DIR/minishell_input.txt" > "$TEST_DIR/minishell_full_output.txt" 2> "$TEST_DIR/minishell_error.txt"
    minishell_exit=$?
    
    # Suppression du prompt de la sortie de minishell
    sed -i '/^minishell> exit$/d' $TEST_DIR/minishell_full_output.txt
    sed '1d' "$TEST_DIR/minishell_full_output.txt" > "$TEST_DIR/minishell_output.txt"
    
    # Comparaison des sorties
    if diff "$TEST_DIR/bash_output.txt" "$TEST_DIR/minishell_output.txt" > /dev/null &&
       diff "$TEST_DIR/bash_error.txt" "$TEST_DIR/minishell_error.txt" > /dev/null &&
       [ $bash_exit -eq $minishell_exit ]; then
        echo -e "${GREEN}[PASS]${NC} $test_name"
    else
        echo -e "${RED}[FAIL]${NC} $test_name"
        echo -e "${YELLOW}Résultats pour le test : $test_name${NC}"
        echo -e "${BLUE}Commande : $cmd${NC}"
        echo -e "Code de sortie Bash : $bash_exit"
        echo -e "Code de sortie Minishell : $minishell_exit"
        
        echo -e "${YELLOW}Sortie standard :${NC}"
        echo -e "${BLUE}Bash :${NC}"
        display_content "$TEST_DIR/bash_output.txt" "  "
        echo -e "${BLUE}Minishell :${NC}"
        display_content "$TEST_DIR/minishell_output.txt" "  "
        
        echo -e "${YELLOW}Sortie d'erreur :${NC}"
        echo -e "${BLUE}Bash :${NC}"
        display_content "$TEST_DIR/bash_error.txt" "  "
        echo -e "${BLUE}Minishell :${NC}"
        display_content "$TEST_DIR/minishell_error.txt" "  "
        
        echo "----------------------------------------"

        # Enregistrement dans le fichier de différences
        {
            echo "Test: $test_name"
            echo "Commande: $cmd"
            echo "Code de sortie Bash : $bash_exit"
            echo "Code de sortie Minishell : $minishell_exit"
            echo "Sortie standard Bash :"
            cat "$TEST_DIR/bash_output.txt"
            echo "Sortie standard Minishell :"
            cat "$TEST_DIR/minishell_output.txt"
            echo "Sortie d'erreur Bash :"
            cat "$TEST_DIR/bash_error.txt"
            echo "Sortie d'erreur Minishell :"
            cat "$TEST_DIR/minishell_error.txt"
            echo "----------------------------------------"
        } >> "$DIFF_FILE"
    fi
}

# Suppression de l'ancien fichier de différences
rm -f "$DIFF_FILE"

# Tests
# run_test "echo Hello World" "Simple echo"
# run_test "echo -n Hello World" "Echo avec option -n"
# run_test "cd .. && pwd" "CD et PWD"
# run_test "export TEST=value && echo \$TEST" "Export et echo d'une variable"
# run_test "unset TEST && echo \$TEST" "Unset d'une variable"
# run_test "env | grep PATH" "Commande env"
# run_test "echo \$?" "Echo du statut de sortie"
# run_test "ls | grep test" "Pipe simple"
# run_test "cat < input.txt" "Redirection d'entrée"
# run_test "echo test > output.txt && cat output.txt" "Redirection de sortie"
# run_test "echo test >> output.txt && cat output.txt" "Redirection en mode append"
# run_test "echo 'Single quotes'" "Guillemets simples"
# run_test "echo \"Double quotes \$HOME\"" "Guillemets doubles avec variable"
# run_test "echo `echo nested command`" "Commande imbriquée"


# # Builtins
# run_test "echo Hello World" "Echo simple"
# run_test "echo -n No newline" "Echo sans nouvelle ligne"
# run_test "echo 'Single quotes'" "Echo avec guillemets simples"
# run_test "echo \"Double quotes\"" "Echo avec guillemets doubles"
# run_test "echo \$HOME" "Echo d'une variable d'environnement"
# run_test "echo \$?" "Echo du statut de sortie"
# run_test "cd .." "CD vers le répertoire parent"
# run_test "cd ~" "CD vers le répertoire home"
# run_test "cd /tmp" "CD vers un chemin absolu"
# run_test "pwd" "PWD simple"
# run_test "export NEW_VAR=value" "Export d'une nouvelle variable"
# run_test "unset NEW_VAR" "Unset d'une variable"
# run_test "env | grep PATH" "Env filtré"
# run_test "exit 0" "Exit avec code 0"
# run_test "exit 42" "Exit avec code non-zéro"

# # Redirections
# run_test "echo Hello > output.txt" "Redirection de sortie"
# run_test "echo World >> output.txt" "Redirection de sortie en mode append"
# run_test "cat < input.txt" "Redirection d'entrée"
# run_test "cat < input.txt > output.txt" "Redirection d'entrée et de sortie"
# run_test "echo Error >&2" "Redirection vers stderr"
# run_test "cat < nonexistent_file" "Redirection depuis un fichier inexistant"
# run_test "echo Hello > /dev/null" "Redirection vers /dev/null"
# run_test "echo Hello > file1 > file2 | cat file3" "Redirections multiples"

# # Pipes
# run_test "echo Hello | cat" "Pipe simple"
# run_test "ls | grep test" "Pipe avec grep"
# run_test "echo Hello | rev" "Pipe avec rev"
# run_test "cat /etc/passwd | grep root | wc -l" "Pipe multiple"
# run_test "echo Hello | cat | cat | cat" "Pipe en chaîne"
# run_test "ls -l | sort | head -n 5" "Pipe complexe"
# run_test "echo Hello | xargs echo" "Pipe avec xargs"
# run_test "echo Hello | sed 's/Hello/Bonjour/'" "Pipe avec sed"
# run_test "echo Hello | awk '{print \$1}'" "Pipe avec awk"
# run_test "ps aux | grep bash | grep -v grep" "Pipe avec exclusion"

# # Variables d'environnement
# run_test "echo \$PATH" "Echo d'une variable d'environnement existante"
# run_test "echo \$NONEXISTENT_VAR" "Echo d'une variable d'environnement inexistante"
# run_test "export TEST=Hello | echo \$TEST" "Export et echo d'une variable"
# run_test "export TEST='Hello World' | echo \$TEST" "Export et echo d'une variable avec espaces"
# run_test "export TEST=\"Hello World\" | echo \$TEST" "Export et echo d'une variable avec guillemets doubles"
# run_test "export TEST=Hello | unset TEST | echo \$TEST" "Export, unset et echo d'une variable"
# run_test "echo \$?" "Echo du statut de sortie après une commande réussie"
# run_test "ls /nonexistent | echo \$?" "Echo du statut de sortie après une commande échouée"
# run_test "export TEST=\$HOME | echo \$TEST" "Export d'une variable utilisant une autre variable"
# run_test "export TEST=\$HOME/test | echo \$TEST" "Export d'une variable avec concaténation"

# Guillemets et échappement
run_test "echo \"Hello 'World'\"" "Guillemets doubles contenant des guillemets simples"
run_test "echo 'Hello \"World\"'" "Guillemets simples contenant des guillemets doubles"
run_test "echo \"Hello \$HOME\"" "Guillemets doubles avec expansion de variable"
run_test "echo 'Hello \$HOME'" "Guillemets simples sans expansion de variable"
run_test "echo \"Hello \\\"World\\\"\"" "Guillemets doubles échappés"
run_test "echo 'Hello \\\'World\\\''" "Guillemets simples échappés"
run_test "echo Hello\\ World" "Espace échappé"
run_test "echo \"Hello\nWorld\"" "Nouvelle ligne dans des guillemets doubles"
run_test "echo 'Hello\nWorld'" "Nouvelle ligne dans des guillemets simples"
run_test "echo \"*\"" "Astérisque dans des guillemets doubles"

# # Commandes multiples
# run_test "echo Hello; echo World" "Commandes séparées par point-virgule"
# run_test "echo Hello && echo World" "Commandes avec AND logique"
# run_test "echo Hello || echo World" "Commandes avec OR logique"
# run_test "false || echo \"Command failed\"" "OR logique après échec"
# run_test "true && echo \"Command succeeded\"" "AND logique après succès"
# run_test "echo Hello; false; echo World" "Commandes multiples avec échec"
# run_test "(echo Hello; echo World)" "Commandes groupées"
# run_test "true && (echo Hello; echo World)" "Commandes groupées avec AND logique"
# run_test "false || (echo Hello; echo World)" "Commandes groupées avec OR logique"
# run_test "echo Hello & echo World & wait" "Commandes en arrière-plan"

# # Expansions et substitutions
# run_test "echo *" "Expansion de glob"
# run_test "echo ~" "Expansion du tilde"
# run_test "echo \$(echo Hello)" "Substitution de commande avec \$()"
# run_test "echo `echo Hello`" "Substitution de commande avec backticks"
# run_test "echo \$((1 + 1))" "Expansion arithmétique"
# run_test "echo {1..5}" "Expansion d'accolades (séquence)"
# run_test "echo {a,b,c}" "Expansion d'accolades (liste)"
# run_test "echo \${HOME:-default}" "Expansion de paramètre avec valeur par défaut"
# run_test "echo \${NONEXISTENT_VAR:=default}" "Expansion de paramètre avec assignation"
# run_test "echo \${#HOME}" "Expansion de paramètre (longueur)"

# # Cas particuliers et edge cases
# run_test " echo Hello" "Commande avec espace au début"
# run_test "echo Hello " "Commande avec espace à la fin"
# run_test "   echo    Hello    World   " "Commande avec plusieurs espaces"
# run_test "echo" "Echo sans arguments"
# run_test "echo ''" "Echo avec chaîne vide"
# run_test "echo \"\"" "Echo avec guillemets doubles vides"
# run_test "ls -l /proc/self/fd" "Liste des descripteurs de fichiers"
# run_test "echo Hello > /dev/full" "Redirection vers un périphérique plein"
# run_test "cat /dev/zero | head -c 10" "Lecture depuis /dev/zero"
# run_test "echo \$RANDOM" "Utilisation de \$RANDOM"

# # Tests supplémentaires
# run_test "echo \$((10#1010))" "Conversion de base"
# run_test "echo \${HOME#/*/}" "Suppression de préfixe"
# run_test "echo \${HOME%/*}" "Suppression de suffixe"
# run_test "echo \${HOME/o/O}" "Substitution simple"
# run_test "echo \${HOME//o/O}" "Substitution globale"
# run_test "echo \${HOME^}" "Mise en majuscule de la première lettre"
# run_test "echo \${HOME^^}" "Mise en majuscule de toute la chaîne"
# run_test "echo \${HOME,}" "Mise en minuscule de la première lettre"
# run_test "echo \${HOME,,}" "Mise en minuscule de toute la chaîne"
# run_test "echo \${!H*}" "Expansion de noms de variables"

# # Tests de robustesse
# run_test "echo Hello >&2 | cat" "Redirection d'erreur et pipe"
# run_test "cat < <(echo Hello)" "Redirection depuis une substitution de processus"
# run_test "echo Hello > >(cat)" "Redirection vers une substitution de processus"
# run_test "echo \$((16#FF))" "Conversion hexadécimale"
# run_test "echo \$((2#1010))" "Conversion binaire"
# run_test "echo \$(( (1 + 2) * 3 ))" "Expression arithmétique complexe"
# run_test "echo \${HOME:0:5}" "Sous-chaîne"
# run_test "echo \${HOME:(-5)}" "Sous-chaîne à partir de la fin"
# run_test "echo \${!HOME*}" "Liste des variables commençant par HOME"
# run_test "echo \${#HOME[@]}" "Nombre d'éléments dans un tableau"