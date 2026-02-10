#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define MAX_CLIENTS 100
#define MAX_COMPTES 100

typedef struct {
    int id_client;
    char nom[50];
    char prenom[50];
    char profession[50];
    char num_tel[20];
    int pin;
} Client;

typedef struct {
    int id_compte;
    int id_client;
    float solde;
    char date_ouverture[15];
} Compte;

void Color(int t, int f) {
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, f * 16 + t);
}

void C_FOND()    { Color(15, 1); }
void C_TITRE()   { Color(14, 1); }
void C_SOUS_TITRE() { Color(11, 1); }
void C_SUCCES()  { Color(10, 1); }
void C_ERREUR()  { Color(12, 1); }
void C_LABEL()   { Color(11, 1); }
void C_VALEUR()  { Color(15, 1); }
void C_SELECT()  { Color(0, 14); }
void C_BORDURE() { Color(13, 1); }
void C_INFO()    { Color(7, 1); }
void C_RETOUR()  { Color(6, 1); }

void clean_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void saisirTexte(char *chaine, int taille) {
    if (fgets(chaine, taille, stdin) != NULL) {
        size_t len = strlen(chaine);
        if (len > 0 && chaine[len - 1] == '\n') chaine[len - 1] = '\0';
        else clean_stdin();
    }
}

void ajouterHistorique(const char *action) {
    FILE *f = fopen("historique.txt", "a");
    if (f == NULL) return;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(f, "[%02d/%02d/%d %02d:%02d] %s\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, action);
    fclose(f);
}

void afficherHistorique() {
    system("cls");
    FILE *f = fopen("historique.txt", "r");
    char ligne[256];

    C_TITRE(); printf("\n  HISTORIQUE DES OPERATIONS\n");
    C_BORDURE();
    printf("  +----------------------------------------------------------------------+\n");
    C_SOUS_TITRE();
    printf("  | DATE & HEURE      | DETAILS DE L'OPERATION                           |\n");
    C_BORDURE();
    printf("  +----------------------------------------------------------------------+\n");
    C_FOND();

    if (f == NULL) {
        C_ERREUR(); printf("  |                   AUCUN HISTORIQUE DISPONIBLE                        |\n"); C_FOND();
    } else {
        while (fgets(ligne, sizeof(ligne), f)) {
            ligne[strcspn(ligne, "\n")] = 0;
            C_BORDURE(); printf("  | "); C_FOND();
            printf("%-68s", ligne);
            C_BORDURE(); printf(" |\n"); C_FOND();
        }
        fclose(f);
    }
    C_BORDURE(); printf("  +----------------------------------------------------------------------+\n");
    C_INFO(); printf("\n  Appuyez sur une touche pour revenir...");
    getch();
}

void sauvegarderDonnees(Client clients[], int nb_clients, Compte comptes[], int nb_comptes) {
    FILE *fc = fopen("clients.txt", "w");
    if (fc) {
        for (int i = 0; i < nb_clients; i++)
            fprintf(fc, "%d;%s;%s;%s;%s;%d\n", clients[i].id_client, clients[i].nom, clients[i].prenom, clients[i].profession, clients[i].num_tel, clients[i].pin);
        fclose(fc);
    }
    FILE *fa = fopen("comptes.txt", "w");
    if (fa) {
        for (int i = 0; i < nb_comptes; i++)
            fprintf(fa, "%d;%d;%.2f;%s\n", comptes[i].id_compte, comptes[i].id_client, comptes[i].solde, comptes[i].date_ouverture);
        fclose(fa);
    }
}

void chargerDonnees(Client clients[], int *nb_clients, Compte comptes[], int *nb_comptes) {
    FILE *fc = fopen("clients.txt", "r");
    if (fc) {
        *nb_clients = 0;
        while (fscanf(fc, "%d;%49[^;];%49[^;];%49[^;];%19[^;];%d\n", &clients[*nb_clients].id_client, clients[*nb_clients].nom, clients[*nb_clients].prenom, clients[*nb_clients].profession, clients[*nb_clients].num_tel, &clients[*nb_clients].pin) == 6) (*nb_clients)++;
        fclose(fc);
    }
    FILE *fa = fopen("comptes.txt", "r");
    if (fa) {
        *nb_comptes = 0;
        while (fscanf(fa, "%d;%d;%f;%14[^\n]\n", &comptes[*nb_comptes].id_compte, &comptes[*nb_comptes].id_client, &comptes[*nb_comptes].solde, comptes[*nb_comptes].date_ouverture) == 4) (*nb_comptes)++;
        fclose(fa);
    }
}

int menuInteractif(char *titre, char *options[], int nb_opt) {
    int choix = 0;
    char touche;

    while (1) {
        system("cls");
        C_BORDURE(); printf("\n  +==========================================+\n");
        C_TITRE();   printf("   %s\n", titre);
        C_BORDURE(); printf("  +==========================================+\n\n");

        for (int i = 0; i < nb_opt; i++) {
            if (i == choix) {
                C_SELECT(); printf("  >> %-30s  \n", options[i]); C_FOND();
            } else {
                C_FOND(); printf("     %-30s  \n", options[i]);
            }
        }

        C_BORDURE(); printf("\n  +------------------------------------------+\n");
        C_INFO();    printf("   [HAUT/BAS] Naviguer  |  [ENTREE] Valider");
        C_FOND();

        touche = getch();

        if (touche == -32 || touche == 224) {
            touche = getch();
            if (touche == 72) { choix--; if (choix < 0) choix = nb_opt - 1; }
            if (touche == 80) { choix++; if (choix >= nb_opt) choix = 0; }
        } else if (touche == 13) return choix;
    }
}

int trouverClient(Client clients[], int nb, int id) { for(int i=0; i<nb; i++) if(clients[i].id_client==id) return i; return -1; }
int trouverCompte(Compte comptes[], int nb, int id) { for(int i=0; i<nb; i++) if(comptes[i].id_compte==id) return i; return -1; }

void afficherClientsLarge(Client clients[], int nb_clients) {
    system("cls");
    C_TITRE(); printf("\n  LISTE DES CLIENTS (%d)\n", nb_clients);
    C_BORDURE(); printf("  +------+-------------------------+----------------------+-----------------+\n");
    C_SOUS_TITRE(); printf("  | ID   | NOM & PRENOM            | PROFESSION           | TELEPHONE       |\n");
    C_BORDURE(); printf("  +------+-------------------------+----------------------+-----------------+\n");
    C_FOND();
    for(int i=0; i<nb_clients; i++) {
        char complet[50];
        sprintf(complet, "%s %s", clients[i].nom, clients[i].prenom);
        C_BORDURE(); printf("  | "); C_VALEUR(); printf("%-4d ", clients[i].id_client);
        C_BORDURE(); printf("| "); C_VALEUR(); printf("%-23s ", complet);
        C_BORDURE(); printf("| "); C_VALEUR(); printf("%-20s ", clients[i].profession);
        C_BORDURE(); printf("| "); C_VALEUR(); printf("%-15s ", clients[i].num_tel);
        C_BORDURE(); printf("|\n");
    }
    C_BORDURE(); printf("  +------+-------------------------+----------------------+-----------------+\n");
    C_INFO(); printf("\n  Appuyez sur une touche...");
    getch();
}

void afficherComptesLarge(Compte comptes[], int nb_comptes, Client clients[], int nb_clients) {
    system("cls");
    C_TITRE(); printf("\n  ETAT DES COMPTES (%d)\n", nb_comptes);
    C_BORDURE(); printf("  +--------+--------------+--------------+-------------------------+\n");
    C_SOUS_TITRE(); printf("  | COMPTE | DATE OUV.    | SOLDE (DH)   | PROPRIETAIRE            |\n");
    C_BORDURE(); printf("  +--------+--------------+--------------+-------------------------+\n");
    C_FOND();
    for(int i=0; i<nb_comptes; i++) {
        int p = trouverClient(clients, nb_clients, comptes[i].id_client);
        char nom[50];
        if (p != -1) sprintf(nom, "%s %s", clients[p].nom, clients[p].prenom);
        else strcpy(nom, "Inconnu");

        C_BORDURE(); printf("  | "); C_VALEUR(); printf("%-6d ", comptes[i].id_compte);
        C_BORDURE(); printf("| "); C_VALEUR(); printf("%-12s ", comptes[i].date_ouverture);
        C_BORDURE(); printf("| "); C_VALEUR(); printf("%-12.2f ", comptes[i].solde);
        C_BORDURE(); printf("| "); C_VALEUR(); printf("%-23s ", nom);
        C_BORDURE(); printf("|\n");
    }
    C_BORDURE(); printf("  +--------+--------------+--------------+-------------------------+\n");
    C_INFO(); printf("\n  Appuyez sur une touche...");
    getch();
}

void ajouterClient(Client clients[], int *nb_clients) {
    system("cls");
    if (*nb_clients >= MAX_CLIENTS) { C_ERREUR(); printf("Memoire pleine.\n"); C_FOND(); return; }
    int id;

    C_TITRE(); printf("\n  [NOUVEAU CLIENT]\n\n");

    C_LABEL(); printf("  ID Client "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    if(scanf("%d", &id)!=1) { clean_stdin(); return; } clean_stdin();

    if (id == 0) return;

    if(trouverClient(clients, *nb_clients, id)!=-1) { C_ERREUR(); printf("\n  ERREUR: ID deja pris.\n"); C_FOND(); getch(); return; }

    clients[*nb_clients].id_client = id;
    C_LABEL(); printf("  Nom        : "); C_VALEUR(); saisirTexte(clients[*nb_clients].nom, 50);
    C_LABEL(); printf("  Prenom     : "); C_VALEUR(); saisirTexte(clients[*nb_clients].prenom, 50);
    C_LABEL(); printf("  Profession : "); C_VALEUR(); saisirTexte(clients[*nb_clients].profession, 50);
    C_LABEL(); printf("  Telephone  : "); C_VALEUR(); saisirTexte(clients[*nb_clients].num_tel, 15);
    do { C_LABEL(); printf("  Code PIN (4 chiffres) : "); C_VALEUR(); scanf("%d", &clients[*nb_clients].pin); clean_stdin(); } while(clients[*nb_clients].pin < 1000 || clients[*nb_clients].pin > 9999);

    (*nb_clients)++;
    char msg[100]; sprintf(msg, "Nouveau Client ID: %d (%s %s)", id, clients[*nb_clients-1].nom, clients[*nb_clients-1].prenom);
    ajouterHistorique(msg);

    C_SUCCES(); printf("\n  SUCCES : Client enregistre !"); C_FOND(); getch();
}

void modifierClient(Client clients[], int nb_clients) {
    system("cls");
    int id, pos;
    C_TITRE(); printf("\n  [MODIFICATION CLIENT]\n\n");
    C_LABEL(); printf("  ID a modifier "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    if(scanf("%d", &id)!=1) { clean_stdin(); return; } clean_stdin();

    if (id == 0) return;

    pos = trouverClient(clients, nb_clients, id);
    if(pos == -1) { C_ERREUR(); printf("\n  Introuvable."); C_FOND(); getch(); return; }

    char *optionsModif[] = {"Modifier NOM", "Modifier PRENOM", "Modifier TELEPHONE", "Modifier PIN", "Retour"};
    int choix;

    while(1) {
        char titre[100];
        sprintf(titre, "MODIFIER CLIENT : %s %s", clients[pos].nom, clients[pos].prenom);
        choix = menuInteractif(titre, optionsModif, 5);

        system("cls");
        if(choix == 0) { C_LABEL(); printf("\n  Nouveau Nom : "); C_VALEUR(); saisirTexte(clients[pos].nom, 50); }
        else if(choix == 1) { C_LABEL(); printf("\n  Nouveau Prenom : "); C_VALEUR(); saisirTexte(clients[pos].prenom, 50); }
        else if(choix == 2) { C_LABEL(); printf("\n  Nouveau Tel : "); C_VALEUR(); saisirTexte(clients[pos].num_tel, 20); }
        else if(choix == 3) {
            do { C_LABEL(); printf("\n  Nouveau PIN (4 chiffres) : "); C_VALEUR(); scanf("%d", &clients[pos].pin); clean_stdin(); }
            while(clients[pos].pin < 1000 || clients[pos].pin > 9999);
        }
        else break;

        char msg[100]; sprintf(msg, "Modification Client ID: %d", id);
        ajouterHistorique(msg);

        C_SUCCES(); printf("\n  Modification enregistree !"); C_FOND(); getch();
    }
}

void supprimerClient(Client clients[], int *nb_clients) {
    system("cls");
    int id, pos; char c;
    C_TITRE(); printf("\n  [SUPPRESSION CLIENT]\n\n");
    C_LABEL(); printf("  ID Client "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    scanf("%d", &id); clean_stdin();

    if (id == 0) return;

    pos = trouverClient(clients, *nb_clients, id);
    if(pos != -1) {
        C_LABEL(); printf("  Confirmer suppression de %s ? (o/n) : ", clients[pos].nom); C_VALEUR(); c = getchar(); clean_stdin();
        if(c=='o'||c=='O') {
            char msg[100]; sprintf(msg, "Suppression Client ID: %d (%s)", id, clients[pos].nom);
            ajouterHistorique(msg);
            for(int i=pos; i < *nb_clients-1; i++) clients[i] = clients[i+1];
            (*nb_clients)--;
            C_SUCCES(); printf("\n  Supprime."); C_FOND();
        }
    } else { C_ERREUR(); printf("\n  Introuvable."); C_FOND(); }
    getch();
}

void ajouterCompte(Compte comptes[], int *nb_comptes, Client clients[], int nb_clients) {
    system("cls");
    int idc, idcl;
    C_TITRE(); printf("\n  [OUVERTURE COMPTE]\n\n");
    C_LABEL(); printf("  ID Nouveau Compte "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    scanf("%d", &idc); clean_stdin();

    if (idc == 0) return;

    if(trouverCompte(comptes, *nb_comptes, idc)!=-1) { C_ERREUR(); printf("\n  ID pris."); C_FOND(); getch(); return; }

    C_LABEL(); printf("  ID Proprietaire   : "); C_VALEUR(); scanf("%d", &idcl); clean_stdin();
    if(trouverClient(clients, nb_clients, idcl)==-1) { C_ERREUR(); printf("\n  Client inconnu."); C_FOND(); getch(); return; }

    comptes[*nb_comptes].id_compte = idc; comptes[*nb_comptes].id_client = idcl;
    C_LABEL(); printf("  Solde Initial (Min 1000) : "); C_VALEUR(); scanf("%f", &comptes[*nb_comptes].solde); clean_stdin();
    C_LABEL(); printf("  Date (JJ/MM/AAAA) : "); C_VALEUR(); saisirTexte(comptes[*nb_comptes].date_ouverture, 15);
    (*nb_comptes)++;

    char msg[100]; sprintf(msg, "Ouverture Compte ID: %d pour Client %d", idc, idcl);
    ajouterHistorique(msg);

    C_SUCCES(); printf("\n  Compte ouvert !"); C_FOND(); getch();
}

void depot(Compte comptes[], int nb_comptes) {
    system("cls");
    int id, pos; float mt;
    C_TITRE(); printf("\n  [DEPOT ESPECES]\n\n");
    C_LABEL(); printf("  ID Compte "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    scanf("%d", &id); clean_stdin();

    if (id == 0) return;

    pos = trouverCompte(comptes, nb_comptes, id);
    if(pos != -1) {
        C_LABEL(); printf("  Montant a deposer : "); C_VALEUR(); scanf("%f", &mt); clean_stdin();
        if(mt > 0) {
            comptes[pos].solde += mt;
            char msg[100]; sprintf(msg, "Depot +%.2f DH sur Compte %d", mt, id);
            ajouterHistorique(msg);
            C_SUCCES(); printf("\n  SUCCES. Nouveau solde : %.2f DH", comptes[pos].solde); C_FOND();
        } else { C_ERREUR(); printf("\n  Montant invalide."); C_FOND(); }
    } else { C_ERREUR(); printf("\n  Compte introuvable."); C_FOND(); }
    getch();
}

void retrait(Compte comptes[], int nb_comptes, Client clients[], int nb_clients) {
    system("cls");
    int id, pos, pin, cl; float mt;
    C_TITRE(); printf("\n  [RETRAIT GAB]\n\n");
    C_LABEL(); printf("  ID Compte "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    scanf("%d", &id); clean_stdin();

    if (id == 0) return;

    pos = trouverCompte(comptes, nb_comptes, id);
    if(pos != -1) {
        C_LABEL(); printf("  Montant (Max 700) : "); C_VALEUR(); scanf("%f", &mt); clean_stdin();
        cl = trouverClient(clients, nb_clients, comptes[pos].id_client);
        C_LABEL(); printf("  Code PIN : "); C_VALEUR(); scanf("%d", &pin); clean_stdin();
        if(pin == clients[cl].pin && comptes[pos].solde >= mt && mt <= 700) {
            comptes[pos].solde -= mt;
            char msg[100]; sprintf(msg, "Retrait -%.2f DH sur Compte %d", mt, id);
            ajouterHistorique(msg);
            C_SUCCES(); printf("\n  SUCCES. Nouveau solde : %.2f DH", comptes[pos].solde); C_FOND();
        } else { C_ERREUR(); printf("\n  ECHEC : PIN faux ou Solde insuffisant."); C_FOND(); }
    } else { C_ERREUR(); printf("\n  Compte introuvable."); C_FOND(); }
    getch();
}

void virement(Compte comptes[], int nb_comptes, Client clients[], int nb_clients) {
    system("cls");
    int ids, idd, ps, pd, pin, cl; float mt;
    C_TITRE(); printf("\n  [VIREMENT]\n\n");
    C_LABEL(); printf("  ID Emetteur "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    scanf("%d", &ids); clean_stdin();

    if (ids == 0) return;

    C_LABEL(); printf("  ID Recepteur: "); C_VALEUR(); scanf("%d", &idd); clean_stdin();
    ps = trouverCompte(comptes, nb_comptes, ids); pd = trouverCompte(comptes, nb_comptes, idd);

    if(ps != -1 && pd != -1) {
        C_LABEL(); printf("  Montant : "); C_VALEUR(); scanf("%f", &mt); clean_stdin();
        cl = trouverClient(clients, nb_clients, comptes[ps].id_client);
        C_LABEL(); printf("  PIN Emetteur : "); C_VALEUR(); scanf("%d", &pin); clean_stdin();
        if(pin == clients[cl].pin && comptes[ps].solde >= mt) {
            comptes[ps].solde -= mt; comptes[pd].solde += mt;
            char msg[100]; sprintf(msg, "Virement %.2f DH de %d vers %d", mt, ids, idd);
            ajouterHistorique(msg);
            C_SUCCES(); printf("\n  VIREMENT EFFECTUE."); C_FOND();
        } else { C_ERREUR(); printf("\n  ECHEC : PIN ou Solde."); C_FOND(); }
    } else { C_ERREUR(); printf("\n  Compte introuvable."); C_FOND(); }
    getch();
}

void supprimerCompte(Compte comptes[], int *nb_comptes) {
    system("cls");
    int id, pos; char c;
    C_TITRE(); printf("\n  [FERMETURE COMPTE]\n\n");
    C_LABEL(); printf("  ID Compte "); C_RETOUR(); printf("(0 pour annuler)"); C_LABEL(); printf(" : "); C_VALEUR();
    scanf("%d", &id); clean_stdin();

    if (id == 0) return;

    pos = trouverCompte(comptes, *nb_comptes, id);
    if(pos != -1) {
        C_LABEL(); printf("  Solde: %.2f. Fermer ? (o/n) : ", comptes[pos].solde); C_VALEUR(); c=getchar(); clean_stdin();
        if(c=='o'||c=='O') {
            char msg[100]; sprintf(msg, "Fermeture Compte ID: %d", id);
            ajouterHistorique(msg);
            for(int i=pos; i < *nb_comptes-1; i++) comptes[i] = comptes[i+1];
            (*nb_comptes)--;
            C_SUCCES(); printf("\n  Ferme."); C_FOND();
        }
    } else { C_ERREUR(); printf("\n  Introuvable."); C_FOND(); }
    getch();
}

int main() {
    C_FOND();
    system("cls");
    SetConsoleTitle("SYSTEME BANCAIRE PROFESSIONNEL");

    Client clients[MAX_CLIENTS];
    Compte comptes[MAX_COMPTES];
    int nb_clients = 0, nb_comptes = 0;

    chargerDonnees(clients, &nb_clients, comptes, &nb_comptes);

    char *menuMain[] = {"GESTION CLIENTS", "GESTION COMPTES", "OPERATIONS BANCAIRES", "QUITTER"};
    char *menuCli[]  = {"Ajouter un Client", "Modifier un Client", "Supprimer un Client", "Liste des Clients", "RETOUR"};
    char *menuCpt[]  = {"Ouvrir un Compte", "Fermer un Compte", "Liste des Comptes", "RETOUR"};
    char *menuOps[]  = {"Effectuer un Retrait", "Effectuer un Depot", "Faire un Virement", "Historique Operations", "RETOUR"};

    int c1, c2;

    while(1) {
        c1 = menuInteractif("ACCUEIL BANQUE", menuMain, 4);

        if(c1 == 0) {
            while(1) {
                c2 = menuInteractif("ESPACE CLIENTS", menuCli, 5);
                if(c2 == 0) ajouterClient(clients, &nb_clients);
                else if(c2 == 1) modifierClient(clients, nb_clients);
                else if(c2 == 2) supprimerClient(clients, &nb_clients);
                else if(c2 == 3) afficherClientsLarge(clients, nb_clients);
                else break;
                sauvegarderDonnees(clients, nb_clients, comptes, nb_comptes);
            }
        }
        else if(c1 == 1) {
            while(1) {
                c2 = menuInteractif("ESPACE COMPTES", menuCpt, 4);
                if(c2 == 0) ajouterCompte(comptes, &nb_comptes, clients, nb_clients);
                else if(c2 == 1) supprimerCompte(comptes, &nb_comptes);
                else if(c2 == 2) afficherComptesLarge(comptes, nb_comptes, clients, nb_clients);
                else break;
                sauvegarderDonnees(clients, nb_clients, comptes, nb_comptes);
            }
        }
        else if(c1 == 2) {
            while(1) {
                c2 = menuInteractif("TRANSACTIONS", menuOps, 5);
                if(c2 == 0) retrait(comptes, nb_comptes, clients, nb_clients);
                else if(c2 == 1) depot(comptes, nb_comptes);
                else if(c2 == 2) virement(comptes, nb_comptes, clients, nb_clients);
                else if(c2 == 3) afficherHistorique();
                else break;
                sauvegarderDonnees(clients, nb_clients, comptes, nb_comptes);
            }
        }
        else if(c1 == 3) {
            system("cls");
            C_SUCCES(); printf("\n  Au revoir !\n"); C_FOND();
            break;
        }
    }

    return 0;
}
