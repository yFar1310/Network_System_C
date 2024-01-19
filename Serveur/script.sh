#!/bin/bash





touch Stats/fichier.log
touch Stats/stats_globales.txt

fichier_sortie="Stats/Historiques"


# Convert the log file to UTF-8 and filter out non-printable characters
iconv -f ISO-8859-1 -t UTF-8 "$fichier_sortie" | tr -cd '[:print:]\n' | awk '{print; print ""}' > Stats/fichier_utf8_filtered.log


awk '
    BEGIN {
        FS="[ ()]+";
    }
   

    /Robot .* a choisi la carte/ {
        cartes_tour[current_tour, $2] = $0;
        next;
    }

    /FIN TOUR NUMERO/ {
        print "Fin du tour numéro : " $5;
        for (joueur in affiche_joueur) {
            if (joueur ~ /^Robot/) {
                print "Carte jouée par " joueur " : " cartes_tour[current_tour, joueur];
            }
        }
        next;
    }

    /FIN MANCHE NUMERO/ {
        print "Fin de la manche numéro : " $5;
        next;
    }

    /DEBUT MANCHE NUMERO/ {
        print "\nDébut de la manche numéro : " $5;
         print "";
        next;
    }

    /DEBUT TOUR NUMERO/ {
        current_tour = $5;
        print "\nDébut du tour numéro : " current_tour;
         print "";
        next;
    }

    /Scores et information des joueurs/ {
        in_scores_block = 1;
        next;
    }

   

   in_scores_block {
    if ($1 == "Robot" || $1 == "Joueur") {
        joueur = $2;
        if ($1 == "Robot") {
            score[joueur] = $6;
        } else {
            score[joueur] = $8;
        }
        groupe = $4;
        parties_jouees[joueur]++;
        total_tours[joueur]++;
        total_scores_joueur[joueur] += score[joueur];
        if (!affiche_joueur[joueur]) {
            if (score[joueur] == "" || score[joueur] == "carte") {
                print "\nJoueur " joueur " (Groupe " groupe ", Score non disponible)";
            } else {
                print "\nJoueur " joueur " (Groupe " groupe " Score " score[joueur] ")";
                print "";
            }
            affiche_joueur[joueur] = 1;
        }
    }
    next;
}

    
  /#############  TABLE DU JEU  #############/ {
    in_table_block = 1;
    table_contents = "";
    next;
}

in_table_block {
    # Ajoutez cette condition pour traiter chaque ligne dans le bloc de table
    if ($0 ~ /^\s*\[.*\]\s*$/) {
        print  $0;  # Déclaration de débogage
        print "";
        table_contents = table_contents $0 "\n";
    }

    if ($0 ~ /^\s*\]\s*$/) {
        in_table_block = 0;
        print "\nTable de jeu :\n" table_contents;
        table_contents = "";  # Réinitialise le contenu de la table
    }

    next;
}



   END {
   
    print "\nStatistiques globales :\n";

    total_scores_globaux = 0;
    total_joueurs_humains = 0;
    total_robots = 0;

    for (joueur in score) {
        if (score[joueur] != "") {
            total_scores_globaux += score[joueur];
            if (joueur ~ /^Robot/) {
                total_robots++;
            } else {
                total_joueurs_humains++;
            }
            

            print joueur " : Score = " score[joueur] ",Tours joués = " total_tours[joueur];
            print "";
        }
    }

    moyenne_totale = total_scores_globaux / (total_joueurs_humains + total_robots);
    print "\nScore total de toutes les parties = " total_scores_globaux;
    printf "Nombre total de joueurs humains = %d\n", total_joueurs_humains;
    printf "Nombre total de joueurs robots = %d\n", total_robots;
    print "Moyenne des scores de toutes les parties = " moyenne_totale;

    # Ajout des statistiques pour chaque robot
    print "\nStatistiques par robot :\n";
    for (robot in total_tours) {
        if (total_tours[robot] > 0 && robot ~ /^Robot/) {
            # Calcul de la moyenne
            moyenne_robot = total_scores_joueur[robot] / total_tours[robot];

            # Calcul de la variance
            variance_robot = 0;
            for (i = 1; i <= parties_jouees[robot]; i++) {
                # Assurez-vous que la partie de robot, i existe
                if (score[robot] != "") {
                    variance_robot += (score[robot] - moyenne_robot) ^ 2;
                } else {
                    # Ajout de messages temporaires pour le débogage
                    print "Avertissement: Score manquant pour " robot ", partie " i;
                }
            }
            variance_robot /= parties_jouees[robot];

            # Affichage des statistiques pour le robot
            print robot " : Moyenne = " moyenne_robot ", Variance = " variance_robot;
        }
    }

    # Ajout des statistiques pour chaque joueur humain
    print "\nStatistiques par joueur humain :\n";
    for (joueur in total_tours) {
        if (total_tours[joueur] > 0 && joueur !~ /^Robot/) {
            # Calcul de la moyenne
            moyenne_joueur = total_scores_joueur[joueur] / total_tours[joueur];

            # Calcul de la variance
            variance_joueur = 0;
            for (i = 1; i <= parties_jouees[joueur]; i++) {
                # Assurez-vous que la partie de joueur, i existe
                if (score[joueur] != "") {
                    variance_joueur += (score[joueur] - moyenne_joueur) ^ 2;
                } else {
                    # Ajout de messages temporaires pour le débogage
                    print "Avertissement: Score manquant pour " joueur ", partie " i;
                }
            }
            variance_joueur /= parties_jouees[joueur];

            # Affichage des statistiques pour le joueur humain
            print joueur " : Moyenne = " moyenne_joueur ", Variance = " variance_joueur;
        } else if (joueur !~ /^Robot/) {
            print joueur " : Aucun tour joué, aucune statistique disponible.";
            print "ok";
        }
    }

    }
' Stats/fichier_utf8_filtered.log > Stats/fichier.log



# Convertir le fichier journal au format PDF en utilisant Pandoc avec le moteur LaTeX TeX Live
pandoc -o Stats/Historiques.pdf Stats/fichier_utf8_filtered.log --pdf-engine=xelatex --verbose


# Extraction et conversion des statistiques globales en PDF
awk '/^Statistiques globales :/,/^ok/{print}' "Stats/fichier.log" > "Stats/stats_globales.txt"

# Vérification si le fichier stats_globales.txt a été créé et n'est pas vide
if [ -s "Stats/stats_globales.txt" ]; then
    pandoc -o Stats/Stats_Globales.pdf "Stats/stats_globales.txt" --pdf-engine=xelatex --verbose
else
    echo "Erreur: Le fichier stats_globales.txt n'a pas été généré correctement ou est vide."
fi

