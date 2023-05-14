# BE RESEAU

## Contenu du dépôt
Ce dépôt inclut : 
  - README.md (ce fichier) qui décrit le contenu du projet et les commandes de compilation et exécution; 
  - tsock_texte et tsock_video : lanceurs pour les applications de test fournies.
  - dossier include : contenant les définitions des éléments fournis manipulés dans le cadre du BE.
  - dossier src : contenant l'implémantation des éléments fournis et complétés dans le cadre du BE.
  - src/mictcp.c : fichier au sein duquel nous avons effectué les modifications. 
  - BE_Réseau.pdf : fichier décrivant en 1/2 pages notre avancée dans le BE, et les choix effectués


## Compilation du protocole mictcp et lancement des applications de test fournies

Pour compiler mictcp et générer les exécutables des applications de test depuis le code source fourni, taper :

    make

Deux applicatoins de test sont fournies, tsock_texte et tsock_video, elles peuvent être lancées soit en mode puits, soit en mode source selon la syntaxe suivante:

    Usage: ./tsock_texte [-p|-s]
    Usage: ./tsock_video [[-p|-s] [-t (tcp|mictcp)]

Seul tsock_video permet d'utiliser, au choix, notre protocole mictcp ou une émulation du comportement de tcp sur un réseau avec pertes.

## Suivi des versions de notre travail

Les différentes versions demandées dans le sujet du BE sont disponibles dans la section main -> tags, où chaque tag correspond à une version donnée vx.
