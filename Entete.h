#ifndef __ENTETE_H_
#define __ENTETE_H_

#include <cstdio>
#include <cstdlib> 
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>  
#include <cmath>
#include <vector>
#include <functional>
using namespace std;


struct TProblem									//**D�finition du probl�me:
{
	std::string Nom;							//**Nom du fichier de donn�es
	int NbVilles;								//**Taille du probleme: Nombre de villes
	std::vector<std::vector <int> > Distance;	//**Distance entre chaque paire de villes. NB: Tableau de 0 � NbVilles-1.  Indice 0 utilis�.
};

struct TSolution								//**D�finition d'une solution: 
{
	std::vector <int> Seq;				//**S�quence dans laquelle les villes sont visit�es. NB: Tableau de 0 � NbVilles-1.
	long FctObj;						//**Valeur de la fonction obj: Sommation des distances incluant le retour au point initial
	bool Valide;						//**Etat de validit� de la solution (pr�sence de chacune des villes une et une seule fois)
	int NbEvaltoGet;					//**Nombre d'evalutions de solutions necessaires pour trouver cette solution... pour �valuer convergence	//***MODIF***
};

struct TAlgo
{
	int		LngListeTabous;				//**Longueur de la liste des tabous													//***NEW***
	
	int		CptEval;					//**COMPTEUR DU NOMBRE DE SOLUTIONS EVALUEES. SERT POUR CRITERE D'ARRET.
	int		NB_EVAL_MAX;				//**CRITERE D'ARRET: MAXIMUM "NB_EVAL_MAX" EVALUATIONS.
	int		TailleVoisinage;			//**Nombre de solutions voisines g�n�r�es � chaque it�ration
	long	FctObjSolDepart;			//**Valeur de la fonction objectif de la solution au d�part de l'algorithme
};

#endif